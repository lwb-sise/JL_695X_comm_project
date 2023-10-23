
#include "u_wireless_list.h"

#ifndef __U_WIRELESS_H__
#define __U_WIRELESS_H__

//自定义发送数据长度
#define U_SEND_DATA_LEN (8)

//697
#undef  SLAVE_ENC_COMMAND_LEN
#define SLAVE_ENC_COMMAND_LEN   U_SEND_DATA_LEN

//7006 需要改小码率不然默认只支持4个字节的长度
#undef BROADCAST_CUSTOM_DATA_LEN
#define BROADCAST_CUSTOM_DATA_LEN   U_SEND_DATA_LEN


#define KEY_SER_NUM_MAX (10)
#define KEY_SER_EVN_MAX KEY_EVENT_MAX
typedef enum {//用户id
    SERIAL_CODE_ID_AMW,
    SERIAL_CODE_ID_JHC,
    SERIAL_CODE_ID_JY,
    SERIAL_CODE_ID_MAX,
}SERIAL_CODE_ID;

typedef enum {//事件
    SERIAL_EVE_DEV,//设备事件
    SERIAL_EVE_KEY,//按键事件
    SERIAL_EVE_USER,//自定义事件
    SERIAL_EVE_MAX,
}SERIAL_EVE;

/**
 * SERIAL_EVE_DEV
*/
typedef enum {
    DEV_EVE_ONLIEN_MIC=1,//1T1 MIC 状态
    DEV_EVE_ONLIEN_MIC_1,//2T1 MIC 1 状态
    DEV_EVE_ONLIEN_MIC_2,//2T1 MIC 2 状态
    DEV_EVE_ONLIEN_MIC_ALL,//2T1 MIC 2 状态
    DEV_EVE_MAX,
}DEV_EVE;

/**
 * SERIAL_EVE_KEY
 * TX 加減按键功能状态
*/
typedef enum {
    KEY_EVE_MIC_VOL,//加減按键 调节mic vol
    KEY_EVE_REVERB,//混响
    KEY_EVE_HALF,//伴奏Half-play
    KEY_EVE_MAX,
}KEY_EVE;


typedef enum {
    SDK_FLAG_2T1,
    SDK_FLAG_1T1,
}SDK_FLAG;

/**
 * SERIAL_EVE_USER
*/
typedef enum {
    USER_EVE_MIC_VOL=1,//MIC VOL
    USER_EVE_PLAY_MAX_VOL,//播放最大音量
    USER_EVE_PLAY_MIN_VOL,//播放最小音量
    USER_EVE_PLAY_KEY_FUN,//按键功能变化
    USER_EVE_MAX,
}USER_EVE;



typedef struct _WIRELESS_INFO_{
    SERIAL_CODE_ID id;//用户id
    u16 send_id;//scan timeid
    u16 heartbeat_id;//心跳id
    u8 cs;//连接状态
    KEY_EVE ks;//加减按键状态
    SDK_FLAG flag;//sdk flag 2T1 1T1

    u32 Filter_time;//复用按键切换按键功能时的滤波时间按

    void (*rx_send)(const u8 *outbuf, u32 len);
    u32(*rx_read)(u8 *inbuf, u32 len, u32 timeout); 
}W_INFO;

/**
 * @brief 设置无线麦连接状态
 * 
 * @param dev 设备
 * @param state 状态
 */
void u_effect_mic_onlin_set(DEV_EVE dev,bool state);

/**
 * @brief 存储预发数据
 * 
 * @param buff 预发数据
 * @param len 预发数据长度
 */
void u_send_default_data_save(u8 *buff,u32 len);

/**
 * @brief 获取预发数据
 * 
 * @param buff 获取数据存放的地址
 * @param int 数据长度
 */
int u_default_data_get(u8 *buff);

/**
 * @brief tx 预发自定义事件
 * 
 * @param num_args ...代表的参数个数
 * @param ... 
 */
void u_default_save_user(int num_args,...);

/**********************************TX*******************************************/
/**
 * @brief tx获取预发数据
 * 
 * @param buff 获取数据存放的地址
 * @param int 数据长度
 */
int u_wtx_data_get(u8 *buff);

/**
 * 加减按键功能变更
*/
void u_wtx_key_state_loogp_update(void);

/**
 * @brief tx 预发key事件
 * 
 * @param key 
 * @param value 
 */
void u_wtx_send_default_key(u8 key,u8 value);


/**********************************RX*******************************************/

/**
 * @brief 初始化rx to mcu scan
 * 
 */
void u_wrx_to_mcu_init(void);

/**
 * @brief tx转发rx数据到mcu
 * 
 * @param buff 
 */
void u_wrx_forward_data(u8 *buff);

#endif