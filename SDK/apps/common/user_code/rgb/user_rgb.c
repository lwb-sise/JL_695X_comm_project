#include "user_head.h"



USER_RGB_STATUS user_rgb;


static RGB_INFO rgb_info = {
    .exit_flag = 0, //;//退出标志
    .send_cn = 1,
    .on_off = 1,
    .Normal_mode = E_RGB_MODE_1,      //上电时候的灯效，播开机提示音之前
    .time = 30,                       //模式扫描时间
};

RGB_INFO *_this_ = NULL;
static u8 first_on=0;
u8 light_off=0;
u8 reback_light=0;

/**
 * @brief 伪随机数
 * 
 * @param start 起始值
 * @param end   结束值
 * @return u32  伪随机数 (取值范围: 起始值->结束值)
 */
static u32 random_number(u32 start, u32 end){
    if (end <= start) {
        return start;
    }
    return JL_RAND->R64L % (end - start + 1) + start;
}

#define USER_SAVE_MAX_DAC   (10)//单位100ms
void max_energy(int in_energy,int *out_max,int *out_mix){
    int dac_energy_max = 0,dac_energy_mix=65535;
    // int dac_energy = audio_dac_energy_get();
    static int energy_max_table[USER_SAVE_MAX_DAC]={0};
    static int energy_mix_table[USER_SAVE_MAX_DAC]={0};
    //保存最近3秒最大能量值
    // u32 cur_ms = timer_get_sec()%USER_SAVE_MAX_DAC;
    u32 cur_ms = (timer_get_ms()/100)%USER_SAVE_MAX_DAC;
    static u32 old_ms = 0;

    if(old_ms!=cur_ms){
        energy_max_table[cur_ms] = 0;
        energy_mix_table[cur_ms] = 65535;
        old_ms = cur_ms;
    }

    if(energy_max_table[cur_ms]<in_energy){
        energy_max_table[cur_ms] = in_energy;
    }

    if(energy_mix_table[cur_ms]>in_energy){
        energy_mix_table[cur_ms] = in_energy;
    }

    //最大能量值 最新能量值
    for(int i = 0;i<USER_SAVE_MAX_DAC;i++){
        if(dac_energy_max<energy_max_table[i]){
            dac_energy_max = energy_max_table[i];
        }
        if(dac_energy_mix>energy_mix_table[i]){
            dac_energy_mix = energy_mix_table[i];
        }
    }
    *out_max = dac_energy_max;
    *out_mix = dac_energy_mix;
    // return  dac_energy_max;
    //保存最近3秒最大能量值
    // r_printf("ms = %d",timer_get_ms());
}

void mic_energy_fun(void *priv){

    static bool keep_flag = 0;
    int energy = audio_dac_energy_get();
    // printf("energy==%d",energy);
   if(energy<0)energy= 0;
    int energy_max,energy_mix;
    //= max_energy(energy);
    int light=0;

    max_energy(energy,&energy_max,&energy_mix);

    if((energy_max-energy_mix)>0){
        light = (USER_RGB_NUM*(energy-energy_mix))/(energy_max-energy_mix);
    }
    // y_printf("------%d",(this->light_n>light)?((this->light_n)-light):(light-this->light_n));
    // log_info("light==%d",light);
    _this_->light_n = light;
    sys_timeout_add(NULL,mic_energy_fun,_this_->time);

}

/**
 * @brief 灯初始化
 * 
 */
void user_rgb_init(void)
{
#ifdef U_RGB_MODE_EN
    r_printf("%s\n",__func__);
    if(user_rgb.rgb_led_init){
        printf("----rgb init is init ok -----\n");
        return;
    }
    led_spi_init();
    mic_energy_fun(NULL);
    printf("------spi init ok \n");

    user_rgb.rgb_led_work_max  = E_RGB_MODE_MAX;///灯效最大模式
    user_rgb.rgb_led_init = 1;///标记初始化完成
    _this_ = &rgb_info;//初始化 扫描时间及开机灯效
    user_rgb.rgb_led_work_mode = _this_->Normal_mode;
	user_rgb_mode_set(_this_->Normal_mode);//上电时候的灯效，播开机提示音之前
    rgb_led_scan(NULL);
#if M_USER_RGB_DOUBLE_SPI    
    sys_timer_add(NULL,user_rgb_deal1,30);
#endif

#endif
}
extern void wdt_clear();
/**
 * @brief 扫描灯效
 * 
 */
void rgb_led_scan(void *priv)
{
    
    // printf("%s\n",__func__);
    if (!_this_)
    {
        sys_timeout_add(NULL, rgb_led_scan, 20);
        return;
    }
    user_rgb_deal();
    if (user_rgb.rgb_led_scan_flag)
    {
        sys_timer_del(user_rgb.rgb_led_scan_flag);
    }
    
    user_rgb.rgb_led_scan_flag = sys_timer_add(NULL,rgb_led_scan, _this_->time);
     
}

/**
 * @brief 获取当前灯效模式
 * 
 * @return u8 
 */
 u8 get_user_rgb_work_mode(void)
 {
     r_printf("cur rgb work mode = %d\n",user_rgb.rgb_led_work_mode);

     return (user_rgb.rgb_led_work_mode);
 }


/**
 * @brief 设置当前灯效模式
 * 
 * @param mode  模式
 */
void user_rgb_mode_set(u8 mode)
{
    printf("%s - rgb_mode = %d\n",__func__,mode);
    static u8 old_rgb_mode = 0xff;
    if(mode >= user_rgb.rgb_led_work_max){
        mode = user_rgb.rgb_led_work_max - 1;
    }
    if(!user_rgb.rgb_led_init){
        printf("rgb spi not init ok,return\n");
        return;
    }
    if(mode == old_rgb_mode){
        printf("cur rgb mode == old rgb mode,not set rgb mode\n");
        return;
    }
    user_rgb.rgb_led_work_mode = mode;
    if (user_rgb.rgb_led_work_mode==E_RGB_MODE_2)
    {
        first_on=1;
    }
    user_rgb_speed_deal();
    old_rgb_mode = mode;
    printf("set_rgb_mode = %d\n",mode);
}

//======================================灯效处理=====================================================

/**
 * @brief 将hsv换成rgb
 * 
 * @param hsv 
 */
void hsv_to_rgb(COLOUR_HSV hsv){
    COLOUR_RGB rgb;
    hsv2rgb(&hsv,&rgb);

    user_rgb.rgb_r = rgb.r;
    user_rgb.rgb_g = rgb.g;
    user_rgb.rgb_b = rgb.b;
}
/**
 * @brief 清除灯效
 * 
 */
void user_rgb_clear(){

    for (u8 i = 0; i < USER_RGB_NUM; i++)
    {
        led_spi_rgb_to_24byte(0,0,0,spi_dat_buf,i);
        
    }
#if M_USER_RGB_DOUBLE_SPI
    for (u8 i = 0; i < USER_RGB_NUM_1; i++)
    {
        led_spi_rgb_to_24byte(0,0,0,spi_dat_buf1,i); 
    }
#endif
}
/**
 * @brief 设置亮灯的颜色及起始位置
 * 
 * @param start 第一颗亮的序号
 * @param end   最后一颗亮的序号
 */
void rgb_color_by_num(u8 r,u8 g,u8 b,u8 start,u8 end){
    // printf("start==%d  end==%d",start,end);
    //向上
    // start%=USER_RGB_NUM;
    // end%=USER_RGB_NUM;
    //向下
    if (start==255)
    {
        start=USER_RGB_NUM;
    }
    if (end==255)
    {
        end=USER_RGB_NUM;
    }
    
    for (u8 i = start; i <= end; i++)
    {
        // printf("i==%d",i);
        led_spi_rgb_to_24byte(r, g, b, spi_dat_buf, i);
    }
}
static u8 mode_0_random;
void set_ramdom(u8 num){
    mode_0_random=num;
}
/**
 * unidirectional、outward、inward
 * mode
 * 0：单向 1：双向 中间朝两端  2：双向 两端朝中间
 * Step：步进值
*/
static int mode_ffect_mode(int mode,/*int *number,*/int StepUP,int StepDu,int display_max,u8 r,u8 g,u8 b){
    #define GRADIENT_V_NUMBER (4)//亮度渐变颗数
    int gradient_number=(display_max*1)/5;//参与亮度渐变的灯个数
    u16 light = _this_->light_n;
    static int display_number=0;

    //当前显示最大数量
    // int display_max ;
    if(0 == mode){
        // display_max = this->number;
    }else if((1 == mode) || (2 == mode)){
        light/=2;
        // display_max = this->number_half;
    }else{
        return -1;
    }
    // printf("light==%d",light);
    //根据能量计算亮灯的数量
    if((light-display_number)>0){
        display_number+=StepUP;
    }else if(display_number){
        display_number-=StepDu;
    }

    display_number=(display_number>display_max)?display_max:display_number;
    display_number=(display_number<0)?0:display_number;
    // printf("display_number==%d",display_number);
    //节奏
    if(0==mode){//单向
        for(int i = 0,tp;i<display_number;i++){
            tp = i;
            if (user_rgb.rgb_led_work_mode==E_RGB_MODE_2)
            {
                set_ramdom(tp);
                if (tp>=display_max)
                {
                    rgb_color_by_num(r,g,b,tp-1,tp-1); 
                }else{
                    rgb_color_by_num(r,g,b,tp,tp); 
                }
            }
            // if (user_rgb.rgb_led_work_mode==E_RGB_MODE_8)
            // {
            //     rgb_color_by_num(r,g,b,tp,tp); 
            // }
            // if (user_rgb.rgb_led_work_mode==E_RGB_MODE_5)
            // {
            //     rgb_color_by_num(r,g,b,USER_RGB_NUM-tp,USER_RGB_NUM-tp); 
            // }
   
        }
    }else if(1 == mode){//双向 中间往两端
        // log_info("display_number==%d",display_number);
        for(int i = 0,tp;i<display_number;i++){
            tp = i+USER_RGB_NUM/2;
            rgb_color_by_num(r,g,b,tp,tp);  
            tp = USER_RGB_NUM-1-tp;
            rgb_color_by_num(r,g,b,tp,tp); 

        }     
    }else if(2 == mode){//双向 两端往中间
        for(int i = 0,tp;i<display_number-1;i++){
            tp = i;
            // printf("111111111 tp==%d",tp);
            rgb_color_by_num(r,g,b,tp,tp);  
            rgb_color_by_num(r,g,b,tp+1,tp+2);  
            tp = (USER_RGB_NUM-1)-i;
            // printf("2222222222 tp==%d",tp);
            rgb_color_by_num(r,g,b,tp,tp); 
            rgb_color_by_num(r,g,b,tp-2,tp-1); 
        }
    }

    // *number = display_number;

    return display_number;
}


