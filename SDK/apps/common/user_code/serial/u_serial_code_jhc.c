#include "user_head.h"


#define LOG_TAG_CONST        U_SERIAL_CODE
#define LOG_TAG             "[U_SERIAL_CODE_JHC]"
#define LOG_ERROR_ENABLE
#define LOG_DEBUG_ENABLE
#define LOG_INFO_ENABLE
#define LOG_DUMP_ENABLE
#define LOG_CLI_ENABLE
#include "debug/user_debug.h"

#ifdef U_EFFECT_MODE_EN && U_SERIAL_MODE_EN
/**
 * @brief 设备消息处理
 * 
 * @param buf 
 */
void u_serial_code_jhc_dev(u8 *buff,u16 len){
    
}

/**
 * @brief 按键事件处理
 * 
 * @param buf 
 */
void u_serial_code_jhc_key(u8 *buff,u16 len){
    
}

/**
 * @brief 自定义事件处理
 * 
 * @param buf 
 */
void u_serial_code_jhc_user(u8 *buff,u16 len){

    #define U_DODE_LENG 6
    if(len<U_DODE_LENG){
        return;
    }

    u8 code=0xff;
    u8 code_table[][U_DODE_LENG]={
    {0x55,0xaa,0x03,0x0a,0x0,U_DISCONNECTED},
    {0x55,0xaa,0x03,0x0a,0x1,U_CONNECTED},
    {0x55,0xaa,0x03,0x0a,0x2,U_MIC_MUTE},
    {0x55,0xaa,0x03,0x0b,0x01,U_MUSIC_PP},
    {0x55,0xaa,0x03,0x0b,0x02,U_MUSIC_PREV},
    {0x55,0xaa,0x03,0x0b,0x03,U_MUSIC_NEXT},
    {0x55,0xaa,0x03,0x0b,0x04,U_MUSIC_UP},
    {0x55,0xaa,0x03,0x0b,0x05,U_MUSIC_DOW},
    {0x55,0xaa,0x03,0x0b,0x06,U_MODE},
    {0x55,0xaa,0x03,0x0b,0x07,U_SOUND_OFF},
    {0x55,0xaa,0x03,0x0b,0x08,U_TONE_VOL_MAX},
    {0x55,0xaa,0x03,0x0b,0x09,U_TONE_VOL_MIN},
    {0x55,0xaa,0x03,0x0b,0x0a,U_TONE_MIC_ON},
    {0x55,0xaa,0x03,0x0b,0x0b,U_TONE_MIC_OFF},
    {0x55,0xaa,0x03,0x0b,0x0c,U_EFFECT_MODE},
    {0x55,0xaa,0x03,0x0b,0x0d,U_TONE_MIC_VOL},
    {0x55,0xaa,0x03,0x0b,0x0e,U_TONE_EFFECT},
    {0x55,0xaa,0x03,0x0b,0x0f,U_TONE_ACCOMPANIMENT},
    {0x55,0xaa,0x03,0x0b,0x10,U_MIC_UP},
    {0x55,0xaa,0x03,0x0b,0x11,U_MIC_DOW},
    {0x55,0xaa,0x03,0x0b,0x12,U_EFFECT_UP},
    {0x55,0xaa,0x03,0x0b,0x13,U_EFFECT_DOW},
    {0x55,0xaa,0x03,0x0b,0x14,U_MAGIC_SOUND},
    };

    for(int i=0;i<MY_SIZE(code_table);i++){
        if(!memcmp(code_table[i],buff,(U_DODE_LENG-1))){
            code=code_table[i][U_DODE_LENG-1];
            break;
        }
    }


    switch (code){
    case U_DISCONNECTED:
        /** MIC1 未连上*/
        break;
    case U_CONNECTED:
        /** MIC1 连上并且处于打开状态*/
        /* code */
        break;
    case U_MIC_MUTE:
        /** MIC2 连上但处于关关闭状态*/
    case U_DISCONNECTED_MIC2:
        /** MIC2 未连上*/
        break;
    case U_CONNECTED_MIC2:
        /** MIC2 连上并且处于打开状态*/
        break;
    case U_MIC_MUTE_MIC2:
        /** MIC2 连上但处于关关闭状态*/
        break;

    case U_MUSIC_PP://播放暂停
        app_task_put_key_msg(KEY_MUSIC_PP, 0);
        break;
    case U_MUSIC_PREV://上一曲
        app_task_put_key_msg(KEY_MUSIC_PREV, 0);
        break;
    case U_MUSIC_NEXT://下一曲
        app_task_put_key_msg(KEY_MUSIC_NEXT, 0);
        break;
    case U_MUSIC_UP://伴奏加
        app_task_put_key_msg(KEY_ACCOMPANIMENT_UP, 0);
        break;
    case U_MUSIC_DOW://伴奏减
        app_task_put_key_msg(KEY_ACCOMPANIMENT_DOWN, 0);
        break;
    case U_MODE://模式键
        app_task_put_key_msg(KEY_CHANGE_MODE, 0);
        break;
    case U_SOUND_OFF://消原音
        app_task_put_key_msg(USER_AUDIO_VOCAL_REMOVE_SWITCH, 0);
        break;
    case U_TONE_VOL_MAX://播放“话筒音量已最大”
        app_task_put_key_msg(KEY_PLAY_TONE_MIC_VOL_MAX, 0);
        break;
    case U_TONE_VOL_MIN://播放“话筒音量已最小”提示音
        app_task_put_key_msg(KEY_PLAY_TONE_MIC_VOL_MIN, 0);
        break;
    case U_TONE_MIC_ON://播放“话筒开”提示音
        app_task_put_key_msg(KEY_PLAY_TONE_MIC_OPEN, 0);
        break;
    case U_TONE_MIC_OFF://播放“话筒关”提示音
        app_task_put_key_msg(KEY_PLAY_TONE_MIC_CLOSE, 0);
        break;
    case U_EFFECT_MODE://音效切换(切换喊话效果 eg:男变女 女变男 等)
        app_task_put_key_msg(KEY_REVERB_CHANGE_MODE, 0);
        break;
    case U_TONE_MIC_VOL://播放"话筒调节"提示音
        app_task_put_key_msg(KEY_PLAY_TONE_MIC_ADJUST, 0);
        break;
    case U_TONE_EFFECT://播放"混响调节"提示音
        app_task_put_key_msg(KEY_PLAY_TONE_REVERB_ADJUST, 0);
        break;
    case U_TONE_ACCOMPANIMENT://播放"伴奏调节"提示音
        app_task_put_key_msg(KEY_PLAY_TONE_ACCOMPANIMENT_ADJUST, 0);
        break;
    case U_MIC_UP://话筒加(预留，暂时没用到)
        app_task_put_key_msg(KEY_MIC_VOL_UP, 0);
        break;
    case U_MIC_DOW://话筒减(预留，暂时没用到)
        app_task_put_key_msg(KEY_MIC_VOL_DOWN, 0);
        break;
    case U_EFFECT_UP://混响加
        app_task_put_key_msg(KEY_REVERB_UP, 0);
        break;
    case U_EFFECT_DOW://混响减
        app_task_put_key_msg(KEY_REVERB_DOWN, 0);
        break;
    case U_MAGIC_SOUND://魔音键
        app_task_put_key_msg(KEY_MAGIC, 0);
        break;
    case U_RX_XIAO_DU_AI://小度APP语音开启
        app_task_put_key_msg(KEY_SEND_SPEECH_START,0);
        break;
    case U_DODGE_EN:
        app_task_put_key_msg(KEY_DODGE_EN,0);
        break;
    case U_RGB_MODE_SWITCH:
        app_task_put_key_msg(USER_LIGHT_RGB_SWITCH,0);
        break;
    default:
        break;
    }
}

#endif//#ifdef U_EFFECT_MODE_EN && U_SERIAL_MODE_EN