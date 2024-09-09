#include "user_head.h"
#include "debug/user_debug.h"

#define LOG_TAG_CONST        U_COMM
#define LOG_TAG             "[U_COMM]"
#define LOG_ERROR_ENABLE
#define LOG_DEBUG_ENABLE
#define LOG_INFO_ENABLE
#define LOG_DUMP_ENABLE
#define LOG_CLI_ENABLE
/**
 * @brief pr口设置为输入
 * 
 * @param gpio 
 * @param dir 
 * @return int 
 */
int u_rtc_port_pr_in(u32 gpio, u32 dir){
    #if ((defined CONFIG_CPU_BR25)||(defined CONFIG_CPU_BR23))
    if(!dir){
        return rtc_port_pr_in(gpio);
    }
    #endif

    #ifdef CONFIG_CPU_BR28
    return 0;
    #endif
}


int u_rtc_port_pr_out(u32 gpio, u32 dir){
    log_debug("pr%d out=%d",gpio,dir);
    #if ((defined CONFIG_CPU_BR25)||(defined CONFIG_CPU_BR23))
    return rtc_port_pr_out(gpio,dir);
    #endif

    #ifdef CONFIG_CPU_BR28
    return 0;
    #endif
}

int u_rtc_port_pr_pu(u32 gpio, u32 dir){
    log_debug("pr%d pu=%d",gpio,dir);
    #if ((defined CONFIG_CPU_BR25)||(defined CONFIG_CPU_BR23))
    return rtc_port_pr_pu(gpio,dir);
    #endif

    #ifdef CONFIG_CPU_BR28
    return 0;
    #endif
}

int u_rtc_port_pr_pd(u32 gpio, u32 dir){
    log_debug("pr%d pd=%d",gpio,dir);
    #if ((defined CONFIG_CPU_BR25)||(defined CONFIG_CPU_BR23))
    return rtc_port_pr_pd(gpio,dir);
    #endif

    #ifdef CONFIG_CPU_BR28
    return 0;
    #endif
}
int u_rtc_port_pr_die(u32 gpio, u32 dir){
    log_debug("pr%d die=%d",gpio,dir);
    #if ((defined CONFIG_CPU_BR25)||(defined CONFIG_CPU_BR23))
    return rtc_port_pr_die(gpio,dir);
    #endif

    #ifdef CONFIG_CPU_BR28
    return 0;
    #endif
}
/**
 * @brief 设置gpio状态
 *
 * @param gpio io口
 * @param ctl 需要设置的状态
 * @param delay_cnt 设置io状态之后延时时间 在设置为输入立马读取ad值时需要用到
 */
