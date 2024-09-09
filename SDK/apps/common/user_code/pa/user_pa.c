#include "user_head.h"

#define LOG_TAG_CONST        U_PA
#define LOG_TAG             "[U_PA]"
#define LOG_ERROR_ENABLE
#define log_info_ENABLE
#define LOG_INFO_ENABLE
#define LOG_DUMP_ENABLE
#define LOG_CLI_ENABLE
#include "debug/user_debug.h"




PA_INFO info_pa={//功放模块
    .en = false,
#ifdef CONFIG_CPU_BR28
    .stream_run = true,
#endif
    .pa_index = LTK_5325,
    .io.mute.pro=IO_PORTA_00,
    .io.mute.mult=NO_CONFIG_PORT,
    .io.abd.pro=NO_CONFIG_PORT,
    .io.abd.mult=NO_CONFIG_PORT,
};



/**
 * @brief 功放io初始化
 * 
 */
void u_pa_io_init(void){
    #ifdef U_PA_MODE_EN

    info_pa.en=true;
    //功放初始化
    u_pa_control(PA_INIT);

    log_info("pa io init ok");

    #endif
}
/**
 * @brief 功放开关 && AB/D类电平档位初始化
 * 
 */
void u_pa_port_init(){


    switch (info_pa.pa_index)
    {
        case LTK_5325:
        //LTK_5325类型功放 适用只有一个MUTE控制脚，没有AB类控制脚,但是要切AB类的时候的电平需要控制在1.2-1.6v(输入上拉)，D类>2.3v
            info_pa.umute_to_abd_en=true;  //umute 是否复用mute脚去控制AB/D类
            info_pa.gpio_s_mute=USER_GPIO_OUT_L;   //mute功放io口状态
            info_pa.gpio_s_ab=USER_GPIO_IN_IO_PU;    //ab功放io口状态
            info_pa.gpio_s_d=USER_GPIO_OUT_H;  //d功放io口状态
            info_pa.gpio_s_off=USER_GPIO_OUT_L;    //off功放io口状态
            if(info_pa.umute_to_abd_en)info_pa.io.abd=info_pa.io.mute;
            break;
        case LTK_5302:
            //LTK_5302    有AB/D独立控制脚位并且串在mute脚 切AB类的时候的电平需要控制在1.2-1.6v(输入上拉)，D类>2.3v
            info_pa.umute_to_abd_en=false;  //umute 是否复用mute脚去控制AB/D类
            info_pa.gpio_s_mute=USER_GPIO_OUT_L;   //mute功放io口状态
            info_pa.gpio_s_umute =USER_GPIO_OUT_H; //unmute功放io口状态 
            info_pa.gpio_s_ab=USER_GPIO_OUT_L;    //ab功放io口状态  一端接MUTE脚，设AB类则输出低与MUTE的高电平 分压到vddio/2
            info_pa.gpio_s_d=USER_GPIO_OUT_H;  //d功放io口状态  一端接MUTE脚,设置为输入或者输出高都可以
            info_pa.gpio_s_off=USER_GPIO_OUT_L;    //off功放io口状态
            if(info_pa.umute_to_abd_en)info_pa.io.abd=info_pa.io.mute;
            break;
        case CS_8509:
            //CS_8679E    有AB/D独立控制脚位 切AB类的时候的低电平，D类>2.3v
            info_pa.umute_to_abd_en=false;  //umute 是否复用mute脚去控制AB/D类
            info_pa.gpio_s_mute=USER_GPIO_OUT_L;   //mute功放io口状态
            info_pa.gpio_s_umute =USER_GPIO_OUT_H; //unmute功放io口状态 
            info_pa.gpio_s_ab=USER_GPIO_OUT_L;    //ab功放io口状态
            info_pa.gpio_s_d=USER_GPIO_OUT_H;  //d功放io口状态
            info_pa.gpio_s_off=USER_GPIO_OUT_L;    //off功放io口状态
            if(info_pa.umute_to_abd_en)info_pa.io.abd=info_pa.io.mute;
            break;
        case CS_8679E:
        //CS_8679E    有AB/D独立控制脚位 切AB类的时候的电平需要控制在1.2-1.6v(输入上拉)，D类>2.3v
            info_pa.umute_to_abd_en=false;  //umute 是否复用mute脚去控制AB/D类
            info_pa.gpio_s_mute=USER_GPIO_OUT_L;   //mute功放io口状态
            info_pa.gpio_s_umute =USER_GPIO_OUT_H; //unmute功放io口状态 
            info_pa.gpio_s_ab=USER_GPIO_IN_IO_PU;    //ab功放io口状态
            info_pa.gpio_s_d=USER_GPIO_OUT_H;  //d功放io口状态
            info_pa.gpio_s_off=USER_GPIO_OUT_L;    //off功放io口状态
            if(info_pa.umute_to_abd_en)info_pa.io.abd=info_pa.io.mute;
            break;
        case HT_326:
        //HT_326类型功放 适用只有开关功放，不需要切AB/D类
            info_pa.umute_to_abd_en=false;//umute 是否复用mute脚去控制AB/D类
            info_pa.gpio_s_mute=USER_GPIO_OUT_L;//mute功放io口状态
            info_pa.gpio_s_umute=USER_GPIO_OUT_H;//mute功放io口状态
            info_pa.gpio_s_ab=USER_GPIO_HI;//ab功放io口状态
            info_pa.gpio_s_d=USER_GPIO_HI;//d功放io口状态
            info_pa.gpio_s_off=USER_GPIO_IN_AD_PD;//off功放io口状态
            if(info_pa.umute_to_abd_en)info_pa.io.abd=info_pa.io.mute;
            break;
    }


}

