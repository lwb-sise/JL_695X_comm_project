#ifndef _U_TWS_H_
#define _U_TWS_H_


typedef  struct _TWS_INFO_{
    u8 tws_mic_status;//对箱 对方mic连接状态
}TWS_INFO;

typedef  enum {
    TWS_SYNC_NULL,//空
    TWS_SYNC_LED,//LED mode
    TWS_SYNC_RGB_KEEP,//RGB mode KEEP
    TWS_SYNC_RGB_NOR,//RGB mode NOR
    TWS_SYNC_EQ,//EQ mode
    TWS_SYNC_VOCAL,//人声消除状态
    TWS_SYNC_PA_MUTE,//功放手动mute状态
    TWS_SYNC_MIC_MUTE,//MIC mute状态
    TWS_SYNC_MIC_ONLINE,//MIC 在线 状态
    TWS_SYNC_MAX,//MAX
}TWS_SYNC;

/**
 * @brief 获取 tws mic状态
 * 
 * @return u8 
 */
u8 u_tws_mic_status_get(void);

/**
 * @brief 设置 tws mic 状态
 * 
 * @param tws_mic_status 
 */
void u_tws_mic_status_set(u8 tws_mic_status);

/**
 * @brief 对箱同步 TX
 * 
 */
void u_tws_sync_info(int num_args,...);

/**
 * @brief 对箱连接时状态同步 TX
 * 
 */
void u_tws_status_sync_all(void);
/**
 * @brief 对箱过一个状态同步到对方
 * 
 */
void u_tws_status_sync_one(TWS_SYNC sync);
/**
 * @brief 对箱连接时eq同步 TX
 * 
 */
void u_tws_eq_sync(void);
#endif