// AT(.amr_code)
void u_comm_gpio_set(u32 gpio,GPIO_CTL ctl,int delay_cnt){

    if(NO_CONFIG_PORT==gpio)return;

    int (*set_direction) (u32 gpio, u32 dir)=NULL;
    int (*set_pull_up) (u32 gpio, u32 dir)=NULL;
    int (*set_pull_down) (u32 gpio, u32 dir)=NULL;
    int (*set_die) (u32 gpio, u32 dir)=NULL;
    int (*direction_output) (u32 gpio, u32 dir)=NULL;
    

    if(gpio>=IO_PORT_PR_00 && gpio<=IO_PORT_PR_04){
        log_debug("rtc");
        gpio-=IO_PORT_PR_00;
        set_direction=u_rtc_port_pr_in;
        set_pull_up=u_rtc_port_pr_pu;
        set_pull_down=u_rtc_port_pr_pd;
        set_die=u_rtc_port_pr_die;
        direction_output=u_rtc_port_pr_out;
    }else{
        set_direction=gpio_set_direction;
        set_pull_up=gpio_set_pull_up;
        set_pull_down=gpio_set_pull_down;
        set_die=gpio_set_die;
        direction_output=gpio_direction_output;
    }

    // log_debug("ctl=%d",ctl);
    switch(ctl){
    case USER_GPIO_HI:
    case USER_GPIO_IN_IO:
        set_direction(gpio, 1);        set_pull_down(gpio, 0);
        set_pull_up(gpio, 0);          set_die(gpio, 1);
        break;
    case USER_GPIO_IN_AD:
        set_direction(gpio, 1);        set_pull_down(gpio, 0);
        set_pull_up(gpio, 0);          set_die(gpio, 0);
        break;
    case USER_GPIO_OUT_H:
        set_direction(gpio, 0);        set_pull_down(gpio, 0);
        set_pull_up(gpio, 0);          set_die(gpio, 1);
        direction_output(gpio, 1);
        break;
    case USER_GPIO_OUT_L:
        set_direction(gpio, 0);        set_pull_down(gpio, 0);
        set_pull_up(gpio, 0);          set_die(gpio, 1);
        direction_output(gpio, 0);
        break;
    case USER_GPIO_IN_IO_PU:
        set_direction(gpio, 1);        set_pull_down(gpio, 0);
        set_pull_up(gpio, 1);          set_die(gpio, 1);
        break;
    case USER_GPIO_IN_IO_PD:
        set_direction(gpio, 1);        set_pull_down(gpio, 1);
        set_pull_up(gpio, 0);          set_die(gpio, 1);
        break;
    case USER_GPIO_IN_AD_PU:
        set_direction(gpio, 1);        set_pull_down(gpio, 0);
        set_pull_up(gpio, 1);          set_die(gpio, 0);
        break;
    case USER_GPIO_IN_AD_PD:
        set_direction(gpio, 1);        set_pull_down(gpio, 1);
        set_pull_up(gpio, 0);          set_die(gpio, 0);
        break;
    default:
        break;
    }

    if(delay_cnt){
        delay(delay_cnt);
    }
}
//================================ K歌音箱 按键消息处理 =========================================
#ifdef U_EFFECT_MODE_EN
u32 mic_2_online_timer =0;
u32 mic_1_online_timer =0;
u8  mic_1_offline = 0;
u8  mic_2_offline = 0;
/**
 * @brief 
 * 
 * @param event 
 * @return int 
 */
