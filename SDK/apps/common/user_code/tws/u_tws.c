#include "user_head.h"
#define LOG_TAG_CONST        U_TWS
#define LOG_TAG             "[U_TWS]"
#define LOG_ERROR_ENABLE
#define LOG_DEBUG_ENABLE
#define LOG_INFO_ENABLE
#define LOG_DUMP_ENABLE
#define LOG_CLI_ENABLE
#include "debug/user_debug.h"

#ifdef U_EFFECT_TWS_EN 
static TWS_INFO info={
    .tws_mic_status=0,
};

static void u_tws_sync_rec(void *_data, u16 len, bool rx);
#define TWS_FUNC_ID_USER_SYNC    TWS_FUNC_ID('U', 'S', 'Y', 'N')

REGISTER_TWS_FUNC_STUB(app_u_tws_sync_rec) = {
    .func_id = TWS_FUNC_ID_USER_SYNC,
    .func    = u_tws_sync_rec,
};



/**
 * @brief 设置同步状态
 * 
 * @param sync 获取项目
 * @param status 项目状态
 * 比较复杂的处理必须通过消息处理的方式在comm里面处理
 */
static void u_tws_sysnc_status_set(TWS_SYNC sync, u8 *status){
    switch (sync) {
    case TWS_SYNC_LED:
        log_debug("TWS_SYNC_LED=%d",*status);
        app_task_put_key_msg((*status)?USER_LIGHT_LED_ON:USER_LIGHT_LED_OFF, 0);
        break;
    case TWS_SYNC_RGB_NOR:
        log_debug("TWS_SYNC_RGB_NOR=%d",*status);
        app_task_put_key_msg(USER_LIGHT_RGB_SET, (*status));
        break;
    case TWS_SYNC_RGB_KEEP:
        log_debug("TWS_SYNC_RGB_KEEP=%d",*status);
        app_task_put_key_msg(USER_LIGHT_RGB_SET, (*status));
        break;
    case TWS_SYNC_VOCAL:
        log_debug("TWS_SYNC_VOCAL=%d",*status);
        app_task_put_key_msg((*status)?USER_AUDIO_VOCAL_REMOVE_ON:USER_AUDIO_VOCAL_REMOVE_OFF,0);
        break;
    case TWS_SYNC_PA_MUTE:
        log_debug("TWS_SYNC_PA_MUTE=%d",*status);
        u_pa_ex_info_set(PA_MANUAL,(*status));
        break;
    case TWS_SYNC_MIC_MUTE:
        log_debug("TWS_SYNC_MIC_MUTE=%d",*status);
        app_task_put_key_msg(KEY_MIC_VOL_MUTE, (*status));
        break;
    case TWS_SYNC_MIC_ONLINE:
        log_debug("TWS_SYNC_MIC_ONLINE=%d",*status);
        u_tws_mic_status_set(*status);
        app_task_put_key_msg(USER_TWS_MIC_STATUS, (*status));
        break;
    case TWS_SYNC_EQ:
        log_debug("TWS_SYNC_EQ=%d",*status);
        app_task_put_key_msg(USER_EQ_SET, (*status));
        break;
        
    
    default:
        break;
    }
}


/**
 * @brief 获取同步状态
 * 
 * @param sync 获取项目
 * @param status 项目状态
 * @return int 0:成功 其他:失败
 */
static int u_tws_sysnc_status_get(TWS_SYNC sync, u8 *status){
    int ret=0;
    switch (sync) {
    case TWS_SYNC_LED:
        *status=u_led_get();
        log_debug("TWS_SYNC_LED=%d",*status);
        break;
    case TWS_SYNC_RGB_KEEP:
        // if(get_rgb_mode(MODE_KEEP)>-1){
        //     *status=get_rgb_mode(MODE_KEEP);
        // }
        log_debug("TWS_SYNC_RGB_KEEP=%d",*status);
        break;
    case TWS_SYNC_RGB_NOR:
        // if(get_rgb_mode(MODE_NOR)>-1){
        //     *status=get_rgb_mode(MODE_NOR);
        // }
        log_debug("TWS_SYNC_RGB_NOR=%d",*status);
        break;
    case TWS_SYNC_VOCAL:
        *status=user_vocal_remove_get();
        log_debug("TWS_SYNC_VOCAL=%d",*status);
        break;
    case TWS_SYNC_PA_MUTE:
        *status=u_pa_ex_info_get(PA_MANUAL);
        log_debug("TWS_SYNC_PA_MUTE=%d",*status);
        break;
    case TWS_SYNC_MIC_MUTE:
        *status=u_effect_mic_mute_get();
        log_debug("TWS_SYNC_MIC_MUTE=%d",*status);
        break;
    case TWS_SYNC_MIC_ONLINE:
        if(u_effect_mic_onlin_get()<0){
            log_error("err TWS_SYNC_MIC_ONLINE=%d",u_effect_mic_onlin_get());
            return -1;
        }
        *status=u_effect_mic_onlin_get();
        log_debug("TWS_SYNC_MIC_ONLINE=%d",*status);
        break;
    case TWS_SYNC_EQ:
        *status=eq_mode_get_cur();
        log_debug("TWS_SYNC_EQ=%d",*status);
        break;
    // case TWS_SYNC_BAT:
    //     break;
    // case TWS_SYNC_TEMP:
    //     break;
    default:
        break;
    }
    return ret;
}

