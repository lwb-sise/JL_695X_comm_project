#include "media/includes.h"
#include "system/includes.h"
#include "app_config.h"
#include "app_task.h"
#include "app_action.h"
#include "tone_player.h"
#include "app_main.h"
#include "ui_manage.h"
#include "vm.h"
#include "key_event_deal.h"
#include "asm/pwm_led.h"
#include "user_cfg.h"
#include "ui/ui_api.h"
#include "clock_cfg.h"
#include "includes.h"
#include "fm/fm_api.h"
#include "fm/fm_manage.h"
#include "fm/fm_rw.h"
#include "user_head.h"

#define LOG_TAG_CONST       APP_FM
#define LOG_TAG             "[APP_FM]"
#define LOG_ERROR_ENABLE
#define LOG_DEBUG_ENABLE
#define LOG_INFO_ENABLE
/* #define LOG_DUMP_ENABLE */
#define LOG_CLI_ENABLE
#include "debug.h"

/*************************************************************
   此文件函数主要是fm模式按键处理和事件处理

	void app_fm_task()
   fm模式主函数

	static int fm_event_handler(struct sys_event *event)
   fm系统事件所有处理入口

	static void fm_app_unint(void)
	fm模式退出

**************************************************************/



#if TCFG_APP_FM_EN

#if USE_FM_VOL_FADE_IN
/**
 * @brief FM音量渐进
 * 
 */
static u8 sys_target=0;
static u16 fm_digital_Fade_in_id=0;
static u8 exit_fm=0;


void fm_digital_Fade_in_end(void){
    if(fm_digital_Fade_in_id){
        sys_timer_del(fm_digital_Fade_in_id);
        fm_digital_Fade_in_id=0;
    }
    int music_volume;
    syscfg_read(CFG_MUSIC_VOL, &music_volume, 1);
    printf("[FM END]+++++++++++++++++++++{%d %d } exit_fm==%d",music_volume,app_audio_get_volume(APP_AUDIO_CURRENT_STATE),exit_fm);
    if (exit_fm)
    {
        if (music_volume>=0)
        {
            app_audio_set_volume(APP_AUDIO_STATE_MUSIC, music_volume, 1);
        }else{
            app_audio_set_volume(APP_AUDIO_STATE_MUSIC, SYS_DEFAULT_VOL, 1);
        }
    }else{
        app_audio_set_volume(APP_AUDIO_STATE_MUSIC, sys_target, 1);
    }
    sys_target=0;
}

void fm_digital_Fade_in(void *priv){
    u8 vol=app_audio_get_volume(APP_AUDIO_STATE_MUSIC);
    if(sys_target>vol){
        app_audio_set_volume(APP_AUDIO_STATE_MUSIC, vol+1, 1);
    }else{
        fm_digital_Fade_in_end();
    }
    vol=app_audio_get_volume(APP_AUDIO_STATE_MUSIC);
    printf("------fm %d",vol);
}

void fm_digital_Fade_in_start(void){
    if(fm_digital_Fade_in_id){
        fm_digital_Fade_in_end();
    }
    fm_digital_Fade_in_id=sys_timer_add(NULL,fm_digital_Fade_in,350);
}
#endif

static u8 fm_idle_flag = 1;
//*----------------------------------------------------------------------------*/
/**@brief    fm按键消息入口
   @param    无
   @return   1、消息已经处理，不需要发送到common  0、消息发送到common处理
   @note
*/
/*----------------------------------------------------------------------------*/
static int fm_key_event_opr(struct sys_event *event)
{
    int ret = true;
    struct key_event *key = &event->u.key;

    int key_event = event->u.key.event;
    int key_value = event->u.key.value;//

    r_printf("key value:%d, event:%d \n", key->value, key->event);

#if (TCFG_SPI_LCD_ENABLE)
    extern int key_is_ui_takeover();
    if (key_is_ui_takeover()) {
        return false;
    }
#endif

    switch (key_event) {
    case KEY_TEST_DEMO_0:
        log_info("KEY_TEST_DEMO_0 = %d \n", key_value);
        app_task_put_key_msg(KEY_TEST_DEMO_1, 5678); //test demo
        break;
    case KEY_TEST_DEMO_1:
        log_info("KEY_TEST_DEMO_1 = %d \n", key_value); //test demo
        break;

    case KEY_MUSIC_PP://暂停播放
        /* app_task_put_key_msg(KEY_TEST_DEMO_0,1234);  //test demo// */
        fm_volume_pp();
        break;
    case  KEY_FM_SCAN_ALL://全自动搜台
    case  KEY_FM_SCAN_ALL_DOWN://全自动搜台
    case  KEY_FM_SCAN_ALL_UP://全自动搜台
        fm_scan_all();
        break;
    case  KEY_FM_SCAN_DOWN:
        fm_scan_down();//半自动搜台
        break;
    case  KEY_FM_SCAN_UP:
        fm_scan_up();//半自动搜台
        break;
    case  KEY_FM_PREV_STATION://下一台
        fm_prev_station();
        break;
    case  KEY_FM_NEXT_STATION:
        fm_next_station();
        break;
    case  KEY_FM_PREV_FREQ://下一个频率
        fm_prev_freq();
        break;
    case  KEY_FM_NEXT_FREQ:
        fm_next_freq();
        break;
    case KEY_VOL_UP:
        fm_volume_up();
        break;
    case KEY_VOL_DOWN:
        fm_volume_down();
        break;

    default:
        ret = false;
        break;
    }
#if (SMART_BOX_EN)
    extern void smartbot_fm_msg_deal(int msg);
    smartbot_fm_msg_deal(key_event);
#endif

    return ret;
}