int u_sys_key_event_handler(struct sys_event *event){
    int ret = true;
    struct key_event *key = &event->u.key;
    int key_event = event->u.key.event;
    int key_value = event->u.key.value;

    switch (key_event) {
    // case KEY_VOL_UP:
    //     log_info("KEY_VOL_UP\n");
    //     if (app_audio_get_volume(APP_AUDIO_STATE_MUSIC) == app_audio_get_max_volume()) {
    //         if (tone_get_status() == 0) {
    //             app_task_put_key_msg(KEY_PLAY_TONE_MIC_VOL_MAX, 0);
    //         }
    //     }
    //     ret = false;
    //     break;
    // case KEY_VOL_DOWN:
    //     log_info("KEY_VOL_DOWN\n");
    //     if (!app_audio_get_volume(APP_AUDIO_STATE_MUSIC)) {
    //         if (tone_get_status() == 0) {
    //             app_task_put_key_msg(KEY_PLAY_TONE_MIC_VOL_MIN, 0);
    //         }
    //     }
    //     ret = false;
    //     break;

    case KEY_PLAY_TONE_MIC_ADJUST://播放"话筒调节"提示音
        log_debug("KEY_PLAY_TONE_MIC_ADJUST");
        tone_play_by_path(USER_FILE_NAME_MicAdjust, 1);
        break;
    case KEY_PLAY_TONE_ACCOMPANIMENT_ADJUST://播放"伴奏调节"提示音
        log_debug("KEY_PLAY_TONE_ACCOMPANIMENT_ADJUST");
        tone_play_by_path(USER_FILE_NAME_AccomAd, 1);
        break;
    case KEY_PLAY_TONE_REVERB_ADJUST://播放"混响调节"提示音
        log_debug("KEY_PLAY_TONE_REVERB_ADJUST");
        tone_play_by_path(USER_FILE_NAME_ReverbAd, 1);
        break;
    case KEY_PLAY_TONE_MIC_VOL_MAX://播放最大音量提示音
        log_debug("KEY_PLAY_TONE_MIC_VOL_MAX");
        if (tone_get_status() == 0) {
            // if(tone_play_by_path(TONE_RES_ROOT_PATH"tone/max.*", 1)){
            //     tone_play_by_path(TONE_NORMAL, 0);
            // }
            tone_play_by_path(tone_table[IDEX_TONE_MAX_VOL], 1);
        }   
        break;
    case KEY_PLAY_TONE_MIC_VOL_MIN://播放最小音量提示音
        log_debug("KEY_PLAY_TONE_MIC_VOL_MIN");
        if (tone_get_status() == 0) {
            // if(u_check_file(TONE_RES_ROOT_PATH"tone/min.*")
            // if(tone_play_by_path(TONE_RES_ROOT_PATH"tone/min.*", 1)){
            //     tone_play_by_path(TONE_NORMAL, 0);
            // }
            tone_play_by_path(tone_table[IDEX_TONE_MAX_VOL], 1);
        }   
        break;
    case KEY_ACCOMPANIMENT_UP://伴奏加
        log_debug("KEY_ACCOMPANIMENT_UP");
        u_effect_accompaniment(U_CMD_UP);
        break;
    case KEY_ACCOMPANIMENT_DOWN://伴奏减
        log_debug("KEY_ACCOMPANIMENT_DOWN");
        u_effect_accompaniment(U_CMD_DOW);
        break;
    case KEY_REVERB_UP://混响加
        log_debug("KEY_REVERB_UP");
        u_effect_reverb(U_CMD_UP);
        break;
    case KEY_REVERB_DOWN://混响减:
        log_debug("KEY_REVERB_DOWN");
        u_effect_reverb(U_CMD_DOW);
        break;


    case USER_LIGHT_RGB_SWITCH://RGB模式切换
        log_debug("USER_LIGHT_RGB_SWITCH");
        // if(USER_RGB_MODE_ON_OFF==get_rgb_mode(MODE_SPE)){
        //     set_rgb_mode_index(0);
		// 	break;
        // }
        // u_rgb_mode_next();
        break;
    case USER_LIGHT_RGB_OFF_ON://RGB开关
        log_debug("USER_LIGHT_RGB_OFF_ON");

        // if(USER_RGB_MODE_ON_OFF!=get_rgb_mode(MODE_KEEP)){
        //     rgb_special_keep(USER_RGB_MODE_ON_OFF);
        // }else{
        //     // rgb_special_diskeep();
        //     set_rgb_mode_index(0);
        // }
        break;
    case USER_LIGHT_RGB_SET://RGB模式设置
        log_debug("USER_LIGHT_RGB_SET");
        set_rgb_mode_auto(key_value);
        break;
    case USER_LIGHT_RGB_KEEP://RGB 维持模式
        log_debug("USER_LIGHT_RGB_KEEP");
        rgb_special_keep(key_value);
        break;


    case USER_LIGHT_LED_SWITCH://LED模式切换
        log_info("USER_LIGHT_SWITCH \n");
        u_led_switch();
        break;
    case USER_LIGHT_LED_ON://LED on
        log_info("USER_LIGHT_LED_ON \n");
        // u_led_ctrl(U_LED_ON);
        break;

    case USER_LIGHT_LED_OFF://LED off
        log_info("USER_LIGHT_LED_OFF \n");
        // u_led_ctrl(U_LED_OFF);
        break;


    case USER_KARAOKE_CHANGER_NONE://原声
        log_info("USER_KARAOKE_CHANGER_NONE \n");
        u_effect_mode_none();
        break;
    case USER_AUDIO_TONE_CTRL://魔音
        log_info("USER_AUDIO_TONE_CTRL \n");
        u_effect_loop_mode();
        break;
    case USER_KARAOKE_EARPHONE_ECHO://指定回声
        log_info("USER_KARAOKE_EARPHONE_ECHO \n");
        u_mic_voicechange_switch(VOICE_CHANGE_ECHO);
        break;
    case USER_KARAOKE_EARPHONE_REVERB://指定混响
        log_info("USER_KARAOKE_EARPHONE_REVERB \n");
        u_mic_voicechange_switch(VOICE_CHANGE_REVERB);
        break;
    case USER_MIC_ONLINE_OR_OFFLINE://mic 上下线消息处理
        log_info("USER_MIC_ONLINE_OR_OFFLINE \n");
        u_eff_mic_effect_start_or_stop();
        log_debug("-%x-",key_value);
        u_tws_status_sync_one(TWS_SYNC_MIC_ONLINE);
        // if(key_value)tone_play_by_path((u8 *)key_value, 1);
        break;
    case KEY_PLAY_TONE_MIC1_CONNECT:
        log_info("KEY_PLAY_TONE_MIC1_CONNECT \n");
        if (tone_get_status())
        {
            extern void mic1_online_mode_timer();
            mic_1_online_timer= sys_timeout_add(NULL,mic1_online_mode_timer,500);
            break;
        }
        mic_1_offline = 0;
        tone_play_by_path(USER_FILE_NAME_Mic1_Connect,1);
        break;
    case KEY_PLAY_TONE_MIC1_DSCONNECT:
        log_info("KEY_PLAY_TONE_MIC1_DSCONNECT \n");
        if (mic_1_online_timer)
        {
            sys_timeout_del(mic_1_online_timer);
            mic_1_online_timer=0;
        }
        mic_1_offline = 1;
        tone_play_by_path(USER_FILE_NAME_Mic1_Disconnect,1);
        break;
    case KEY_PLAY_TONE_MIC2_CONNECT:
        log_info("KEY_PLAY_TONE_MIC2_CONNECT \n");
        if (tone_get_status())
        {
            extern void mic2_online_mode_timer();
            mic_2_online_timer= sys_timeout_add(NULL,mic2_online_mode_timer,500);
            break;
        }
        mic_2_offline = 0;
        tone_play_by_path(USER_FILE_NAME_Mic2_Connect,1);
        break;
    case KEY_PLAY_TONE_MIC2_DSCONNECT:
        log_info("KEY_PLAY_TONE_MIC2_DSCONNECT \n");
        if (mic_2_online_timer)
        {
            sys_timeout_del(mic_2_online_timer);
            mic_2_online_timer=0;
        }
        mic_2_offline = 1;
        tone_play_by_path(USER_FILE_NAME_Mic2_Disconnect,1);
        break;

    case USER_AUDIO_VOCAL_REMOVE_SWITCH://消原音
        log_info("USER_AUDIO_VOCAL_REMOVE_SWITCH \n");
    #if AUDIO_VOCAL_REMOVE_EN
        r_printf("---remove_switch-%d",user_vocal_remove_get());
        if(user_vocal_remove_get()){
            user_vocal_remove_off();
        }else{
            user_vocal_remove_on();            
        }
        u_tws_status_sync_one(TWS_SYNC_VOCAL);
        tone_play_by_path(user_vocal_remove_get()?USER_FILE_NAME_XRSK:USER_FILE_NAME_XRSG, 1);
    #endif
        break;
    //对箱接收到处理
    case USER_AUDIO_VOCAL_REMOVE_ON://消原音 开
        log_info("USER_AUDIO_VOCAL_REMOVE_ON \n");
        user_vocal_remove_on();
        break;
    case USER_AUDIO_VOCAL_REMOVE_OFF://消原音 关
        log_info("USER_AUDIO_VOCAL_REMOVE_OFF \n");
        user_vocal_remove_off();
        break;

    case KEY_PA_MUTE_UMUTE_SWITCH://功放umute 、mute切换看是否有噗噗声
        log_info("KEY_PA_MUTE_UMUTE_SWITCH \n");
        u_pa_mute_umute_switch();
        break;
    case KEY_MIC_VOL_UP://话筒加(预留，暂时没用到)
        log_info("KEY_MIC_VOL_UP \n");
        u_effect_mic_vol(U_CMD_UP);
        break;
    case KEY_MIC_VOL_DOWN://话筒减(预留，暂时没用到)
        log_info("KEY_MIC_VOL_DOWN \n");
        u_effect_mic_vol(U_CMD_DOW);
        break;

    case KEY_MIC_VOL_MUTE://话筒MUTE
        log_info("KEY_MIC_VOL_MUTE \n");
        u_effect_mic_mute_get()?u_effect_mic_umute():u_effect_mic_mute();
        
        tone_play_by_path(u_effect_mic_mute_get()?USER_FILE_NAME_MicClose:USER_FILE_NAME_MicOpen, 1);
        break;
    case KEY_MIC_VOL_ON:
        log_info("KEY_MIC_VOL_ON \n");
        u_effect_mic_mute();
        break;
    case KEY_MIC_VOL_OFF:
        log_info("KEY_MIC_VOL_OFF \n");
        u_effect_mic_umute();
        break;

    case USER_TWS_MIC_STATUS:
        log_info("USER_TWS_MIC_STATUS \n");
        if(u_tws_mic_status_get() || u_effect_mic_onlin_get()){
            log_info("eq set EQ_MODE_NORMAL \n");
            #if (defined(TCFG_EFFECT_TOOL_ENABLE) && !TCFG_EFFECT_TOOL_ENABLE)
            if( EQ_MODE_NORMAL !=eq_mode_get_cur()){
                eq_mode_set(EQ_MODE_NORMAL);
            }
            #endif
        }else if( EQ_MODE_CUSTOM !=eq_mode_get_cur()){
            log_info("eq set EQ_MODE_CUSTOM \n");
            #if (defined(TCFG_EFFECT_TOOL_ENABLE) && !TCFG_EFFECT_TOOL_ENABLE)
            eq_mode_set(EQ_MODE_CUSTOM);
            #endif
        }
        break;
    case USER_EQ_SET:
        log_info("USER_EQ_SET\n");
        eq_mode_set(key_value);
        break;
    default:
        ret=false;
        break;
    }
    return ret;
}
#endif 
///======================= 外挂电量指示灯 四颗灯 每个大概是百分之二十五 ==========================
#if BAT_LED_SHOW
volatile u8 led_status = 0;
#define FILT_TIMES  50
// #define FILT_THRESHOLD 2
u8 tmp[FILT_TIMES] = {0};
u8 tmp_b[FILT_TIMES/5] = {0};


