#ifndef _U_EFFECT_MIC_CHECK_SERIAL_
#define _U_EFFECT_MIC_CHECK_SERIAL_

#ifdef U_EFFECT_MODE_EN && U_SERIAL_MODE_EN
/**
 * @brief mic serial check pause
 * 
 * @param ktimer 暂停时长
 */
void u_effect_mic_check_serial_pause(u32 ktimer,EFFECT_INFO *info);
#endif//U_EFFECT_MODE_EN && U_SERIAL_MODE_EN

#endif