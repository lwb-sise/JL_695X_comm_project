#ifndef _U_EFFECT_MIC_CHECK_IO_
#define _U_EFFECT_MIC_CHECK_IO_

#ifdef U_EFFECT_MODE_EN
/**
 * @brief mic io check init
 * 
 * @param check_io 
 */
void u_effect_mic_check_io_init(IO_CHECK *check_io,void (*func)(void *priv));


/**
 * @brief 关闭 mic io 检测
 * 
 * @param check_io 
 */
void u_effect_mic_check_io_off(IO_CHECK *check_io);

/**
 * @brief mic io check pause
 * 
 * @param ktimer 暂停时长
 */
void u_effect_mic_check_io_pause(u32 ktimer,EFFECT_INFO *info);


/**
 * @brief 获取 mic 在线状态
 * 
 * @return int 0：不在线 -1：错误  1：mic1 在线 2: mic2 在线 3：mic1 mic2在线
 */
int u_effect_mic_io_onlin_get(EFFECT_INFO *info);

#endif//U_EFFECT_MODE_EN

#endif