struct timer_hdl {
    int index;
    int prd;
};

static struct timer_hdl hdl;


#define __this  (&hdl)


static const u32 timer_div[] = {
    /*0000*/    1,
    /*0001*/    4,
    /*0010*/    16,
    /*0011*/    64,
    /*0100*/    2,
    /*0101*/    8,
    /*0110*/    32,
    /*0111*/    128,
    /*1000*/    256,
    /*1001*/    4 * 256,
    /*1010*/    16 * 256,
    /*1011*/    64 * 256,
    /*1100*/    2 * 256,
    /*1101*/    8 * 256,
    /*1110*/    32 * 256,
    /*1111*/    128 * 256,
};

#define APP_TIMER_CLK           clk_get("timer")
#define MAX_TIME_CNT            0x7fff
#define MIN_TIME_CNT            0x100

#define TIMER_CON               JL_TIMER5->CON
#define TIMER_CNT               JL_TIMER5->CNT
#define TIMER_PRD               JL_TIMER5->PRD
#define TIMER_VETOR             IRQ_TIME5_IDX

#define TIMER_UNIT_US           2 //1US��һ���ж�
#define MAX_TIMER_PERIOD_MS     (1000/TIMER_UNIT_US)

/*-----------------------------------------------------------*/

static volatile u32 delay_cnt = 0;