void user_rgb_mode_0_music(){

    _this_->time=40;
    static u16 cnt=0;
    static u8 random_cnt=0;
    static u8 pre_random=0;
    static u16 add = 0;
    u32 get_music_energy = 0;
    get_music_energy = audio_dac_energy_get();
    if (get_music_energy<50)
    {
        return;
    }
    

    //0红 240蓝 60黄 180兰 300紫 120绿
    static u16 rgb_colour_add_1[6] = {200, 300, 60, 300, 60, 200};
    static u16 rgb_colour_add_2[6] = {60, 200, 300, 60, 200, 300};
    user_rgb.colour_hsv.s = MAX_S;
    user_rgb.colour_hsv.v = MAX_V/2;

    cnt++;
    if (cnt==(10*10))
    {   
        add++;
        add%=6;
        cnt=0;
    }

    user_rgb.colour_hsv.h = rgb_colour_add_2[add];; 
    hsv_to_rgb(user_rgb.colour_hsv);
    mode_ffect_mode(0,USER_RGB_NUM/6,1,USER_RGB_NUM-1,user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b);
    if (mode_0_random>pre_random)
    {//若顶点位置更新，则重新赋值
        pre_random=mode_0_random;
    }
    if (pre_random>0)
    {
        random_cnt++;
        if ((random_cnt%10)==0)
        {
            pre_random--;
            random_cnt=0;
        }
        user_rgb.colour_hsv.h = rgb_colour_add_1[add];; 
        hsv_to_rgb(user_rgb.colour_hsv);
        rgb_color_by_num(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,pre_random,pre_random); 
    }

}

/**
 * @brief 七彩渐变
 * 
 */
void user_rgb_mode_0(void)
{
    _this_->time=30;
    static int hsv_h = 240;
    if (reback_light)
    {
        hsv_h=240;
        reback_light=0;
    }
    u32 get_music_energy = 0;
    get_music_energy = audio_dac_energy_get();

    // if((get_music_energy < 10 && user_rgb.rgb_led_work_mode==E_RGB_MODE_1)||user_rgb.rgb_led_work_mode==E_RGB_MODE_4){ 
        user_rgb.colour_hsv.s = MAX_S;
        user_rgb.colour_hsv.v = MAX_V/2;
        user_rgb.colour_hsv.h = hsv_h; 
        hsv_to_rgb(user_rgb.colour_hsv); 

        for(u8 i = 0; i < USER_RGB_NUM; i++){
            led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf,i);
        }
        
        if(hsv_h <0){
            hsv_h = 360;
        }else{
            hsv_h--;
        }
    // }else{
    //     user_rgb_mode_0_music();
    // }


}

/**
 * @brief 无音乐上下弹跳，有音乐两边往中间频闪
 * 
 */
void user_rgb_mode_1(void)
{

    u32 get_music_energy = 0;
    // get_music_energy = 0;//audio_dac_energy_get();

    
    static int down=4, up=5;
    static u8 dir=0;
    static u16 rgb_cnt_1 = 0;
    //0红 240蓝 60黄 180兰 300紫 120绿
    static u16 rgb_colour_add[3] = {60, 300, 200};
    static u16 add = 0;
    if (reback_light)
    {//关灯重开，重新计数
        down=4;
        up=5;
        dir=0;
        rgb_cnt_1 = 0;
        add = 0;
        reback_light=0;
    }
    user_rgb.colour_hsv.s = MAX_S;
    user_rgb.colour_hsv.v = MAX_V/2;
    user_rgb.colour_hsv.h = rgb_colour_add[add]; 
    hsv_to_rgb(user_rgb.colour_hsv);
    printf("get_music_energy==%d",get_music_energy);
    if(get_music_energy < 10){ 
        _this_->time=200;
        if (!dir)
        {      
        //首轮
            //中间往两边  
            if (first_on)
            {       
                //下面五个        
                if ((down-2)>=0)
                {
                    led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, down);  
                    led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, down-1);   
                    led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, down-2);  
                }
                else if (down==1)
                {
                    led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, down);  
                    led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, down-1);  
                }
                else
                {
                    led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, down);  
                }
                //上面五个
                if ((up+2)<=9)
                {
                    led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, up);  
                    led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, up+1);   
                    led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, up+2);
                }
                else if(up==8){
                    led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, up);  
                    led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, up+1);  
                }
                else{
                    led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, up);
                }  

            }else{
                //第二轮
                //下面
                if (down==5)
                {

                    led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, 4); 
                    led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, 3); 
                }
                else if (down==4)
                {

                    led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, 4); 
                    led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, 3); 
                    led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, 2);
                }
                else if ((down-2)>=0)
                {
                    led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, down);  
                    led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, down-1);   
                    led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, down-2);  
                }
                else if (down==1)
                {
                    led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, down);  
                    led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, down-1);  
                }
                else
                {
                    led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, down);  
                }


            //上面
                if (up==4)
                {
                    led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, 5); 
                    led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, 6); 
                }else if (up==5)
                {
                    led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, 5); 
                    led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, 6); 
                    led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, 7); 
                }
                else if ((up+2)<=9)
                {
                    led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, up);  
                    led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, up+1);   
                    led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, up+2);
                }
                else if(up==8){
                    led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, up);  
                    led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, up+1);  
                }
                else{
                    led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, up);
                }  

            }
            
    
            down--;
            up++;
            if (down==-1 || up==10)
            {
                dir=1;
                rgb_cnt_1++;
                down=1;
                up=8;
            }
            
        }else{
            //两边往中间
            //下面
                if (down<3)
                {
                    for (u8 i = 0; i <= down; i++)
                    {
                        led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, i); 
                    }        
                }else if(down>2 && down<5){
                    led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, down);  
                    led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, down-1);   
                    led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, down-2);  
                }else if(down==5){
                    led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, 4);
                    led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, 3);
                }else{
                    led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, 4);  
                }
            //上面
                if (up>6)
                {
                    for (u8 i = 9; i >= up; i--)
                    {
                        led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, i); 
                    } 
                }else if (up>4){
                    led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, up);  
                    led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, up+1);   
                    led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, up+2); 
                }else if(up==4){
                    led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, 5);  
                    led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, 6);  
                }else 
                {
                    led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, 5);  
                }
                
                
            
            down++;
            up--;
            if (down==7 || up==2)
            {
                first_on=0;
                dir=0;
                rgb_cnt_1++;
                down=5;
                up=4;
            }
            

        }
        // printf("dir=%d  down==%d",dir,down);
        // printf("dir=%d  up==%d",dir,up);
       
    }else 
    {
        _this_->time=50;
        mode_ffect_mode(2,USER_RGB_NUM/4,1,USER_RGB_NUM/2,user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b);
    }
    
    if (rgb_cnt_1==2)
    {
        rgb_cnt_1=0;
        add++;
        add%=3;
    }

}

//向上彩色流水
void user_rgb_mode_02(){
    _this_->time=40;
    u8 rgb_offset = 22;
    static u16 rgb_cnt = 1;
    if (reback_light)
    {
        rgb_cnt=1;
        reback_light=0;
    }
    // printf("user_rgb.rgb_r=%d user_rgb.rgb_g=%d user_rgb.rgb_b=%d",user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b);

    for(u8 i = 0; i < USER_RGB_NUM ; i++){

        user_rgb.colour_hsv.s = MAX_S;
        user_rgb.colour_hsv.v = MAX_V;
        user_rgb.colour_hsv.h = (rgb_cnt + i * rgb_offset) % 360; 
        hsv_to_rgb(user_rgb.colour_hsv); 
        led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf,((USER_RGB_NUM-1) -i));
    }

    if(rgb_cnt > 360){
        rgb_cnt = 1;
    }else{
        rgb_cnt += 5;
    }
}

// 呼吸灯效（期间不完全暗就换色）
void user_rgb_mode_03(void)
{
    user_rgb_mode_0();
}

/**
 * @brief 上往下频谱
 * 
 */
