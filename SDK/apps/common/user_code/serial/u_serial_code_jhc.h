#ifndef __U_SERIAL_CODE_JHC_H__
#define __U_SERIAL_CODE_JHC_H__

typedef  enum {
    U_MUSIC_PP,//音乐 暂停/播放
    U_MUSIC_PREV,//音乐 上一曲
    U_MUSIC_NEXT,//音乐 下一曲
    U_MUSIC_UP,//音乐 加
    U_MUSIC_DOW,//音乐 减
    U_MODE,//模式
    U_SOUND_OFF,//消原声
    // U_SILENCE,//silence  消人声
    U_TONE_VOL_MAX,//MIC 最大声提示音
    U_TONE_VOL_MIN,//MIC 最小声提示音
    U_TONE_MIC_ON,//开MIC提示音
    U_TONE_MIC_OFF,//关MIC提示音
    U_EFFECT_MODE,//effect mode 切换
    U_TONE_MIC_VOL,//MIC VOL调节 提示音
    U_TONE_EFFECT,//混响调节 提示音
    U_TONE_ACCOMPANIMENT,//伴奏调节 提示音
    U_MIC_UP,//MIC 加
    U_MIC_DOW,//MIC 减
    U_EFFECT_UP,//混响 加
    U_EFFECT_DOW,//混响 减
    U_MAGIC_SOUND,//魔音

    U_RX_XIAO_DU_AI,//小度AI功能
    U_DODGE_EN,     //闪避开关
    U_RGB_MODE_SWITCH,  //RGB模式

    U_DISCONNECTED,
    U_CONNECTED,
    U_MIC_MUTE,

    U_DISCONNECTED_MIC2,
    U_CONNECTED_MIC2,
    U_MIC_MUTE_MIC2,
}SERIAL_CODE;

/**
 * @brief 设备消息处理
 * 
 * @param buf 
 */
void u_serial_code_jhc_dev(u8 *buff,u16 len);
/**
 * @brief 按键事件处理
 * 
 * @param buf 
 */
void u_serial_code_jhc_key(u8 *buff,u16 len);
/**
 * @brief 自定义事件处理
 * 
 * @param buf 
 */
void u_serial_code_jhc_user(u8 *buff,u16 len);

#endif