void user_delay_nms(int cnt)
{
    delay_cnt = cnt;

    while (delay_cnt);
}

void __attribute__((weak)) timer_1ms_handler()
{
    bat_led(led_status);
}

___interrupt
static void timer0_isr()
{
  // irq_handler_enter(TIME1_INT);

    TIMER_CON |= BIT(14);//��� PND �ж������־
    timer_1ms_handler();//���50us�����Ҳ���֤��ʱ������

//    irq_handler_exit(TIME1_INT);
}

int timer0_init() 
{
    u32 prd_cnt;
    u8 index;

    /* printf("%s :%d", __func__, __LINE__); */

    for (index = 0; index < (sizeof(timer_div) / sizeof(timer_div[0])); index++) {
        prd_cnt = TIMER_UNIT_US * (APP_TIMER_CLK / 1000) / timer_div[index];//����1000Ϊ1ms,��1000000Ϊ1us
        if (prd_cnt > MIN_TIME_CNT && prd_cnt < MAX_TIME_CNT) {
            break;
        }
    }
    __this->index   = index;
    __this->prd     = prd_cnt;

    TIMER_CNT = 0;
    TIMER_PRD = prd_cnt; //1ms
    request_irq(TIMER_VETOR, 1, timer0_isr, 0);
    /* TIMER_CON = (index << 4) | BIT(0) | BIT(3); */
    TIMER_CON = (index << 4) | BIT(0);

    /* printf("PRD : 0x%x / %d", TIMER_PRD, clk_get("timer")); */

    return 0;
}