void user_rgb_mode_04(){
    

    static u16 rgb_colour_add[3] = {60, 300, 200};
    static u16 add = 0;
    static u16 rgb_cnt_1 = 0;
    static u16 random_num_1 = 7;

    if (reback_light){
        add = 0;
        rgb_cnt_1 = 0;
        random_num_1 = 7;
        reback_light=0;
    }

    user_rgb.colour_hsv.s = MAX_S;
    user_rgb.colour_hsv.v = MAX_V/2;
    user_rgb.colour_hsv.h = rgb_colour_add[add]; 
    hsv_to_rgb(user_rgb.colour_hsv);

    
    u32 get_music_energy = 0;
    get_music_energy = audio_dac_energy_get();

    if(get_music_energy < 10){ 
        _this_->time=70;
        for(u8 i = 9; i >7; i--){
            led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, i);      
        }

        for(u8 i = 8; i > random_num_1; i--){
            led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf,  i);
        }

    }else{
        _this_->time=50;
        mode_ffect_mode(0,USER_RGB_NUM/4,1,USER_RGB_NUM,user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b);
    }

    rgb_cnt_1++;
    random_num_1 --;
    if (random_num_1==3)
    {
        random_num_1=7;
    }
    if (rgb_cnt_1>(20*4))
    {
        rgb_cnt_1=0;
        add++;
        add%=3;
    }
    printf("random_num_1==%d",random_num_1);
}

/**
 * @brief 向上/向下逐个点亮至点满
 * 
 */
void user_rgb_mode_05(){
    _this_->time=70;
    static u16 rgb_cnt_1 = 0;
    static u16 cnt = 0;
    static u8 dir=0;
    static u16 add = 0;
    static u16 rgb_colour_add[3] = {60, 300, 200};

    if (reback_light){
        rgb_cnt_1 = 0;
        cnt = 0;
        dir=0;
        add = 0;
        reback_light=0;
    }

    user_rgb.colour_hsv.s = MAX_S;
    user_rgb.colour_hsv.v = MAX_V/2;
    user_rgb.colour_hsv.h = rgb_colour_add[add]; 
    hsv_to_rgb(user_rgb.colour_hsv);
    if (!dir)
    {
        led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf,  0);    
        for(u8 i = 1; i < cnt; i++){
            led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf,  i);      
        }
    }
    else{
         led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf,  9);   
        for(u8 i = 1; i < cnt; i++){
            led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, (USER_RGB_NUM-1) - i);      
        }
    }
    
    printf("random_num_1==%d",cnt);

    
    cnt++;
    if (cnt==(USER_RGB_NUM+1))
    {
        dir=!dir;
        rgb_cnt_1++;
    }
    printf("rgb_cnt_1==%d",rgb_cnt_1);
    cnt%=(USER_RGB_NUM+1);
    if (rgb_cnt_1>1)
    {
        rgb_cnt_1=0;
        add++;
        add%=3;
    }
}

/**
 * @brief 背景一个底色，中间来回跳动，有音乐时中间往两边伸展频闪
 * 
 */
void user_rgb_mode_06(){
    
    static int rgb_cnt_1 = 0;
    static u16 rgb_cnt_2 = 0;
    static u16 add1 = 0,add2=0;
    static u8 dir=0;
    u32 get_music_energy = 0;
    get_music_energy = audio_dac_energy_get();
    //0红 240蓝 60黄 180兰 300紫 120绿
    static u16 rgb_colour_add_1[3] = {60, 200, 300};
    static u16 rgb_colour_add_2[6] = {200, 300, 60, 300, 60, 200};

    user_rgb.colour_hsv.s = MAX_S;
    user_rgb.colour_hsv.v = MAX_V/2;

    if (reback_light){
        rgb_cnt_1 = 0;
        rgb_cnt_2 = 0;
        add1 = 0;
        add2 = 0;
        dir=0;
        reback_light=0;
    }
    /* 背景色*/
    for(u8 i = 0; i < USER_RGB_ROW_NUM; i++){
        user_rgb.colour_hsv.h = rgb_colour_add_1[add1];
        hsv_to_rgb(user_rgb.colour_hsv);
        led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, i);  
    }
    user_rgb.colour_hsv.h = rgb_colour_add_2[add2]; 
    hsv_to_rgb(user_rgb.colour_hsv);
    if(get_music_energy < 10){ 
        _this_->time=150;
            /* 中间色*/

            if (!dir)
            {
                rgb_cnt_1++;
                
                if (rgb_cnt_1<8)
                {//向上的
                    led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, rgb_cnt_1);  
                    led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, rgb_cnt_1+1);
                    led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, rgb_cnt_1+2);  
                }
            }else{
                rgb_cnt_1--;
                if (rgb_cnt_1>=0)
                {//向上的
                    led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, rgb_cnt_1);  
                    led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, rgb_cnt_1+1);
                    led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, rgb_cnt_1+2);  
                }

            }
            printf("rgb_cnt_1==%d",rgb_cnt_1);
            if (rgb_cnt_1>7)
            {
                if (rgb_cnt_1==8)
                {
                    add1++;
                    add1%=3;
                    add2++;
                    add2%=6;
                    led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, 9); 
                } 
                else if(rgb_cnt_1==9){
                    led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, 8); 
                    led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, 9); 
                    dir=1;
                    rgb_cnt_1=7;
                }           
                 
            }else if (rgb_cnt_1<0)
            {
                if (rgb_cnt_1==-1)
                {
                    add1++;
                    add1%=3;
                    add2++;
                    add2%=6;
                    led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, 0); 
                }else if (rgb_cnt_1==-2)
                {
                    led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, 0); 
                    led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, 1); 
                    dir=0;
                    rgb_cnt_1=0;
                }
                   
            }
    
    }else{
        /* 中间色*/
        _this_->time=50;
        mode_ffect_mode(1,USER_RGB_NUM/4,1,USER_RGB_NUM/2,user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b);
        rgb_cnt_2++;
        if (rgb_cnt_2>(20*4))
        {
            rgb_cnt_2=0;
            add1++;
            add1%=3;
            add2++;
            add2%=6;
        }
    }
    


}
/**
 * @brief 下往上频谱
 * 
 */
void user_rgb_mode_07(){
    static u16 rgb_colour_add[3] = {60, 300, 200};
    static u16 add = 0;
    static u16 rgb_cnt_1 = 0;
    static u16 random_num_1 = 2;

    if (reback_light){
        random_num_1 = 2;
        rgb_cnt_1 = 0;
        add = 0;
        reback_light=0;
    }

    user_rgb.colour_hsv.s = MAX_S;
    user_rgb.colour_hsv.v = MAX_V/2;
    user_rgb.colour_hsv.h = rgb_colour_add[add]; 
    hsv_to_rgb(user_rgb.colour_hsv);
    
    u32 get_music_energy = 0;
    get_music_energy = audio_dac_energy_get();
    printf("get_music_energy==%d",get_music_energy);
    if(get_music_energy < 10){ 
        _this_->time=70;
        for(u8 i = 0; i <2; i++){
            led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, i);      
        }

        for(u8 i = 2; i < random_num_1; i++){
            led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf,  i);
        }

    }else{
        _this_->time=50;
        mode_ffect_mode(0,USER_RGB_NUM/4,1,USER_RGB_NUM,user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b);
    }

    rgb_cnt_1++;
    random_num_1 ++;
    if (random_num_1==6)
    {
        random_num_1=2;
    }
    if (rgb_cnt_1>(20*4))
    {
        rgb_cnt_1=0;
        add++;
        add%=3;
    }
    
}

/**
 * @brief 无音乐上下弹跳，有音乐中间往两边伸展
 * 
 */
void user_rgb_mode_08(){

    static int rgb_cnt_1 = 0;
    static u16 rgb_cnt_2 = 0;
    static u16 add1 = 0,add2=0;
    static u8 dir=0;
    u32 get_music_energy = 0;
    // get_music_energy = audio_dac_energy_get();
    //0红 240蓝 60黄 180兰 300紫 120绿
    static u16 rgb_colour_add_1[3] = {60, 200, 300};
    static u16 rgb_colour_add_2[6] = {200, 300, 60, 300, 60, 200};

    if (reback_light){
        rgb_cnt_1 = 0;
        rgb_cnt_2 = 0;
        add1 = 0;
        add2 = 0;
        dir=0;
        reback_light=0;
    }

    user_rgb.colour_hsv.s = MAX_S;
    user_rgb.colour_hsv.v = MAX_V/2;
    user_rgb.colour_hsv.h = rgb_colour_add_2[add2]; 
    hsv_to_rgb(user_rgb.colour_hsv);
    if(get_music_energy < 10){ 
        _this_->time=150;
            /* 中间色*/

            if (!dir)
            {
                rgb_cnt_1++;
                
                if (rgb_cnt_1<8)
                {//向上的
                    led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, rgb_cnt_1);  
                    led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, rgb_cnt_1+1);
                    led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, rgb_cnt_1+2);  
                }
            }else{
                rgb_cnt_1--;
                if (rgb_cnt_1>=0)
                {//向上的
                    led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, rgb_cnt_1);  
                    led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, rgb_cnt_1+1);
                    led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, rgb_cnt_1+2);  
                }

            }
            printf("rgb_cnt_1==%d",rgb_cnt_1);
            if (rgb_cnt_1>7)
            {
                if (rgb_cnt_1==8)
                {
                    add1++;
                    add1%=3;
                    add2++;
                    add2%=6;
                    led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, 9); 
                } 
                else if(rgb_cnt_1==9){
                    led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, 8); 
                    led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, 9); 
                    dir=1;
                    rgb_cnt_1=7;
                }           
                 
            }else if (rgb_cnt_1<0)
            {
                if (rgb_cnt_1==-1)
                {
                    add1++;
                    add1%=3;
                    add2++;
                    add2%=6;
                    led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, 0); 
                }else if (rgb_cnt_1==-2)
                {
                    led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, 0); 
                    led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, 1); 
                    dir=0;
                    rgb_cnt_1=0;
                }
                   
            }
    
    }else{
        /* 中间色*/
        _this_->time=50;
        mode_ffect_mode(1,USER_RGB_NUM/4,1,USER_RGB_NUM/2,user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b);
        rgb_cnt_2++;
        if (rgb_cnt_2>(20*4))
        {
            rgb_cnt_2=0;
            add1++;
            add1%=3;
            add2++;
            add2%=6;
        }
    }

}


