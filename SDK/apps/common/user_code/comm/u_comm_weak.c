/**
 * @file u_comm_weak.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-07-14
 * 
 * @copyright Copyright (c) 2023
 * 
 * 若函数接口声明
 */
#include "user_head.h"


#define LOG_TAG_CONST        U_COMM
#define LOG_TAG             "[U_COMM_WEAK]"
#define LOG_ERROR_ENABLE
#define LOG_DEBUG_ENABLE
#define LOG_INFO_ENABLE
#define LOG_DUMP_ENABLE
#define LOG_CLI_ENABLE
#include "debug/user_debug.h"

/**
 * @brief 获取gpio对应的ad通道
 * 
 * @param pr io口
 * @return int -1：错误 其他正确
 */
__attribute__((weak)) int u_comm_get_gpio_ad_ch(u32 pr){
    int ret=-1;
    return ret;
}

/**
 * @brief 获取gpio 属于哪一组
 * 
 * @param gpio 返回当前组的PN00
 * @return int error:-1
 */
__attribute__((weak)) int u_comm_gpio_pro_get(u32 gpio){
    return -1;
}

/*
*@brief project init
*
*/
__attribute__((weak)) void u_project_init(void){
    log_debug("弱函数");
}

/*
*@brief project eixt
*
*/
__attribute__((weak)) void u_project_exit(void){
    log_debug("弱函数");
}

/**
 * @brief 处理dac上电噗噗声
 * 
 */
__attribute__((weak)) void u_comm_dac_pupu(void){

}

/**
 * @brief 变音切换
 * 
 */
__attribute__((weak))  void mic_voicechange_switch(u8 eff_mode){

}

/**
 * @brief u_gpio_set_dir_input
 *
 * @param gpio 参考宏IO_PORTx_xx，如IO_PORTA_00
 * @param dir 1，输入；0，输出
 *
 * @return
 */
__attribute__((weak))  int u_gpio_set_dir_input(u32 gpio, u32 dir){
    return 0;
}

/**
 * @brief u_gpio_set_pull_up
 *
 * @param gpio 参考宏IO_PORTx_xx，如IO_PORTA_00
 * @param value 1，上拉；0，不上拉
 *
 * @return
 */
__attribute__((weak))  int u_gpio_set_pull_up(u32 gpio, int value){
    return 0;
}


/**
 * @brief gpio_set_pull_down
 *
 * @param gpio 参考宏IO_PORTx_xx，如IO_PORTA_00
 * @param value 1，下拉；0，不下拉
 *
 * @return
 */
__attribute__((weak))  int u_gpio_set_pull_down(u32 gpio, int value){
    return 0;
}

/**
 * @brief gpio_set_die
 *
 * @param gpio 参考宏IO_PORTx_xx，如IO_PORTA_00
 * @param value 1，IO普通输入；0，IO模拟输入
 *
 * @return
 */
__attribute__((weak))  int u_gpio_set_die(u32 gpio, int value){
    return 0;
}

/**
 * @brief gpio_direction_output
 *
 * @param gpio 参考宏IO_PORTx_xx，如IO_PORTA_00
 * @param value 1，输出1；0，输出0
 *
 * @return
 */
__attribute__((weak))  int u_gpio_set_dir_output(u32 gpio, int value){
    return 0;
}

/*----------------------------------------------------------------------------*/
/**@brief    变声音效指定模式切换
   @param    eff_mode 模式索引
   @return
   @note
*/
/*----------------------------------------------------------------------------*/
__attribute__((weak))  void u_mic_voicechange_switch(u8 eff_mode){

}

/**
 * @brief 人声消除
 * 
 * @param sw 
 */
__attribute__((weak))  void vocal_remove_switch(bool sw){

}

/**
 * @brief 获取SDK版本
 * 
 */
__attribute__((weak))  void* u_get_sdk_version(void){

}