static void clock_critical_enter(void)
{
    TIMER_CON = 0 ;
}

static void clock_critical_exit(void)
{
    u32 prd_cnt;
    u8 index;

    /* printf("%s :%d", __func__, __LINE__); */

    for (index = 0; index < (sizeof(timer_div) / sizeof(timer_div[0])); index++) {
        prd_cnt = TIMER_UNIT_US * (APP_TIMER_CLK / 1000) / timer_div[index];
        if (prd_cnt > MIN_TIME_CNT && prd_cnt < MAX_TIME_CNT) {
            break;
        }
    }
    __this->index   = index;
    __this->prd     = prd_cnt;

    TIMER_CNT = 0;
    TIMER_PRD = prd_cnt; //1ms
    /* TIMER_CON = (index << 4) | BIT(0) | BIT(3); */
    TIMER_CON = (index << 4) | BIT(0);
}
CLOCK_CRITICAL_HANDLE_REG(timer0, clock_critical_enter, clock_critical_exit)



/*----------------------------------------------------------------------------*/
/**@brief   LED�ص�
   @param   led �ص��ĸ��ƣ�0 ~ 3��
   @return  void
   @author  
   @note    void bat_led_clear_all_without(u8 led)
*/
/*----------------------------------------------------------------------------*/
void bat_led_clear_all_without(u8 led)
{
    (led < 2 ) ? (BAT_LED_CLAER(5)) : (BAT_LED_CLAER(4)); 
}


/*----------------------------------------------------------------------------*/
/**@brief   LED��ʾ
   @param   led �����ĸ����ƣ�0 ~ 3��
   @return  void
   @author  
   @note    void bat_led_on(u8 led)
*/
/*----------------------------------------------------------------------------*/
void bat_led_on(u8 led)
{
    // printf("led==%d",led);
    switch (led)
    {
    case 0:
        // BAT_LED1_ON;
        BAT_LED0_ON;
        break;
    case 1:
        // BAT_LED0_ON;
        BAT_LED1_ON;
        break;
    case 2:
        BAT_LED2_ON;
        break;
    case 3:
        BAT_LED3_ON;
        break;
    }
}
#if 0
/*----------------------------------------------------------------------------*/
/**@brief   �õ�ѹ���־ֵ��ʾ
   @param   bat_val ��ѹ���ֵ
            power_shutdown_val �ػ���ѹ
            old_level ��ǰLED��ʾ��������
   @return  u8
            ����LED��ʾ��������
   @author  
   @note    u8 get_bat_led_level(u16 bat_val ,u16 power_shutdown_val ,u8 old_level)
*/
/*----------------------------------------------------------------------------*/
u8 get_bat_led_level(u16 bat_val ,u16 power_shutdown_val ,u8 old_level)
{ 
    static u8 i = 0;
    static u8 old_level_tmp = 0;
    u8 cur_level_tmp;
    u16 std_diff_vol = 420 - power_shutdown_val;
    u16 cur_diff_vol = bat_val - power_shutdown_val;///(420-LOW_POWER_SHUTDOWN)

    if(bat_val < power_shutdown_val)
    {
        old_level = 0;
        return old_level;
    }
    else if(bat_val > 420){
        cur_level_tmp |= 0x0f;
        //return old_level;
    }   
    else{
        if(cur_diff_vol >= std_diff_vol*3/4)
        {
            cur_level_tmp = 3;
        }else if((cur_diff_vol >= std_diff_vol*2/4) && (cur_diff_vol < std_diff_vol*3/4))
        {
            cur_level_tmp = 2;
        }
        else if((cur_diff_vol >= std_diff_vol*1/4) && (cur_diff_vol < std_diff_vol*2/4))
        {
            cur_level_tmp = 1;
        }else{
            cur_level_tmp = 0;
        }
    }
    
    if(old_level_tmp == cur_level_tmp)//����10��
    {
        i++;
        if(i > 10)
        {
            
            old_level = 0;
            for(i = 0; i <= cur_level_tmp;i++)
            {
                old_level |= BIT(i);
            }
            i = 0;
            return old_level;
        }
    }else{
        old_level_tmp = cur_level_tmp;
        i = 0;
    }
    
    return old_level;

}