/**
 * @brief auto mute 需要的外部mute信息
 * 
 * @param obj 设置项
 * @param status 项目状态
 */
void u_pa_ex_info_set(EX_FLAG obj,bool status){
    #ifdef U_PA_MODE_EN

    if(!info_pa.en)return;


    switch (obj){
    case PA_MIC://mic 上线
        info_pa.mic=status;
        break;
    case PA_MANUAL://手动 mute key
        info_pa.manual=status;
        break;
    case PA_LINEIN://linein pp puse
        info_pa.linein=status;
        break;
    case PA_SYS_MUTE://系统自动mute
        info_pa.sys=status;
        break;
    
    default:
        break;
    }

    #endif
}

/**
 * @brief 获取 模式 mute状态
 *
 * @return true mute
 * @return false umute
 */
static bool get_task_mute_status(void){
    static volatile bool run=0;
    static volatile bool Historical_state=0;

    if(run){
        return Historical_state;
    }
    run = true;

    bool mute_status=info_pa.sys;//mute_staust_sys_get();

    //各个模式特有mute
    if (app_check_curr_task(APP_BT_TASK)){
        #if TCFG_APP_BT_EN
        mute_status = !bt_audio_is_running();
        #endif
    }else if (app_check_curr_task(APP_MUSIC_TASK)){
        #if TCFG_APP_MUSIC_EN
        mute_status = (file_dec_get_status() == FILE_DEC_STATUS_PLAY)?0:1;
        #endif
    }else if(app_check_curr_task(APP_FM_TASK)){
        mute_status=false;
    }else if(app_check_curr_task(APP_LINEIN_TASK)){
        #if TCFG_APP_LINEIN_EN
        mute_status = !info_pa.linein;
        #endif
    }else{// power on power off idle
        mute_status = 1;
    }

    run=0;
    Historical_state = mute_status;
    return Historical_state;
}

static void u_pa_auto_mute(void){

    if(!info_pa.en)return;
    
    bool auto_mute=true;

    //自动mute
    auto_mute = get_task_mute_status();

    /*******************************mute*******************************/
    //mic 插入
#if TCFG_MIC_EFFECT_ENABLE 
    if(info_pa.mic || mic_effect_get_status()){
        auto_mute = 0;
    }
#endif
    //音量为0
    if(!app_var.music_volume){
        auto_mute = 1;
    }
    //提示音
    if(tone_get_status()){//tone_get_status 状态更新 比tone_get_dec_status 状态更新慢
        if(tone_get_dec_status()){//容易造成概率性死机
            auto_mute = 0;
        }
    }
#if USE_BT_UNMUTE_ALLTIME    
    if (app_check_curr_task(APP_BT_TASK))
    {
        auto_mute = 0;
    }
#endif
    u_pa_control(auto_mute?PA_MUTE:PA_UMUTE);

}


/**
 * @brief 功放自动mute初始化
 * 
 */
void u_pa_automute_init(void){
    #ifdef U_PA_MODE_EN

    if(!info_pa.en)return;

    info_pa.id_auto= my_time_add(NULL,u_pa_auto_mute,M_PA_AUTO_MUTE_DET_MAX_CNT);

    #endif
}


/**
 * @brief 功放控制总入口
 * PA_UPDATE
 */
