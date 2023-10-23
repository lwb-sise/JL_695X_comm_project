#include "user_head.h"


#define LOG_TAG_CONST        U_CHECK_VBAT
#define LOG_TAG             "[U_CHECK_VBAT]"
#define LOG_ERROR_ENABLE
#define LOG_DEBUG_ENABLE
#define LOG_INFO_ENABLE
#define LOG_DUMP_ENABLE
#define LOG_CLI_ENABLE
#include "debug/user_debug.h"

#ifdef U_VBAT_MODE_EN
CHECK_VBAT_INFO info_check_vbat={
    .en=true,
    .io.mult=NO_CONFIG_PORT,
    .io.pro=IO_PORTB_08,
    .ad_ch=AD_CH_PB8,
    .up=1000,
    .dow=1000,
    .sample_freq=10,
};
#endif



/**
 * @brief 电源电压滤波
 *
 * @param in_vlaue 输入值
 * @return int 返回一段时间内的 最大值
 */
static int u_filter_vbat_level(int in_vlaue){
    #define USER_SAVE_TIMER (1000)//ms
    #define USER_SAVE_MAX_VBAT   (10)//周期N=USER_SAVE_TIMER*USER_SAVE_MAX_VBAT ms

    int vabt_max = 0;
    static int table_max[USER_SAVE_MAX_VBAT]={0};

    u32 cur_ms = (timer_get_ms()/USER_SAVE_TIMER)%USER_SAVE_MAX_VBAT;
    static u32 old_ms = 0;
    if(old_ms!=cur_ms){
        table_max[cur_ms] = 0;
        old_ms = cur_ms;
    }

    if(table_max[cur_ms]<in_vlaue){
        table_max[cur_ms] = in_vlaue;
    }

    //最大值
    for(int i = 0;i<USER_SAVE_MAX_VBAT;i++){
        if(vabt_max<table_max[i]){
            vabt_max = table_max[i];
        }
    }

    return vabt_max;
}

/**
 * @brief 电池电压检测初始化
 * 
 */
void u_check_vbat_init(void){
    #ifdef U_VBAT_MODE_EN

    gpio_set_die(info_check_vbat.io.pro, 0);
    gpio_set_pull_down(info_check_vbat.io.pro, 0);
    gpio_set_pull_up(info_check_vbat.io.pro, 0);
    gpio_set_direction(info_check_vbat.io.pro, 1);
    
    os_time_dly(1);

    adc_add_sample_ch(info_check_vbat.ad_ch);
    adc_set_sample_freq(info_check_vbat.ad_ch, info_check_vbat.sample_freq);
    log_info("u_check_vbat_init ok!!!");
    #endif 
}

int get_mid_value(int* arr,int num){
    int temp = 0;
    for(int i=0;i<num;i++){
        // log_info("arr[%d]==%d",i,arr[i]);
        for(int j=0;j<num;j++){
            if(arr[i] > arr[j]){
                temp = arr[i];
                arr[i] = arr[j];
                arr[j] = temp;
            }
        }
    }
    return arr[num/2];
}
/**
 * @brief 电源电压滤波
 * 
 * @param in_value 输入值
 * @return int 返回一段时间内的 中间值
 */
int user_filter_vbat_level_mid(int in_value){
    #define USER_SAVE_MID_VBAT   (50)//滤波时间 单位s

    int vabt_mid = 0;
    static int table_mid[USER_SAVE_MID_VBAT]={0};
    static int index = 0;
    // log_info(">>>>>>>index==%d",index);
    table_mid[index++] = in_value;
    vabt_mid = get_mid_value(table_mid,USER_SAVE_MID_VBAT);
    if(index == USER_SAVE_MID_VBAT){
        index = 0;
    }

    return vabt_mid;
}


/**
 * @brief 获取电池电压
 * 
 * @param in_vbat 默认传入值
 * @return int 
 * err：in_vbat 其他：电池电压
 */
int u_get_vbat(int in_vbat){
    #ifdef U_VBAT_MODE_EN

    if(!info_check_vbat.en ||!info_check_vbat.up || !info_check_vbat.dow)return in_vbat;
    int adc_res = adc_get_voltage(info_check_vbat.ad_ch);
    // log_info("adc_res ==%d",adc_res);
    adc_res=adc_res*(info_check_vbat.up+info_check_vbat.dow)/info_check_vbat.dow;
    in_vbat=user_filter_vbat_level_mid(adc_res);
    // log_info("in_vbat==%d",in_vbat/10);
    return in_vbat/10;
    #endif

    return in_vbat;
}