static void find(u8 *a, int  l, u8 *max, u8 *value)
{
    if(l == 1) 
    {
        *max = 1;
        *value = a[0];
        return;
    }
    find(a,l-1, max, value);
    if(a[l-1] == *value) *max++;
    else
    {
        int i, cnt = 1;
        for(i = 0;i<l-1; i ++)
            if(a[i] == a[l-1])cnt++;
        if(*max < cnt)
        {
            *max = cnt;
            *value = a[l-1];
        }
    }
}

static void find_max(u8 *a ,int l,u8 *max)
{
    *max = 0;
    for(int i = 0;i < l;i++)
    {
        if(*max < a[i])
        {
            *max = a[i];
        }
    }
}

/*----------------------------------------------------------------------------*/
/**@brief   �õ����ȼ���ʾLED����
   @param   cur_battery_level �����ȼ���0 ~ 9��
            old_level ��ǰLED��ʾ��������
   @return  u8
            ����LED��ʾ��������
   @author  
   @note    u8 get_bat_led_level(u16 bat_val ,u16 power_shutdown_val ,u8 old_level)
*/
/*----------------------------------------------------------------------------*/

u8 get_bat_led_level_by_bl(u8 cur_battery_level , u8 old_level)
{
    static u8 battery_level = 0;
    static l = 0;
    static u8 k = 0;
    u8 cur_level_tmp = 0;
    u8 max;
    u8 old_battery_level = 0;
    u8 i = 0;
    static u8 j = 0;
    if(l == 0)//�ϵ�ʱֱ����ʾ����
    {
        old_battery_level = cur_battery_level;
        battery_level = old_battery_level;
        l = 1;
        r_printf("screen1");
        r_printf("battery_level1 = %d",battery_level);
        goto screen;
    }
    // r_printf("cur bat value = %d  ,old bat value = %d\n",cur_battery_level,old_battery_level);
    tmp[j] = cur_battery_level;
    if(j == FILT_TIMES -1)
    {
        // find(tmp, FILT_TIMES, &max, &old_battery_level);// ��50�����ҵ���ͬ��������ֵ
        find_max(tmp ,FILT_TIMES,&old_battery_level);//��50�����ҵ�����ֵ
        j = 0;
      
        if(k < FILT_TIMES/5)
        {
            tmp_b[k] = old_battery_level;
            k++;
            return old_level;
        }else{
            //find(tmp_b, FILT_TIMES/5, &max, &old_battery_level);
            max = 0;
            for( i = 0;i < FILT_TIMES/5;i++)
            {
                max += tmp_b[i];
                
            }
            old_battery_level = max/(FILT_TIMES/5);//��10�����ֵ�м���ƽ��ֵ
            printf("x = %d y = %d\n",max/(FILT_TIMES/5),max%(FILT_TIMES/5));
            k = 0;
            if(battery_level != old_battery_level && max%(FILT_TIMES/5) == 0){
                battery_level = old_battery_level;
                r_printf("battery_level2 = %d",battery_level);
            }else{
                return old_level;
            }
        }
        printf("old_battery_level = %d\n",old_battery_level);
        
        
screen:
        r_printf("screen2");
        if(old_battery_level == 0){
        cur_level_tmp = 0;
        i = 0;
        return 0;
        }else if(old_battery_level >= 1 && old_battery_level <= 2)
        {
            cur_level_tmp = 1;//��ʾ1����
        }else if(old_battery_level > 2  && old_battery_level <=  4)
        {
            cur_level_tmp = 2;//��ʾ2����
        }else if(old_battery_level > 4 && old_battery_level < 6)
        {
            cur_level_tmp = 3;//��ʾ3����
        }else if(old_battery_level >= 6 && old_battery_level < 10)
        {
            cur_level_tmp = 4;//��ʾ4����
        }
        old_level = 0;
        for(i = 0; i < cur_level_tmp;i++)
        {
            old_level |= BIT(i);
        }
        i = 0;
        return old_level;
    } 
    j++;
    return old_level;
}
#endif
/**
 * @brief 根据电量值返回对应需要显示的灯的数量
 * 
 * @param bat 
 * @return u8 
 */