void u_pa_control(PA_STATUS ctrl){
    #ifdef U_PA_MODE_EN

    if(!info_pa.en)return;
    // log_info("ctrl==%d info_pa.status_abd==%d  info_pa.status_mute==%d",ctrl,info_pa.status_abd,info_pa.status_mute);
    switch (ctrl){
        case PA_CLASS_AB:
            if(ctrl!=info_pa.status_abd){
                log_info("PA_CLASS_AB"); 
                info_pa.status_abd=ctrl;
                u_comm_gpio_set(info_pa.io.abd.pro,info_pa.gpio_s_ab,0);
            }
            break;

        case PA_CLASS_D:
            if(ctrl!=info_pa.status_abd){
                log_info("PA_CLASS_D"); 
                info_pa.status_abd=ctrl;
                u_comm_gpio_set(info_pa.io.abd.pro,info_pa.gpio_s_d,0);
            }
            break;

        case PA_UMUTE:
            if(ctrl!=info_pa.status_mute){
                log_info("PA_UMUTE");
                info_pa.status_mute=ctrl;

                if(APP_FM_TASK == app_get_curr_task())
                {
                    u_pa_control(PA_CLASS_AB);
                }
                else
                {
                    u_pa_control(PA_CLASS_D);
                }
                if (!info_pa.umute_to_abd_en)
                {//abd类脚位不复合mute脚，则需要在设置一下脚位状态
                    u_comm_gpio_set(info_pa.io.mute.pro,info_pa.gpio_s_umute,0);
                }

            }
            break;

        case PA_MUTE:
            if(ctrl!=info_pa.status_mute){
                log_info("PA_MUTE");
                info_pa.status_mute=ctrl;
                info_pa.status_abd=ctrl;
                if (!info_pa.umute_to_abd_en)
                {//有单独AB类控制脚，mute的时候最好设置ab类控制脚为高阻态，部分功放不设置状态会有噗噗声
                    u_comm_gpio_set(info_pa.io.abd.pro,USER_GPIO_HI,0);
                }
                u_comm_gpio_set(info_pa.io.mute.pro,info_pa.gpio_s_mute,0);
            }
            break;

        case PA_UPDATE:
            if(ctrl!=info_pa.status_mute){
                log_info("PA_UPDATE");
                info_pa.status_mute=ctrl;
                u_pa_control(PA_UMUTE);     
            }        
            break;
        case PA_INIT:
            log_info("PA_INIT");
            u_pa_port_init();
            u_pa_control(PA_MUTE); 
            break;

        case PA_POWER_OFF_IDLE:
        case PA_POWER_OFF_SOFT:
            log_info("[PA_POWER_OFF_SOFT]");
            {//有单独AB类控制脚，mute的时候最好设置ab类控制脚为高阻态，部分功放不设置状态会有噗噗声
                u_comm_gpio_set(info_pa.io.abd.pro,USER_GPIO_HI,0);
            }
            u_comm_gpio_set(info_pa.io.mute.pro,info_pa.gpio_s_off,0);
            break;

    }
    // bt_check_power_off_deal();
    #endif
}
/**
 * @brief 关闭功放自动mute
 * 
 */
void u_pa_automute_off(void){
    #ifdef U_PA_MODE_EN

    if(!info_pa.en)return;
    log_info("[%s]",__func__);
    if(info_pa.id_auto){
        my_time_del(info_pa.id_auto);
        info_pa.id_auto=0;
    }

    u_pa_control(PA_POWER_OFF_SOFT);

    info_pa.en=false;

    #endif
}

/**
 * @brief 处理dac上电噗噗声
 * 
 */
void u_comm_dac_pupu(void){
    #ifdef U_PA_MODE_EN

    #if ((defined CONFIG_CPU_BR25)||(defined CONFIG_CPU_BR23))
    log_info(">>>>>u_comm_dac_pupu>>>>>>");
	extern struct audio_dac_hdl dac_hdl;
	extern int audio_dac_start(struct audio_dac_hdl *dac);
	extern int audio_dac_stop(struct audio_dac_hdl *dac);
	audio_dac_start(&dac_hdl);
	audio_dac_stop(&dac_hdl);
    log_info(">>>>>u_comm_dac_pupu  end >>>>>>");
    #endif

    #ifdef CONFIG_CPU_BR28    
    sound_platform_power_on("dac");
    os_time_dly(5);
    #endif
    
    os_time_dly(20);
    // delay_2ms(25);

    #endif
}