/**
 * @brief 对箱同步 RX
 * 
 * @param _data 
 * @param len 
 * @param rx 
 */
static void u_tws_sync_rec(void *_data, u16 len, bool rx){
    u8 *rec=_data;
    u8 user_len = rec[0];
    if (rx && len && user_len) {
        log_debug("rec len=%d\r\n",len);
        // for(int i=0;i<len;i++)
        //     printf("[i=%d data=%d]\r\n",i,rec[i]);
        u8 * data = &rec[1];

        for(int i=0;i<len;i++){
            log_debug("RX I=%d data=%d",i,((u8 *)_data)[i]);
        }
        for(int i=0;i<user_len;i++){
            // log_debug("i=%d len =%d sync = %d data=%d",i,len,data[i*2],data[i*2+1]);
            u_tws_sysnc_status_set(data[i*2],&data[i*2+1]);
        }
    }

}

/**
 * @brief 对箱同步 TX
 * 
 * -------------------------------------------------------------------------------------------------------------------------------
 * 
 * -------------------------------------------------------------------------------------------------------------------------------
 *  一个字节    |   一个字节    |   一个字节    |   一个字节    |   一个字节    |   一个字节      | 一个字节   |   
 * -------------------------------------------------------------------------------------------------------------------------------
 *  同步项数量n |   同步项目1   |   同步项目*** |   同步项目n   |   同步项目1值 |   同步项目***值 | 同步项目n值 |
 * 
 * -------------------------------------------------------------------------------------------------------------------------------
 */
void u_tws_sync_info(int num_args,...){
    #ifdef TCFG_USER_TWS_ENABLE

    u8 sed_buff[((num_args*2)/4)*4+4];
    memset(sed_buff, 0, sizeof(sed_buff));

    sed_buff[0]=num_args;
    u8 *data=&sed_buff[1];
    
    TWS_SYNC sync;
    va_list args;
    va_start(args, num_args);

    for (int i = 0; i < num_args; i+=2){
        sync = va_arg(args, int);
        if(sync<=TWS_SYNC_NULL || sync>=TWS_SYNC_MAX){
            continue;
        }
        // sed_buff[i+1]=TWS_SYNC_NULL;
        data[i*2]=sync;
        if(u_tws_sysnc_status_get(sync,&data[i*2+1])){
            data[i*2]=TWS_SYNC_NULL;
        }
        // log_debug("sync = %d",sync,data[num_args+i]);
    }
    va_end(args);

    log_debug("---sizeof(sed_buff)=%d",sizeof(sed_buff));
    for(int i=0;i<sizeof(sed_buff);i++)
        printf("[i=%d data=%d]\r\n",i,sed_buff[i]);
    // tws_api_send_data_to_slave(sed_buff, sizeof(sed_buff), TWS_FUNC_ID_USER_SYNC);
    tws_api_send_data_to_sibling(sed_buff, sizeof(sed_buff), TWS_FUNC_ID_USER_SYNC);
   
    #endif
}

/**
 * @brief 对箱连接时状态同步 TX
 * 
 */
void u_tws_status_sync_all(void){
    log_debug("MASTER SEND STATUS SYNC %d",tws_api_get_role());
    if(tws_api_get_role() != TWS_ROLE_SLAVE){
        log_debug("MASTER SEND SYNC INFO DATA");
        u_tws_sync_info(5,TWS_SYNC_LED,TWS_SYNC_EQ,TWS_SYNC_VOCAL,TWS_SYNC_MIC_ONLINE,TWS_SYNC_PA_MUTE,TWS_SYNC_MIC_MUTE,TWS_SYNC_RGB_NOR,TWS_SYNC_RGB_KEEP);
    }
}

/**
 * @brief 对箱过一个状态同步到对方
 * 
 */
void u_tws_status_sync_one(TWS_SYNC sync){
    u_tws_sync_info(1,sync);
}

/**
 * @brief 获取 tws mic状态
 * 
 * @return u8 
 */
u8 u_tws_mic_status_get(void){
    return info.tws_mic_status;
}

/**
 * @brief 设置 tws mic 状态
 * 
 * @param tws_mic_status 
 */
void u_tws_mic_status_set(u8 tws_mic_status){
    info.tws_mic_status=tws_mic_status;
}


#endif//#ifdef U_EFFECT_TWS_EN 