/**
 * @brief 呼吸灯（完全暗灯才换色）
 * 
 */
void user_rgb_mode_2(void)
{
    u16 rgb_bright_time = 120;
    u8 rgb_colour_kind = 6;
    static u16 rgb_bright_cnt = 0;
    static u8 hsv_v = 0;
    static u16 hsv_h = 0;
    static u8 add = 0;
    static u16 rgb_colour_add[6] = {0, 240, 60, 180, 300, 120}; //0红 240蓝 60黄 180兰 300紫 120绿

    user_rgb.colour_hsv.s = MAX_S;
    user_rgb.colour_hsv.v = hsv_v;
    user_rgb.colour_hsv.h = rgb_colour_add[add]; 
    hsv_to_rgb(user_rgb.colour_hsv); 

    for(u8 i = 0; i < USER_RGB_NUM; i++){
        led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf,i);
    }
    
    if(rgb_bright_cnt < rgb_bright_time / 2){    /* 亮度慢慢增加 */
        hsv_v += MAX_V / (rgb_bright_time / 2);
        rgb_bright_cnt++;
    }else if(rgb_bright_cnt < rgb_bright_time){  /* 亮度慢慢降低 */
        hsv_v -= MAX_V / (rgb_bright_time / 2);
        rgb_bright_cnt++;
    }else{                                  /* 换颜色 */
        if(add > 4){
            add = 0;
        }else{
            add++;
        }
        rgb_bright_cnt = 0;
    }
}



/**
 * @brief 两颗两颗上下跳
 * 
 */
void user_rgb_mode_3(void)
{
    static u16 hsv_h = 0;
    static u16 rgb_cnt = 0;
    static u16 rgb_cnt_1 = 0;
    static u16 add = 0;
    u8 rgb_colour_kind = 6;
    u8 rgb_cnt_max = 40;
    u16 rgb_colour_add[6] = {0, 240, 60, 180, 300, 120};

    user_rgb.colour_hsv.s = MAX_S;
    user_rgb.colour_hsv.v = MAX_V;
    user_rgb.colour_hsv.h = rgb_colour_add[add]; 
    hsv_to_rgb(user_rgb.colour_hsv);

    if(rgb_cnt < rgb_cnt_max){
        if(rgb_cnt / 10 == 0){
            led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf,2);
            led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf,3);
        }
        if(rgb_cnt / 10 == 1){
            led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf,1);
            led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf,4);
       
        }
        if(rgb_cnt / 10 == 2){
            led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf,0);
            led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf,5);
        }
        if(rgb_cnt / 10 == 3){
            led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf,1);
            led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf,4);
        }
        if(rgb_cnt_1 > 0){
            rgb_cnt_1 = 0;
            rgb_cnt++;
        }else{
            rgb_cnt_1++;
        }
        
    }else{
        if(add > 4){
            add = 0;
        }else{
            add++;
        }
        rgb_cnt = 0;
    }  
}

/**
 * @brief 转三圈后 逐个点亮
 * 
 */
void user_rgb_mode_4(void)
{
    static u16 hsv_h = 0;
    static u16 rgb_cnt_1 = 0;
    static u16 rgb_cnt_2 = 0;
    u16 rgb_cnt_limit = 40;
    u8 rgb_colour_kind = 6;

    user_rgb.colour_hsv.s = MAX_S;
    user_rgb.colour_hsv.v = MAX_V;
    user_rgb.colour_hsv.h = hsv_h; 
    hsv_to_rgb(user_rgb.colour_hsv);

     if(rgb_cnt_1 < 18){               /* �����Ȧ��ȥ */
        led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, rgb_cnt_1 % 6);      
    }else if(rgb_cnt_1 < 23){     /* 5 */
        for(u8 i = 0; i < 1; i++){
            led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, (5 - i));      
        }
        led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, rgb_cnt_1 % 6);      
    }else if(rgb_cnt_1 < 27){
        for(u8 i = 0; i < 2; i++){
            led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, (5 - i));      
        }
        led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, ((rgb_cnt_1 + 1) % 6));      
    }else if(rgb_cnt_1 < 30){
        for(u8 i = 0; i < 3; i++){
            led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, (5 - i));      
        }
        led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, ((rgb_cnt_1 + 2) % 6));      
    }else if(rgb_cnt_1 < 32){
        for(u8 i = 0; i < 4; i++){
            led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, (5 - i));      
        }
        led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, ((rgb_cnt_1 + 3) % 6));      
    } else if(rgb_cnt_1 < 33){
        for(u8 i = 0; i < 5; i++){
            led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, (5 - i));      
        }
        led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, ((rgb_cnt_1 + 4) % 6));      
    } 
    else if(rgb_cnt_1 < 34){
        for(u8 i = 0; i < 6; i++){
           led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, (5 - i));      
        } 
        led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, ((rgb_cnt_1 + 5) % 6));      
    }
    else{
        hsv_h += MAX_H / rgb_colour_kind;
        if(hsv_h == MAX_H){
            hsv_h = 0;
        }
        rgb_cnt_1 = 0;
    }

    if(rgb_cnt_2 > 4){
        rgb_cnt_1++;
        rgb_cnt_2 = 0;
    }else{
        rgb_cnt_2++;
    }
}


/**
 * @brief 均分两个颜色
 * 
 */
void user_rgb_mode_5(void)
{

    u16 hsv_h_1 = 65;
    u16 hsv_h_2 = 0;
    static u16 rgb_colour_add = 0;
    static u16 rgb_cnt_1 = 0;
    static u16 rgb_cnt_2 = 0;
    static u16 add = 0;
    //0红 240蓝 60黄 180兰 300紫 120绿
    static u16 rgb_colour_add_1[6*4] = {  180,60, 180, 60,
                                        60, 240, 60, 240, 
                                        180, 300, 180,300,
                                        120, 300, 120, 300,
                                        0, 120, 0, 120,
                                        330, 240, 330, 240};//{0, 240, 60, 300, 120, 180};
    static u16 rgb_colour_add_2[6*4] = {  60, 180, 60, 180,
                                        240, 60, 240, 60,
                                        300, 180,300,180,
                                        300, 120, 300 ,120,
                                        120, 0, 120, 0,
                                        240,330,240,330};
        
    if (reback_light){
        rgb_cnt_1 = 0;
        rgb_cnt_2 = 0;
        add = 0;
        reback_light=0;
    }

    user_rgb.colour_hsv.s = MAX_S;
    user_rgb.colour_hsv.v = MAX_V/2;

    for(u8 i = 0; i < 4; i++){
        user_rgb.colour_hsv.h = rgb_colour_add_1[add];; 
        hsv_to_rgb(user_rgb.colour_hsv);
        led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, i);

        user_rgb.colour_hsv.h = rgb_colour_add_2[add]; 
        hsv_to_rgb(user_rgb.colour_hsv);
        led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, 4 + i);

    }
    for(u8 i = 0; i < 3; i++){
        user_rgb.colour_hsv.h = rgb_colour_add_1[add];; 
        hsv_to_rgb(user_rgb.colour_hsv);
        led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, 8+i);
        user_rgb.colour_hsv.h = rgb_colour_add_2[add]; 
        hsv_to_rgb(user_rgb.colour_hsv);
        led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf,11 + i);
    }
    led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf,14);
    u32 get_music_energy = 0;
    get_music_energy = audio_dac_energy_get();
    if(get_music_energy < 10){ 
        _this_->time=30;
        if(rgb_cnt_1 > 20){
            if(add > (MY_SIZE(rgb_colour_add_2)-2)){
                add = 0;
            }else{
                add++;
            }
            rgb_cnt_1 = 0;
        }else{
            rgb_cnt_1++;
        }
    }
    else{
        _this_->time=15;
        if(rgb_cnt_1 > 5){
            if(add > (MY_SIZE(rgb_colour_add_2)-2)){
                add = 0;
            }else{
                add++;
            }
            rgb_cnt_1 = 0;
        }else{
            rgb_cnt_1++;
        }
    }



}

/**
 * @brief 转圈覆盖颜色
 * 
 */
#define RGB_COVER_SPEED  5 //覆盖速度