//*----------------------------------------------------------------------------*/
/**@brief    fm 模式活跃状态 所有消息入口
   @param    无
   @return   1、当前消息已经处理，不需要发送comomon 0、当前消息不是fm处理的，发送到common统一处理
   @note
*/
/*----------------------------------------------------------------------------*/

static int fm_event_handler(struct sys_event *event)
{
    int err = 0;
    switch (event->type) {
    case SYS_KEY_EVENT:
        return fm_key_event_opr(event);
        break;

    case SYS_DEVICE_EVENT:
        if ((u32)event->arg == DEVICE_EVENT_FROM_FM) {
            if (event->u.dev.event == DEVICE_EVENT_IN) {
                log_info("fm online \n");
            } else if (event->u.dev.event == DEVICE_EVENT_OUT) {
                log_info("fm offline \n");
#if USE_FM_VOL_FADE_IN
                exit_fm=1;
                fm_digital_Fade_in_end();
#endif                
                app_task_switch_next();
            }
            return true;
        }
        return false;

    default:
        return false;
    }
}

static void fm_app_init(void)
{
    fm_idle_flag = 0;
    sys_key_event_enable();
    ui_update_status(STATUS_FM_MODE);
    clock_idle(FM_IDLE_CLOCK);
    fm_manage_init();//
    fm_api_init();//设置频率信息
}


static void fm_app_start(void)
{
    fm_manage_start();//收音出声
}

static void fm_app_uninit(void)
{
    fm_api_release();
    fm_manage_close();
    /* tone_play_stop(); */
    tone_play_stop_by_path(tone_table[IDEX_TONE_FM]);
    fm_idle_flag = 1;
}


static void  fm_tone_play_end_callback(void *priv, int flag)
{
    u32 index = (u32)priv;

    if (APP_FM_TASK != app_get_curr_task()) {
        log_error("tone callback task out \n");
        return;
    }

    switch (index) {
    case IDEX_TONE_FM:
        ///提示音播放结束， 启动播放器播放
        u_pa_control(PA_CLASS_AB);
#if USE_FM_VOL_FADE_IN
        sys_target=FM_FADE_IN_MAX_VOL;
        printf("+++++++++++++++++++++{%d %d}",sys_target,app_audio_get_volume(APP_AUDIO_CURRENT_STATE));
        app_audio_set_volume(APP_AUDIO_STATE_MUSIC, 0, 0);
        fm_digital_Fade_in_start();
#endif
        fm_app_start();
        break;
    default:
        break;
    }
}

//*----------------------------------------------------------------------------*/
/**@brief    fm主任务
   @param    无
   @return   无
   @note
*/
/*----------------------------------------------------------------------------*/
void app_fm_task()
{
    int msg[32];
    fm_app_init();
#if USE_FM_VOL_FADE_IN
    exit_fm=0;
#endif
#if TCFG_DEC2TWS_ENABLE
    extern void set_tws_background_connected_flag(u8 flag);
    extern u8 get_tws_background_connected_flag();
    if (get_tws_background_connected_flag()) { //不播放提示音
        fm_app_start();
        set_tws_background_connected_flag(0);
    } else
#endif
    {
        tone_play_with_callback_by_name(tone_table[IDEX_TONE_FM], 1, fm_tone_play_end_callback, (void *)IDEX_TONE_FM);
    }
//    if (err) {
//        ///提示音播放失败，直接启动播放
//        fm_app_start();
//    }

    while (1) {
        app_task_get_msg(msg, ARRAY_SIZE(msg), 1);

        switch (msg[0]) {
        case APP_MSG_SYS_EVENT:
            if (fm_event_handler((struct sys_event *)(&msg[1])) == false) {
                app_default_event_deal((struct sys_event *)(&msg[1]));    //由common统一处理
            }
            break;
        default:
            break;
        }

        if (app_task_exitting()) {
#if USE_FM_VOL_FADE_IN
            exit_fm=1;
            fm_digital_Fade_in_end();
#endif
            fm_app_uninit();
            return;
        }
    }
}

static u8 fm_idle_query(void)
{
    return fm_idle_flag;
}
REGISTER_LP_TARGET(fm_lp_target) = {
    .name = "fm",
    .is_idle = fm_idle_query,
};

#else


void app_fm_task()
{

}


#endif


