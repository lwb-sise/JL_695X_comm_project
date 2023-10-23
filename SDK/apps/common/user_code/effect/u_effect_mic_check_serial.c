#include "user_head.h"

#define LOG_TAG_CONST        U_EFFECT_MIC_SERIAL
#define LOG_TAG             "[U_EFFECT_MIC_SERIAL]"
#define LOG_ERROR_ENABLE
#define LOG_DEBUG_ENABLE
#define LOG_INFO_ENABLE
#define LOG_DUMP_ENABLE
#define LOG_CLI_ENABLE
#include "debug/user_debug.h"


#ifdef U_EFFECT_MODE_EN && U_SERIAL_MODE_EN

#if (U_MIC_CHECK_CLASS==U_MIC_CHECK_SERIAL)


static void u_effect_mic_check_pause_backup(EFFECT_INFO *info){
    if(!info)return;

    info->kid=0;

    if(!tone_get_status()){
        u_eff_mic_effect_start_or_stop();
    }else{
        info->kid=my_timeout_add(info,u_effect_mic_check_pause_backup,31);
    }
}
/**
 * @brief mic io check pause
 * 
 * @param ktimer 暂停时长
 */
void u_effect_mic_check_serial_pause(u32 ktimer,EFFECT_INFO *info){
    if(info->kid){
        my_timeout_del(info->kid);
        info->kid=0;
    }

    info->kid=my_timeout_add(info,u_effect_mic_check_pause_backup,ktimer);
}

#endif//U_EFFECT_MODE_EN && U_SERIAL_MODE_EN

#endif