void user_rgb_mode_6(void)
{
    u8 rgb_offset = 25;
    u16 hsv_h_1 = 0;
    u16 hsv_h_2 = 60;
    static u16 rgb_cnt_1 = 0;
    static u16 rgb_cnt_2 = 0;
    // static u16 rgb_colour_add[6] = {0, 240, 60, 180, 300, 120, 0};
    static u16 rgb_colour_add[7] = {120, 0, 240, 60, 180, 300, 120};
    static u16 add = 0;

    user_rgb.colour_hsv.s = MAX_S;
    user_rgb.colour_hsv.v = 190;

    /* 原色*/
    for(u8 i = 0; i < USER_RGB_ROW_NUM; i++){
        user_rgb.colour_hsv.h = rgb_colour_add[add]; 
        hsv_to_rgb(user_rgb.colour_hsv);

        led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, ((USER_RGB_ROW_NUM-1) - i));      
    }
    /* 覆盖色 */
    for(u8 i = 0; i < rgb_cnt_2; i++){
        user_rgb.colour_hsv.h = rgb_colour_add[add + 1]; 
        hsv_to_rgb(user_rgb.colour_hsv);

        led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, ((USER_RGB_ROW_NUM-1) - i));      
    }

    if(rgb_cnt_1 > RGB_COVER_SPEED){
        if(rgb_cnt_2 > (USER_RGB_ROW_NUM-1)){
            if(add > (sizeof(rgb_colour_add) - 1)){
                add = 0;
            }else{
                add++;
            }
            rgb_cnt_2 = 0;
        }else{
            rgb_cnt_2++;
        }
        rgb_cnt_1 = 0;
    }else{
        rgb_cnt_1++;
    }
}

/**
 * @brief 纯色切换
 * 
 */
void user_rgb_mode_7(void)
{
    u16 hsv_h_1 = 0;
    u16 hsv_h_2 = 90;
    u16 hsv_h_3 = 180;
    u16 hsv_h_4 = 270;
    static u16 rgb_cnt_1 = 0;
    static u16 rgb_cnt_2 = 0;
    static u32 rgb_colour_add = 90;

    user_rgb.colour_hsv.s = MAX_S;
    user_rgb.colour_hsv.v = MAX_V;

    for(u8 i = 0; i < USER_RGB_ROW_NUM; i++){
        user_rgb.colour_hsv.h = (hsv_h_1 + rgb_colour_add) % MAX_H; 
        hsv_to_rgb(user_rgb.colour_hsv);
        led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, i);
    }

    
    if(rgb_cnt_1 > 20){

        if(rgb_cnt_2 > 4){
            rgb_colour_add = 0;
            rgb_cnt_2 = 0;
        }else{
            rgb_colour_add += 90;
            rgb_cnt_2++;
        }

        rgb_cnt_1 = 0;
    }else{
        rgb_cnt_1++;
    }
}

/**
 * @brief 两颗灯来回跳动-假火焰
 * 
 */
void user_rgb_mode_8(void)
{
    _this_->time=50;
    static u16 hsv_h = 13;
    static u16 rgb_cnt = 0;
    static u8 random_num_1 = 7;
    static u8 random_num_2 = 2;

    user_rgb.colour_hsv.s = MAX_S;
    user_rgb.colour_hsv.v = MAX_V;
    user_rgb.colour_hsv.h = hsv_h; 


    for(u8 i = 9; i >7; i--){
        // user_rgb.colour_hsv.h = hsv_h; 
        hsv_to_rgb(user_rgb.colour_hsv);
        led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, i);      
    }

    for(u8 i = 8; i > random_num_1; i--){
        printf("i==%d",i);
        // user_rgb.colour_hsv.h = hsv_h; 
        hsv_to_rgb(user_rgb.colour_hsv);
        led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf,  i);
    }



    if(rgb_cnt > 1){
        random_num_1 --;
        if (random_num_1==4)
        {
            random_num_1=7;
        }
        
        random_num_2 = random_number(2, 4);
        rgb_cnt = 0;
    }else{
        rgb_cnt++;
    }
    printf("");
    // if(hsv_h > MAX_H){
    //     hsv_h = 0;
    // }else{
    //     hsv_h++;
    // }
}

/**
 * @brief 频谱1
 * 
 */
void user_rgb_mode_9(void)
{
    static u16 hsv_h = 15;
    static u16 rgb_cnt = 0;
    static int music_energy = 3;
    static u32 music_energy_cnt = 0;
    static u32 music_energy_max = 5000;
    static u32 music_energy_max_cnt = 0;
    static u16 rgb_colour_add[6] = {0, 240, 60, 180, 300, 120};
    static u16 add = 0;
    static u16 rgb_cnt_1 = 0;
    u32 get_music_energy = 0;

    user_rgb.colour_hsv.s = MAX_S;
    user_rgb.colour_hsv.v = MAX_V;
    user_rgb.colour_hsv.h = rgb_colour_add[add]; 
    hsv_to_rgb(user_rgb.colour_hsv);



    get_music_energy = audio_dac_energy_get();
    if(get_music_energy < 500){ 
        return;
    }

    if((music_energy + 300) / 1000 > 4){
        music_energy_cnt = 4;
    }else{
        music_energy_cnt = (music_energy + 300) / 1000;
    }

 
    if(rgb_cnt < 4){    
        rgb_cnt++;
    }else if(rgb_cnt < 8){
        music_energy_cnt = 0;
        rgb_cnt++;
    }else{
        music_energy = get_music_energy;
        rgb_cnt = 0;
    }

    for(u8 i = 0; i <= music_energy_cnt; i++){
        led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, i);      
    }

    if(rgb_cnt_1 > 300){
        if(add > 4){
            add = 0;
        }else{
            add++;
        }
        rgb_cnt_1 = 0;
    }else{
        rgb_cnt_1++;
    }

    // if(hsv_h > 360){
    //     hsv_h = 0;
    // }else{
    //     hsv_h++;
    // }
}

/**
 * @brief 频谱2
 * 
 */
void user_rgb_mode_10(void)
{
    static u16 hsv_h = 13;
    static u16 rgb_cnt = 0;
    static int music_energy = 3;
    static u32 music_energy_cnt = 0;
    static u32 music_energy_max = 5000;
    static u32 music_energy_max_cnt = 0;
    u32 get_music_energy = 0;

    user_rgb.colour_hsv.s = MAX_S;
    user_rgb.colour_hsv.v = MAX_V;
    user_rgb.colour_hsv.h = hsv_h; 
    hsv_to_rgb(user_rgb.colour_hsv);



    get_music_energy = audio_dac_energy_get();
    if(get_music_energy < 500){ 
        return;
    }


    if((music_energy + 300) / 1000 > 4){
        music_energy_cnt = 4;
    }else{
        music_energy_cnt = (music_energy + 300) / 1000;
    }


    if(rgb_cnt < 4){   
        rgb_cnt++;
    }else if(rgb_cnt < 8){
        music_energy_cnt = 0;
        rgb_cnt++;
    }else{
        music_energy = get_music_energy;
        rgb_cnt = 0;
    }

    for(u8 i = 0; i <= music_energy_cnt; i++){
        led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, i);      

    }
}

// #defined Circle_up  
#define RGB_ALL_NUMBER     (12)
// #define sdf
COLOUR_HSV hsv_12_table[RGB_ALL_NUMBER];
void user_rgb_mode_12_init(void){
    static bool init_flag = 0;
    if(init_flag)return;
    const int rgb_colour_add12[12] = {240, 240, 240, 300,300, 300, 120,120,120, 350, 350,350};
    init_flag = 1;
    for(int i= 0;i<RGB_ALL_NUMBER;i++){
        hsv_12_table[i].h=rgb_colour_add12[i];//(MAX_H*i)/RGB_ALL_NUMBER;
        hsv_12_table[i].v=MAX_V;
        hsv_12_table[i].s=MAX_S;
    }
}
#define STEP_VALUE  (10)// Step value
/**
 * @brief 五色转圈
 * 
 */
void user_rgb_mode_12(void){

    user_rgb_mode_12_init();
    if(_this_->energy < 10){ 
        _this_->time=60;
    }else{
        _this_->time=30;
    }
    static u8 cnt;
    cnt++;
    if(cnt>11)
    {
        cnt=0;
    }
    for(int i= 0;i<RGB_ALL_NUMBER;i++){

        //hsv_12_table[i].h=(hsv_12_table[i].h+STEP_VALUE)%MAX_H;
        user_rgb.colour_hsv.h=hsv_12_table[(i+cnt)%12].h; 
        user_rgb.colour_hsv.s=hsv_12_table[i].s;
        user_rgb.colour_hsv.v=hsv_12_table[i].v;


        hsv_to_rgb(user_rgb.colour_hsv);
		led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf,i);
       // led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf,RGB_ALL_NUMBER-1-i);
    }

}

/**
 * @brief 红绿蓝转圈
 * 
 */
void user_rgb_mode_13(void){
    u32 sec = (timer_get_ms()/250)%RGB_ALL_NUMBER;
    // printf("user_rgb_mode_13 ");
    
    int number = 0;
    for(int i=0;i<RGB_ALL_NUMBER;i++){
        if(i<2){
            user_rgb.rgb_r=0xff;
            user_rgb.rgb_g=0;
            user_rgb.rgb_b=0;
        }else if(i<4){
            user_rgb.rgb_r=0;
            user_rgb.rgb_g=0xff;
            user_rgb.rgb_b=0;
        }else{
            user_rgb.rgb_r=0;
            user_rgb.rgb_g=0;
            user_rgb.rgb_b=0xff;
        }

        number = ((sec+i)%RGB_ALL_NUMBER);
        led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf,number);
    }

}

void user_rgb_mode_14(void){
    user_rgb.rgb_r=0;
    user_rgb.rgb_g=0;
    user_rgb.rgb_b=0;
    for(int i=0;i<USER_RGB_NUM;i++){
        led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf,i);
    }

}