u8 get_bat_level_by_vbat(u16 bat){
    printf("[%s] bat ==%d",__func__,bat);
    if (bat>400)
    {//4颗
        return 15;//1111
    }
    else if (bat>370)
    {//3颗
        return 7 ;//0111
    }
    else if (bat>350)
    {//2颗
        return 3;//0011
    }
    else
    {//1颗
        return 1;//0001
    }
    
}

u8 get_bat_level_by_level(u8 level){
    printf("[%s] level ==%d",__func__,level);
    if (level>8)//80%
    {//4颗
        return 15;//1111
    }
    else if (level>5)//50%
    {//3颗
        return 7 ;//0111
    }
    else if (level>3)//30%
    {//2颗
        return 3;//0011
    }
    else
    {//1颗
        return 1;//0001
    }
    
}
/*----------------------------------------------------------------------------*/
/**@brief   LED��ʾ
   @param   led_status ��ʾ������
   @return  void
   @author  
   @note    void bat_led(u8 led_status)
*/
/*----------------------------------------------------------------------------*/
void bat_led(u8 led_status)
{
    static u8 i = 0;

    // printf("led_status==%d",led_status);
    
    if(!led_status)
    {
        BAT_LED_CLAER(4);
        BAT_LED_CLAER(5);
    }
    if (led_status == 1)
    {
        static u16 cnt = 0;
        cnt++;
        if (cnt>=500)
        {
            cnt%=1000;
            BAT_LED_CLAER(4);
            BAT_LED_CLAER(5);
        }else{
            bat_led_on(0);
        }
        
        return ;
    }
    

    for ( i; i < 4; i++)
    {
        if (led_status & BIT(i))
        {
            bat_led_clear_all_without(i);
            bat_led_on(i);
            
            break;
        }
    }
    i++;

    if(i > 3)
    i = 0;
}



#endif//BAT_LED_SHOW



/**
 * @brief 无操作自动关机
 * 
 */
void bt_check_power_off_deal(void)
{
    
#if USER_NO_ACTION_AUTO_OFF

    static u8 mute_cnt=0;
    if ((get_total_connect_dev() == 0) && (app_get_curr_task() == APP_BT_TASK)) 	
	{    
        if(app_var.auto_shut_down_timer == 0)
		{
            sys_auto_shut_down_enable();
        }
    }
	else
	{
        if (info_pa.sys)
        {
            //无音乐
            mute_cnt=0;
        }else{
            //有音乐
            if(app_var.auto_shut_down_timer)
            {
                mute_cnt++;
            }
        }

        if (mute_cnt>50)
        {//关闭自动关机
            sys_auto_shut_down_disable();   
        }
           
    }
#endif
}