#ifndef __U_SERIAL_CODE_AMW_H__
#define __U_SERIAL_CODE_AMW_H__

/**
 * @brief 设备消息处理
 * 
 * @param buf 
 */
void u_serial_code_amw_dev(u8 *buff,u16 len);
/**
 * @brief 按键事件处理
 * 
 * @param buf 
 */
void u_serial_code_amw_key(u8 *buff,u16 len);
/**
 * @brief 自定义事件处理
 * 
 * @param buf 
 */
void u_serial_code_amw_user(u8 *buff,u16 len);

#endif