extern u8 power_off_flag ;
u8 poweroff_time_id = 0;
void power_off_ui(void)
{
    printf("power_off_ui");
    static u16 hsv_h = 0;
    static u16 rgb_cnt = 0;
    static u16 rgb_cnt_1 = 0;
    static u16 rgb_cnt_2 = 0;
    u16 rgb_cnt_limit = 40;
    u8 rgb_colour_kind = 6;

    user_rgb.colour_hsv.s = MAX_S;
    user_rgb.colour_hsv.v = MAX_V;
    user_rgb.colour_hsv.h = hsv_h; 
    hsv_to_rgb(user_rgb.colour_hsv);   

    for(u8 i = 0; i < rgb_cnt_1; i++){
        led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, (5 - i));      
    }
    led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, (rgb_cnt_2));      

    printf("rgb_cnt_2==%d",rgb_cnt_2);
    if(rgb_cnt_2 > 4){
        rgb_cnt_1++;
        if(rgb_cnt_1 > 6){
            rgb_cnt_1 = 0;
            
            extern void poweroff_done();
            user_rgb.rgb_led_work_mode = E_RGB_MODE_0;
            power_off_flag = 1;
            user_rgb_clear();
            led_spi_send_rgbbuf(spi_dat_buf, USER_RGB_NUM);
            poweroff_done();
        }
        rgb_cnt = 0;
        rgb_cnt_2 = 0;
    }else{
        rgb_cnt++;
        rgb_cnt_2 = rgb_cnt / 2;
    }
   
}

void u_rgb_mid_set_hsv_to_rgbbuf(COLOUR_HSV hsv,u8 num){
    COLOUR_RGB rgb;
    hsv2rgb(&hsv,&rgb);
    // printf("rgb.r==%d  rgb.g==%d  rgb.b==%d  num==%d",rgb.r,rgb.g,rgb.b,num);

    led_spi_rgb_to_24byte(rgb.r,rgb.g,rgb.b,spi_dat_buf, num);     
}

void hsv_table_init(){

    printf("=====hsv_table_init=====");
    for(int i_zyc = 0;i_zyc < 7;i_zyc++){
        _this_->hsv_v[i_zyc].v = 255;
        _this_->hsv_v[i_zyc].h = (180*i_zyc)/6;
        // _this_->buf->hsv[i_zyc].h = 150-(150*i_zyc)/12;
        _this_->hsv_v[i_zyc].s = MAX_S;        
    }
    
}
/**
 * @brief 两边往中间流动
 * 
 */
void user_rgb_mode_new1(){
    _this_->time=30;
    static u8 test;
    int i_zyc = 0;
    if (!test)
    {
       hsv_table_init();
       test=1;
    }
    // printf("------user_rgb_mode_test0------\n");
    u8 map_table[][2]={ {0,13},{1,12},{2,11},{3,10},{4,9},{5,8},{6,7}, };
    for(i_zyc = 0;i_zyc <MY_SIZE(map_table);i_zyc++){

        _this_->hsv_v[i_zyc].h=_this_->hsv_v[i_zyc].h>0?(_this_->hsv_v[i_zyc].h-1):MAX_H;
        // printf("_this_->hsv_v[i_zyc].v==%d",_this_->hsv_v[i_zyc].v);
        // u_rgb_mid_set_hsv_to_rgbbuf(_this_->hsv_v[i_zyc],i_zyc);
        u_rgb_mid_set_hsv_to_rgbbuf(_this_->hsv_v[i_zyc],map_table[i_zyc][0]);
        u_rgb_mid_set_hsv_to_rgbbuf(_this_->hsv_v[i_zyc],map_table[i_zyc][1]);  

    }
    u_rgb_mid_set_hsv_to_rgbbuf(_this_->hsv_v[i_zyc-1],14);  
}

/**
 * @brief 顺时针全亮转圈
 * 
 */
void user_rgb_mode_new2(){
    _this_->time=40;
    u8 rgb_offset = 10;//22;
    static u16 rgb_cnt = 1;
    if (reback_light)
    {
        rgb_cnt=1;
        reback_light=0;
    }
    // printf("user_rgb.rgb_r=%d user_rgb.rgb_g=%d user_rgb.rgb_b=%d",user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b);

    for(u8 i = 0; i < USER_RGB_NUM ; i++){

        user_rgb.colour_hsv.s = MAX_S;
        user_rgb.colour_hsv.v = MAX_V;
        user_rgb.colour_hsv.h = (rgb_cnt + i * rgb_offset) % 360; 
        hsv_to_rgb(user_rgb.colour_hsv); 
        led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf,((USER_RGB_NUM-1) -i));
    }

    if(rgb_cnt > 360){
        rgb_cnt = 1;
    }else{
        rgb_cnt += 5;
    }
      
}


#define COLOUR_CHANGE_STEP  15
/**
 * @brief 顺时针转圈
 * 
 */
void user_rgb_mode_new3(){
    _this_->time=30;
    static u16 hsv_h = 0;
    static u16 rgb_cnt_1 = 5;

    static u8 circle_1,color_cnt;

    printf("get_music_energy==%d",_this_->energy);


    static u8 cicle_speed=5;
    
    if (_this_->energy>RGB_MID_ENERGY && _this_->energy<RGB_HIG_ENERGY)
    {
        cicle_speed =1;
    }else if(_this_->energy>RGB_HIG_ENERGY){
        cicle_speed =1;
    }else{
        cicle_speed =5;
    }

    user_rgb.colour_hsv.s = MAX_S;
    user_rgb.colour_hsv.v = MAX_V;
    user_rgb.colour_hsv.h = hsv_h; 
    hsv_to_rgb(user_rgb.colour_hsv);

    circle_1++;
    if (circle_1>=cicle_speed)
    {
        rgb_cnt_1++;
        if ((rgb_cnt_1 %12)==0)
        {
            rgb_cnt_1=0;
        } 
        circle_1=0;
    }

    for (u8 i = 0; i < 4; i++)
    {//亮灯数量
       led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, (rgb_cnt_1+i)%USER_RGB_NUM);  
    }
    

    if(hsv_h > 360){
        hsv_h = 0;
    }else{
        color_cnt++;
        if (color_cnt==COLOUR_CHANGE_STEP)
        {
            color_cnt=0;
            hsv_h++;
        }
       
    }
}
/**
 * @brief 下往上两个两个渐亮后渐灭(同方向)
 * 
 */
void user_rgb_mode_new4(){

    
//rgb num
    u8 circle_up[8]={0,1,2,3,4,5,6,7};
    u8 circle_down[7]={14,13,12,11,10,9,8};
    static u8 light_drak_mode=0;
    static u8 run,cnt;
//rgb color
    static u16 hsv_h;
    static u8 color_cnt;

    user_rgb.colour_hsv.s = MAX_S;
    user_rgb.colour_hsv.v = MAX_V;
    user_rgb.colour_hsv.h = hsv_h; 
    hsv_to_rgb(user_rgb.colour_hsv); 

    if(hsv_h > 360){
        hsv_h = 0;
    }else{
        // color_cnt++;
        // if (color_cnt==2)
        // {
            // color_cnt=0;
            hsv_h++;
        // }
       
    }
    //spped
    if (_this_->energy<10)
    {
        _this_->time=40;
    }
    else
    {
        _this_->time=20;
    }
    

//rgb mode
    if (!light_drak_mode)
    {
        for (u8 i = 0; i <run; i++)
        {
            led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, circle_up[i]);  
            if (i<7)
            {
            led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, circle_down[i]);
        }
        } 
    }
    else{
        for (u8 i = (MY_SIZE(circle_up)-1); i > run; i--)
        {
            led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, circle_up[i]);  
            if (i<7)
            {
            led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, circle_down[i]);
            }
        }
    }
    cnt++;
    if (cnt==4)
    {
        run++;
        run%=MY_SIZE(circle_up);  
        // printf("----run----%d\n",run); 
        if (run == 0)
        {
            light_drak_mode=!light_drak_mode;
        }   
        cnt=0;
    }
    
    
}
/**
 * @brief 七色流水顺时针转
 * 
 */
void user_rgb_mode_new5(){

    user_rgb_mode_02();

}
/**
 * @brief 上往下逐个点亮后，下往上逐个熄灭
 * 
 */
void user_rgb_mode_new6(){

    //rgb num
    u8 circle_up[8]={0,1,2,3,4,5,6,7};
    u8 circle_down[7]={14,13,12,11,10,9,8};
    static u8 light_drak_mode=0;
    static u8 run;
//rgb color
    static u16 hsv_h;
    static u8 color_cnt;

    user_rgb.colour_hsv.s = MAX_S;
    user_rgb.colour_hsv.v = MAX_V;
    user_rgb.colour_hsv.h = hsv_h; 
    hsv_to_rgb(user_rgb.colour_hsv); 

    if(hsv_h > 360){
        hsv_h = 0;
    }else{
        color_cnt++;
        if (color_cnt==5)
        {
            color_cnt=0;
            hsv_h++;
        }
       
    }
//speed
    static u8 cnt; //流动速度
    if (_this_->energy<10)
    {
        _this_->time=30;
    }else{
        _this_->time=15;
    }
   //rgb mode 
    if (!light_drak_mode)
    {
        for (u8 i = 0; i <= run; i++)
        {
            printf("i==%d\n",i);
            led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, circle_up[(MY_SIZE(circle_up)-1)-i]);  
            if ((MY_SIZE(circle_up)-1)-i<7)
            {
            led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, circle_down[(MY_SIZE(circle_up)-1)-i]);
            }
        }
        
    }
    else{
        for (u8 i = (MY_SIZE(circle_up)-1); i > run; i--)
        {
            led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, circle_up[i]);  
            if (i<MY_SIZE(circle_down))
            {
            led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, circle_down[i]);
            }
        }
    }
    cnt++;
    if (cnt==5)
    {     
        run++;
        run%=MY_SIZE(circle_up);
        if (run == 0)
        {
            light_drak_mode=!light_drak_mode;
        }  
        printf("----run----%d\n",run); 
        cnt=0;
    }
}

