#ifndef _U_MAINTENANCE_VOLTAGE_H_
#define _U_MAINTENANCE_VOLTAGE_H_

//*********************************************************************************//
//						            电源维持控制配置	                                    //
//*********************************************************************************//


#define M_PWR_IO_PORT     					JL_PORTA
#define M_PWR_IO_BIT      					BIT(3)
#define M_PWR_IO_INIT()   					do{M_PWR_IO_PORT->DIR &= ~M_PWR_IO_BIT;\
                             					M_PWR_IO_PORT->HD0 |= M_PWR_IO_BIT;\
                             					M_PWR_IO_PORT->PU &= ~M_PWR_IO_BIT;\
                             					M_PWR_IO_PORT->PD &= ~M_PWR_IO_BIT;\
                          					}while(0)

#define M_PWR_IO_ON()     					do{M_PWR_IO_PORT->OUT |= M_PWR_IO_BIT;}while(0)
#define M_PWR_IO_OFF()     					do{M_PWR_IO_PORT->OUT &= ~M_PWR_IO_BIT;}while(0)



typedef  enum {
    KEEP_ON,
    KEEP_OFF,
}KEEP_CTRL;

/**
 * @brief 电源维持初始化
 * 
 */
void u_maintenance_voltage_init(void);

/**
 * @brief 开启维持电压
 * 
 */
void u_maintenance_voltage_on(void);
/**
 * @brief 关闭维持电压
 * 
 */
void u_maintenance_voltage_off(void);


#endif//_U_MAINTENANCE_VOLTAGE_H_