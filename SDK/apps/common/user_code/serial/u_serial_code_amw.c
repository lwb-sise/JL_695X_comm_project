#include "user_head.h"


#define LOG_TAG_CONST        U_SERIAL_CODE
#define LOG_TAG             "[U_SERIAL_CODE_AMW]"
#define LOG_ERROR_ENABLE
#define LOG_DEBUG_ENABLE
#define LOG_INFO_ENABLE
#define LOG_DUMP_ENABLE
#define LOG_CLI_ENABLE
#include "debug/user_debug.h"

#ifdef U_EFFECT_MODE_EN && U_SERIAL_MODE_EN
/**
 * @brief 设备事件处理
 * 
 * @param buff 
 * @param len 
 */
void u_serial_code_amw_dev(u8 *buff,u16 len){
    DEV_EVE mic=buff[0];
    SDK_FLAG flag=buff[1];
    u8 on_line_in=buff[2];
    u8 on_line=u_effect_mic_onlin_get();

    // log_debug("[%d %d %d %d]",mic,flag,on_line_in,on_line);

    if(2>len || (SDK_FLAG_1T1!=flag && SDK_FLAG_2T1!=flag) || tone_get_status())return;

    if(DEV_EVE_ONLIEN_MIC_ALL!=mic){
        log_debug("不需要单独设置mic的状态");
        return;
    }

    if(SDK_FLAG_1T1==flag){
        if(U_UART_TASK_1==os_current_task()){
            on_line_in?(on_line|=BIT(DEV_EVE_ONLIEN_MIC_1)):(on_line&=(~BIT(DEV_EVE_ONLIEN_MIC_1)));
        }else if(U_UART_TASK_2==os_current_task()){
            on_line_in?(on_line|=BIT(DEV_EVE_ONLIEN_MIC_2)):(on_line&=(~BIT(DEV_EVE_ONLIEN_MIC_2)));
        }
    }else if(SDK_FLAG_2T1==flag){
        on_line=on_line_in;
    }

    //log_debug("mic all status=[MIC=%d,MIC1=%d,MIC2=%d]",\
    !!(on_line_in&BIT(DEV_EVE_ONLIEN_MIC)),\
    !!(on_line_in&BIT(DEV_EVE_ONLIEN_MIC_1)),\
    !!(on_line_in&BIT(DEV_EVE_ONLIEN_MIC_2))\
    );
    // log_debug("on_line=%d",on_line);
    u_effect_mic_onlin_set(DEV_EVE_ONLIEN_MIC_ALL,on_line);
}

void mic1_online_mode_timer(){
    app_task_put_key_msg(KEY_PLAY_TONE_MIC1_CONNECT, 0);
}

void mic2_online_mode_timer(){
    app_task_put_key_msg(KEY_PLAY_TONE_MIC2_CONNECT, 0);
}

/**
 * @brief 按键事件处理
 * 
 * @param buff 
 * @param len 
 */
void u_serial_code_amw_key(u8 *buff,u16 len){
    //单击             //长按          //hold         //抬起            //双击                //三击
    u32 serial_key_table[KEY_SER_NUM_MAX][KEY_SER_EVN_MAX]={
        [0]={KEY_NULL,KEY_NULL,KEY_NULL,KEY_NULL,KEY_MUSIC_PP},
        [3]={USER_AUDIO_TONE_CTRL,KEY_MIC_VOL_MUTE,KEY_NULL,KEY_NULL,USER_AUDIO_VOCAL_REMOVE_SWITCH},

        //伴奏
        /*加*/[1]={KEY_ACCOMPANIMENT_UP,KEY_MUSIC_PREV,KEY_NULL,KEY_NULL,KEY_ACCOMPANIMENT_UP},
        /*减*/[2]={KEY_ACCOMPANIMENT_DOWN,KEY_MUSIC_NEXT,KEY_NULL,KEY_NULL,KEY_ACCOMPANIMENT_DOWN},

        //混响调节
        /*加*/[4]={KEY_REVERB_UP,KEY_REVERB_UP,KEY_REVERB_UP,KEY_NULL,KEY_REVERB_UP},
        /*减*/[5]={KEY_REVERB_DOWN,KEY_REVERB_DOWN,KEY_REVERB_DOWN,KEY_NULL,KEY_REVERB_DOWN},
    };
    u8 ks=buff[0];
    u8 key=buff[1];
    u8 key_eve=buff[2];
    log_debug("ks=%d key=%d value=%d\r\n",ks,key,key_eve);
    if(1==key || 2==key){
        if(KEY_EVE_MIC_VOL==ks)return;//mic vol tx端调节
        else if(KEY_EVE_REVERB==ks){
            key+=3;
        }
    }

    u16 msg=serial_key_table[key][key_eve];
    if(len<3 || KEY_NULL==msg || !msg)return;   
    
    r_printf("key=%d eve=%d msg=%d\r\n",key,key_eve,msg);

    app_task_put_key_msg(msg, 0);
}

/**
 * @brief 自定义事件处理
 * 
 * @param buff 
 * @param len 
 */
void u_serial_code_amw_user(u8 *buff,u16 len){
    switch (buff[0]){
    case USER_EVE_PLAY_KEY_FUN:
        log_debug("USER_EVE_PLAY_KEY_FUN");
        r_printf("ks---%d--",buff[1]);
        // static bool flag=false;
        // flag=!flag;
        // if(flag){
        //     cur_eq_set_global_gain(AEID_MUSIC_EQ, -12);
        // }else{
        //     cur_eq_set_global_gain(AEID_MUSIC_EQ, 0);
        // }
        // break;
        //统一放到u_comm里面处理
        if(KEY_EVE_MIC_VOL==buff[1]){
            app_task_put_key_msg(KEY_PLAY_TONE_MIC_ADJUST, 0);
        }else if(KEY_EVE_HALF==buff[1]){
            app_task_put_key_msg(KEY_PLAY_TONE_ACCOMPANIMENT_ADJUST, 0);
        }else if(KEY_EVE_REVERB==buff[1]){
            app_task_put_key_msg(KEY_PLAY_TONE_REVERB_ADJUST, 0);
        }
        break;
    case USER_EVE_PLAY_MAX_VOL:
        app_task_put_key_msg(KEY_PLAY_TONE_MIC_VOL_MAX, 0);
        break;
    case USER_EVE_PLAY_MIN_VOL:
        app_task_put_key_msg(KEY_PLAY_TONE_MIC_VOL_MIN, 0);
        break;
    case USER_EVE_MIC_VOL:
        log_debug("---mic vol %d %d\r\n",buff[1],buff[2]);
        u8 max_vol=buff[2];
        u8 vol =buff[1];
        if(!vol){
            app_task_put_key_msg(KEY_PLAY_TONE_MIC_VOL_MIN, 0);
        }else if (max_vol == vol){
            app_task_put_key_msg(KEY_PLAY_TONE_MIC_VOL_MAX, 0);
        }
        break;
    
    default:
        break;
    }

}

#endif//#ifdef U_EFFECT_MODE_EN && U_SERIAL_MODE_EN