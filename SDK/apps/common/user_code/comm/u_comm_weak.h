#ifndef _U_COMM_WEAK_H_
#define _U_COMM_WEAK_H_

/*
*@brief project init
*
*/
void u_project_init(void);

/*
*@brief project eixt
*
*/
void u_project_exit(void);

/**
 * @brief 获取gpio对应的ad通道
 * 
 * @param pr io口
 * @return int -1：错误 其他正确
 */
int u_comm_get_gpio_ad_ch(u32 pr);

/**
 * @brief 获取gpio 属于哪一组
 * 
 * @param gpio 返回当前组的PN00
 * @return int error:-1
 */
int u_comm_gpio_pro_get(u32 gpio);



/**
 * @brief 处理dac上电噗噗声
 * 
 */
void u_comm_dac_pupu(void);

/**
 * @brief u_gpio_set_dir_input
 *
 * @param gpio 参考宏IO_PORTx_xx，如IO_PORTA_00
 * @param dir 1，输入；0，输出
 *
 * @return
 */
int u_gpio_set_dir_input(u32 gpio, u32 dir);

/**
 * @brief u_gpio_set_pull_up
 *
 * @param gpio 参考宏IO_PORTx_xx，如IO_PORTA_00
 * @param value 1，上拉；0，不上拉
 *
 * @return
 */
int u_gpio_set_pull_up(u32 gpio, int value);


/**
 * @brief u_gpio_set_pull_down
 *
 * @param gpio 参考宏IO_PORTx_xx，如IO_PORTA_00
 * @param value 1，下拉；0，不下拉
 *
 * @return
 */
int u_gpio_set_pull_down(u32 gpio, int value);

/**
 * @brief u_gpio_set_die
 *
 * @param gpio 参考宏IO_PORTx_xx，如IO_PORTA_00
 * @param value 1，IO普通输入；0，IO模拟输入
 *
 * @return
 */
int u_gpio_set_die(u32 gpio, int value);

/**
 * @brief u_gpio_set_dir_output
 *
 * @param gpio 参考宏IO_PORTx_xx，如IO_PORTA_00
 * @param value 1，输出1；0，输出0
 *
 * @return
 */
int u_gpio_set_dir_output(u32 gpio, int value);

/*----------------------------------------------------------------------------*/
/**@brief    变声音效指定模式切换
   @param    eff_mode 模式索引
   @return
   @note
*/
/*----------------------------------------------------------------------------*/
void u_mic_voicechange_switch(u8 eff_mode);

/**
 * @brief 人声消除
 * 
 * @param sw 
 */
void vocal_remove_switch(bool sw);

/**
 * @brief 获取SDK版本
 * 
 */
void* u_get_sdk_version(void);

#endif//_U_COMM_WEAK_H_