/**
 * @brief 由下往上两个两个灯往上跑然后逐个（两个）点亮
 * 
 */
void user_rgb_mode_new7(){
    
    static u8 cnt =0;
    static u8 status[15]={1,0,0,0,0,
                          0,0,0,0,0,
                          0,0,0,1,0};

    static int p = 0; //起始位置
    static int p1 = 13;

    static int end = 7;
    static int end1 = 8;
    static int color = 0;

    if (_this_->energy<10)
    {
        _this_->time=200;
    }else{
        _this_->time=150;
    }
   
    static u16 h[6] = {0,100,150,200,300,360};

    user_rgb.colour_hsv.s = MAX_S;
    user_rgb.colour_hsv.v = MAX_V;



    if (p<end)
    {
        status[p]=1;
        if (p-1>=0)
        {
            status[p-1]=0;
        }
       
        p++;
    }else if (p==end)
    {
        status[p]=1;
        if (p-1>=0)
        {
            status[p-1]=0;
    }
        p=0;
        end--;
        }

    if (p1>end1)
    {
        status[p1]=1;
        if (p1+1<15)
        {
            status[p1+1]=0;
        }
        p1--;
    }else if (p1==end1)
    {
        status[p1]=1;
        if (p1+1<15)
        {
            status[p1+1]=0;
        }
        p1=13;
        end1++;
    }else if(end1==14){
        status[14]=1;
        }

    
    user_rgb.colour_hsv.h = h[color]; 
    hsv_to_rgb(user_rgb.colour_hsv); 

    for (int i = 0; i<15 ; i++)
    {
        if (status[i])
        {
            led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf,i); 
    } 
        } 
   
    
    if (end<0)
    {
        color++;
        color=color%6;
        printf("h[%d]:%d",color,h[color]);
        p=0;
        end=7;

        p1=13;
        end1=8;
        for (int i = 0; i<15 ; i++)
        {
            status[i]=0;
        }
    }

    }

/**
 * @brief 无音乐上下弹跳，有音乐中间往两边伸展
 * 
 */
void user_rgb_mode_new8(){

    static u8 status[11] = {0,0,0,0,0,0,0,0,1,1,1};
    static u8 status1[11] = {0,0,0,0,0,0,0,0,1,1,1};
    static u16 h[6] = {0,100,150,200,300,360};
    static int cnt = 0;
    static int flag = 0;
    static int color = 0;
    if (_this_->energy<10)
    {
        _this_->time=200;
    }else{
        _this_->time=50;
    }
    

    if (reback_light){
        reback_light=0;
    }

    user_rgb.colour_hsv.s = MAX_S;
    user_rgb.colour_hsv.v = MAX_V;
    user_rgb.colour_hsv.h = h[color]; 
    hsv_to_rgb(user_rgb.colour_hsv);
    if (flag)
        /* 中间色*/

        {
            
        for(int i=0;i<8;i++){
            if (status[i])
            {//向上的
                led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf,7-i); 
            }

            if (status1[i]&&i<7)
            {//向上的
                led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf,14-i); 
            }

        }
    }else{
        for(int i=0;i<8;i++){
            if (status[i])
        {
                led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, i); 
            }

            if (status1[i]&&i<7)
            {
                    led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf,8+i); 
            } 

        }
            }           
                
    cnt++;
    
    if(cnt==11){
        flag=1;
    }
    else if(cnt==22){
        cnt=0;
        flag=0;
        color++;
        color=color%6;
            }
    printf("cnt:%d ,flag:%d",cnt,flag);
    u8 temp = status[10];
    for (int i = 10; i > 0; i--)
    {
        status[i]=status[i-1];
        }
    status[0]=temp;
    temp = status1[10];
    for (int i = 10; i > 0; i--)
    {
        status1[i]=status1[i-1];
        }
    status1[0]=temp;
}

/**
 * @brief 两边往中间走再逐个灭，再又中间往两边亮再逐个灭 最后换色
 *
 */
void user_rgb_mode_new16(void)
{
    static int rgb_cnt_1 = 0;
    static u8 rgb_cnt_2 = 0;
    static u16 add1 = 0,add2=0;
    static u8 dir=0 ,led_mode=0;
    //0红 240蓝 60黄 180兰 300紫 120绿
    static u16 rgb_colour_add_1[6] = {234, 350,120,60,180, 300}; //蓝-粉-绿-黄-青蓝-紫

    const u8 circle_down[6]={11,10,9,8,7,6};
    const u8 circle_up[6]={6,7,8,9,10,11};

    const u8 circle_up1[6]={0,1,2,3,4,5};
    const u8 circle_down1[6]={5,4,3,2,1,0};
    const u8 circle_down2[7]={6,5,4,3,2,1,0};
    if(add2==0)
    {
        user_rgb.colour_hsv.s = MAX_S;
        user_rgb.colour_hsv.v = MAX_V;       
    }
    else if(add2==1)
    {

        user_rgb.colour_hsv.s = MAX_S*0.5;
        user_rgb.colour_hsv.v = MAX_V;

    }
    else
    {
        user_rgb.colour_hsv.s = MAX_S;
        user_rgb.colour_hsv.v = MAX_V/2;
    }
    user_rgb.colour_hsv.h = rgb_colour_add_1[add2];
    hsv_to_rgb(user_rgb.colour_hsv);
    if (_this_->energy<10)
    {
        _this_->time=200;
    }else{
        _this_->time=50;
    }
    
    
    rgb_cnt_1++;
    if(rgb_cnt_1%29==0)
    {
        dir=1;
    }
    else
    {
        dir=0;
    }
    if(!dir)
    {
        rgb_cnt_2++;
        if(rgb_cnt_2<7)
        {
            for(int i=0;i<rgb_cnt_2;i++)
            {
                led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, i);
                led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, circle_down[i]);
            }
        }
        else if(7<=rgb_cnt_2&&rgb_cnt_2<13)
        {
            for(int i=rgb_cnt_2-7;i<7;i++)
            {
                led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, circle_down[i]);
                led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, circle_down1[6-i]);
            }
            
             
        }
        else if(15<=rgb_cnt_2&&rgb_cnt_2<21)
        {
            for(int j=6;j<rgb_cnt_2-8;j++)
            {
                led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, j);
                led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, circle_down1[j-6]);
            }

        }
        else if (21<=rgb_cnt_2&&rgb_cnt_2<27)
        {
            for(int j=12;j>rgb_cnt_2-15;j--)
            {
                led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, j);
                led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, circle_down1[j-6]);
            }

        }
    }
    else
    {
        printf("==add2=%d",add2);
        rgb_cnt_1=0;
        rgb_cnt_2=0;
        add2++;
        if(add2>5)
        {
            add2=0;
        }
    }
}
void user_rgb_mode_new9(void){

    static u8 cnt = 0;
    static u8 color =0;
    static u8 status[15] = {0};
    static u16 h[6] = {234, 350,120,60,180, 300};
    static u8  s[6] = {250,127,250,250,250,250};
    static u8  v[6] = {250,250,127,64,32,16};
    if (_this_->energy<10)
    {
        _this_->time=200;
    }else{
        _this_->time=50;
    }
    user_rgb.colour_hsv.s = s[color];
    user_rgb.colour_hsv.v = v[color];
    user_rgb.colour_hsv.h = h[color]; 
    hsv_to_rgb(user_rgb.colour_hsv);
    switch (cnt)
    {
        case 0:
            status[0] = 1;
            status[4] = 1;
            status[8] = 1;
            status[13] = 1;
            status[10]=1;
            status[11]=1;
            break;
        case 1:
            status[1] = 1;
            status[7] = 1;
            status[3] = 1;
            status[5] = 1;
            status[9]=1;
            status[14]=1;
            status[12]=1;
            break;
        case 2:
            status[2] = 1;
            status[6] = 1;
            status[9] = 0;
            status[13] = 0;
            status[10]=0;
            status[11]=0;
            break;
        case 3:
            status[2] = 0;
            status[6] = 0;
            status[10]=0;
            break;
        case 4:
            status[1] = 0;
            status[7] = 0;
            status[3] = 0;
            status[5] = 0;
            status[14]=0;
            status[12]=0;
            break;
        default:
            break;
    }
    for(int i=0;i<15;i++){
        if(status[i]){
            led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, i);
        }
    }
    cnt++;
    if (cnt>=5)
    {
        color++;
        color%=6;
        cnt=0;
    }
}

/**
 * @brief 没音乐的时候三个常亮往上闪两颗，有音乐时候逐渐往上补齐
 *
 */

