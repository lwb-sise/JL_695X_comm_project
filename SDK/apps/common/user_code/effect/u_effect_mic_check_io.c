#include "user_head.h"

#define LOG_TAG_CONST        U_EFFECT_MIC_IO
#define LOG_TAG             "[U_EFFECT_MIC_IO]"
#define LOG_ERROR_ENABLE
#define LOG_DEBUG_ENABLE
#define LOG_INFO_ENABLE
#define LOG_DUMP_ENABLE
#define LOG_CLI_ENABLE
#include "debug/user_debug.h"

#ifdef U_EFFECT_MODE_EN

#if (U_MIC_CHECK_CLASS == U_MIC_CHECK_IO)

/**
 * @brief 获取 mic 在线状态
 * 
 * @return int 0：不在线 -1：错误  1：mic1 在线 2: mic2 在线 3：mic1 mic2在线
 */
int u_effect_mic_io_onlin_get(EFFECT_INFO *info){
    int ret=0;
    if(!info->en){
        return -1;
    }

    for(int i=0;i<MY_SIZE(info->check_io);i++){
        if(info->check_io[i].on_line){
            ret|=BIT(i);
        }
    }

    return ret;
}


static void u_effect_mic_check_pause_backup(EFFECT_INFO *info){
    if(!info)return;

    IO_CHECK *check_io = info->check_io;
    if(!tone_get_status()){
        for(int i=0;MY_SIZE(info->check_io);i++){
            info->check_io[i].kid=0;
            info->check_io[i].en = true;
            info->check_io[i].old_state=false;
            info->check_io[i].on_line=false;
        }
    }else{
        check_io->kid=my_timeout_add(info,u_effect_mic_check_pause_backup,31);
    }
}



/**
 * @brief mic io check pause
 * 
 * @param ktimer 暂停时长
 */
void u_effect_mic_check_io_pause(u32 ktimer,EFFECT_INFO *info){
    u8 timeout_flag=0;
    for(int i=0;MY_SIZE(info->check_io);i++){
        info->check_io[i].en = false;
        if(info->check_io[i].kid){
            my_timeout_del(info->check_io[i].kid);
            info->check_io[i].kid=0;
        }

    }
    info->check_io[0].kid=my_timeout_add(info,u_effect_mic_check_pause_backup,ktimer);
}

/**
 * @brief check io init
 * 
 * @param check_io 
 */
void u_effect_mic_check_io_init(IO_CHECK *check_io,void (*func)(void *priv)){
    if(!check_io || !check_io->en)return;

    check_io->time=60;
    check_io->func=func;

    int ret=u_check_io_init(check_io);
    if(ret){
        log_error("mic check init error");
        return;
    }

    check_io->en=true;
}

/**
 * @brief 关闭 mic io 检测
 * 
 * @param check_io 
 */
void u_effect_mic_check_io_off(IO_CHECK *check_io){
    if(!check_io)return;
    u_check_io_off(&check_io);
    if(check_io->threshold<0){
        u_comm_gpio_set(check_io->io.pro,USER_GPIO_IN_IO_PU,0);
    }else{
        u_comm_gpio_set(check_io->io.pro,USER_GPIO_IN_IO_PD,0);
    }
    check_io->en=false;
}

#endif//U_EFFECT_MODE_EN

#endif