void user_rgb_mode_new10(void)
{
// data init
    static u16 rgb_colour_add[6] = {240, 350,120,60,180, 300}; //蓝-粉-绿-黄-青蓝-紫
    static u8 add = 0;
    static u8 rgb_cnt_1 = 0;
    static u8 rgb_cnt_2 = 0;
    static u8 random_num_1 = 7;
    static u8 led_mode = 0;
    static u8 flash = 0;
    u8 led_flag=0;
//reback
    if (reback_light){
        random_num_1 = 7;
        rgb_cnt_1 = 0;
        add = 0;
        reback_light=0;
    }
// color
    user_rgb.colour_hsv.s = MAX_S;
    user_rgb.colour_hsv.v = MAX_V/2;
    user_rgb.colour_hsv.h = rgb_colour_add[add];
    hsv_to_rgb(user_rgb.colour_hsv);

//mode
    if(_this_->energy < 10){
        //no music
        _this_->time=500;
        for(u8 i = 0; i<5; i++){
            led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, i);
        }

        for(u8 i = 6; i < random_num_1; i++){
            led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf,  i);
            led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, 14);
        }
    }
    else{
        // have music
        rgb_cnt_2++;
        _this_->time=100;
        led_flag=1;
        for(u8 i=0;i<5;i++)
        {
            led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, i);
        }
        led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, 13);
        led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, 12);
        led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, 11);

        if(flash==1){
            led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, 5);
            led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, 7);

            for(u8 i=8;i<15;i++){
                if (i==9)
            {
                }else{
                    led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, i);
                }
            }
        }else if(flash==2){
            led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, 5);
            led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, 7);
            led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, 6);
            for(u8 i=8;i<15;i++){
                led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, i);
            }
        }
        if(rgb_cnt_2<9)
        {
        }
        else
        {
            rgb_cnt_2=0;
            add++;
            add%=6;
        }
        flash++;
        flash=flash%3;
    }

    rgb_cnt_1++;
    random_num_1 ++;

    if (random_num_1==8)
    {
        random_num_1=6;
        led_mode++;
    }
    if(led_mode>1)
    {
        led_mode=0;
        if(led_flag==0)
        {
            add++;
            add%=6;
        }
    }

    printf("====add: %d======\n",add);
    printf("====led_flag: %d======\n",led_flag);
    if (rgb_cnt_1>(20*4))
    {
        rgb_cnt_1=0;
    }


}

/**
 * @brief 全亮渐变色 ，有音乐闪烁
 * 
 */
void user_rgb_mode_new11(){
    static u8 index_number[3]={0,4,8};
    static u8 index=0 ,add=0 ,cnt=0,cnt2=0;
    // color
    static u16 rgb_colour_add[6] = {240, 350,120,60,180, 300}; //蓝-粉-绿-黄-青蓝-紫
    user_rgb.colour_hsv.s = MAX_S;
    user_rgb.colour_hsv.v = MAX_V/2;
    user_rgb.colour_hsv.h = rgb_colour_add[add];
    hsv_to_rgb(user_rgb.colour_hsv);
    static u8 status[8] = {0};
    _this_->time=40;
    //mode
    // if(_this_->energy < 10){
       
        user_rgb_mode_0();
    // }else{

        // for (int i=(cnt%4)*2; i <(cnt%i)*2+2; i++)
            // printf("index_number[index]==%d",index_number[index]);
 

        //     led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf, i);
        // }    

            cnt++;

                // printf("=========== i==%d ============",i);
            }


            

/**
 * @brief 呼吸灯
 * 
 */
void user_rgb_mode_new12(){
    user_rgb_mode_2();
}

/**
 * @brief 均分两色
 * 
 */
void user_rgb_mode_new13(){
    user_rgb_mode_5();
}

void user_rgb_mode_new14(){
    user_rgb_mode_12();
}

#if M_USER_RGB_DOUBLE_SPI
void user_rgb_breath_mode(){

    u16 rgb_bright_time = 120;
    static u16 rgb_bright_cnt = 0;
    static u8 hsv_v = 0;
    static u8 add = 0;
    static u16 rgb_colour_add[6] = {0, 240, 60, 180, 300, 120}; //0红 240蓝 60黄 180兰 300紫 120绿

    user_rgb.colour_hsv.s = MAX_S;
    user_rgb.colour_hsv.v = hsv_v;
    user_rgb.colour_hsv.h = rgb_colour_add[add]; 
    hsv_to_rgb(user_rgb.colour_hsv); 

    for(u8 i = 0; i < USER_RGB_NUM_1; i++){
        led_spi_rgb_to_24byte(user_rgb.rgb_r,user_rgb.rgb_g,user_rgb.rgb_b,spi_dat_buf1,i);
    }
    
    if(rgb_bright_cnt < rgb_bright_time / 2){    /* 亮度慢慢增加 */
        hsv_v += MAX_V / (rgb_bright_time / 2);
        rgb_bright_cnt++;
    }else if(rgb_bright_cnt < rgb_bright_time){  /* 亮度慢慢降低 */
        hsv_v -= MAX_V / (rgb_bright_time / 2);
        rgb_bright_cnt++;
    }else{                                  /* 换颜色 */
        if(add > 4){
            add = 0;
        }else{
            add++;
        }
        rgb_bright_cnt = 0;
    }
}
#endif
/**
 * @brief 低电红灯闪烁
 * 
 */
void user_rgb_mode_lowpower(){
    static u16 cnt=0;
    cnt++;
    if (cnt<10)
    {
        user_rgb.colour_hsv.h=0;
        user_rgb.colour_hsv.s=MAX_S;
        user_rgb.colour_hsv.v=MAX_V;
        for (u8 i = 0; i < USER_RGB_NUM; i++)
        {
            u_rgb_mid_set_hsv_to_rgbbuf(user_rgb.colour_hsv, i);   
        }

    }else{
        user_rgb_clear();
    }
    
    cnt%=20;
    led_spi_send_rgbbuf(spi_dat_buf, USER_RGB_NUM);
}
//======================================灯效处理end=====================================================
void user_rgb_speed_deal(void){
    if(!user_rgb.rgb_led_init){
        // r_printf("rgb spi not init ok,return\n");
        return;
    }
    switch(user_rgb.rgb_led_work_mode)
    {
        case E_RGB_MODE_0:
            _this_->time=1000;
            break;
        case E_RGB_MODE_1:
             _this_->time=100;
            break;
        case E_RGB_MODE_2:
            _this_->time=30;
            break;
        case E_RGB_MODE_3:
            _this_->time=30;
            break;
        case E_RGB_MODE_4:
            _this_->time=30;
            break;  
        case E_RGB_MODE_5:
            _this_->time=1000;
            break;
        case E_RGB_MODE_6:
             _this_->time=30;
            break;
        case E_RGB_MODE_7:
            _this_->time=30;
            break;
        case E_RGB_MODE_8:
            _this_->time=30;
            break;
        case E_RGB_MODE_9:
            _this_->time=30;
            break;                       
    }

}


#if M_USER_RGB_DOUBLE_SPI
void user_rgb_deal1(){

    wdt_clear();
    if(!user_rgb.rgb_led_init){
        // r_printf("rgb spi not init ok,return\n");
        return;
    }
    user_rgb_clear();
    if (!power_off_flag)
    {
       user_rgb_breath_mode();
    }
    

    led_spi_send_rgbbuf1(spi_dat_buf1, USER_RGB_NUM_1);
}
#endif 

void user_rgb_deal(void)
{
    wdt_clear();
    if(!user_rgb.rgb_led_init){
        // r_printf("rgb spi not init ok,return\n");
        return;
    }
#if !M_USER_RGB_FM_EN
    if(user_rgb.fm_flag){ 
        r_printf("open fm\n");
        return;
    }
#endif
#if USE_SUOAI_RGB_MODE
    extern u8 low_bat_change_rgb;
    if (low_bat_change_rgb)
    {
        user_rgb_mode_lowpower();
        return;
    }
#endif

    if (light_off)
    {
        
        _this_->time=1000;
        user_rgb_clear();
        if (light_off==1)
        {//关灯只发一次数据
            printf("rgb all off!!!\n");
            led_spi_send_rgbbuf(spi_dat_buf, USER_RGB_NUM);
        }
        light_off=2;
        return;
    } 
    _this_->energy=audio_dac_energy_get()*10;
    user_rgb_clear();
    
    // printf("user_rgb.rgb_led_work_mode==%d",user_rgb.rgb_led_work_mode);

    switch(user_rgb.rgb_led_work_mode)
    {
        case E_RGB_MODE_0://关灯
            user_rgb_clear();
            break;
        case E_RGB_MODE_1: //
            // user_rgb_mode_new1();
            user_rgb_mode_new2();
            break;
        case E_RGB_MODE_2://七彩频谱
            // user_rgb_mode_new2();
            user_rgb_mode_new1();
            break;
        case E_RGB_MODE_3://跑马
            user_rgb_mode_new3();
            break;
        case E_RGB_MODE_4://七彩渐变
            user_rgb_mode_new4();
            break;
        case E_RGB_MODE_5:
            user_rgb_mode_new5();
            break;
        case E_RGB_MODE_6:
            user_rgb_mode_new6();
            break;
        case E_RGB_MODE_7:
            user_rgb_mode_new7();
            break;
        case E_RGB_MODE_8:
            user_rgb_mode_new8();
            break;
        case E_RGB_MODE_9:
            user_rgb_mode_new9();
            break;
        case E_RGB_MODE_10:
            user_rgb_mode_new10();
            break;
        case E_RGB_MODE_11:
            user_rgb_mode_new11();
            break;
        case E_RGB_MODE_12:
            user_rgb_mode_new12();
            break;
        case E_RGB_MODE_13:
            user_rgb_mode_new13();
            break;
        case E_RGB_MODE_14:
            user_rgb_mode_new14();
            break;
        // case E_RGB_MODE_POWEROFF:
        //     power_off_ui();
            // break;
        default:
            break;
    }

    led_spi_send_rgbbuf(spi_dat_buf, USER_RGB_NUM);


    
}