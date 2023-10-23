#include "user_head.h"


/**
 * @brief 电源维持初始化
 * 
 */
void u_maintenance_voltage_init(void){
#ifdef U_PWR_CTRL_EN
    M_PWR_IO_INIT();
    u_maintenance_voltage_on();
#endif   
}

/**
 * @brief 
 * 
 * @param control 
 */
static void u_maintenance_voltage(KEEP_CTRL control){

    if(KEEP_ON==control){
        printf("------M_PWR_IO_ON-------");
        M_PWR_IO_ON();
    }else if(KEEP_OFF==control){
        printf("------M_PWR_IO_OFF-------");
        M_PWR_IO_OFF();
    }else{
        return;
    }


}

/**
 * @brief 开启维持电压
 * 
 */
void u_maintenance_voltage_on(void){
#ifdef U_PWR_CTRL_EN
    u_maintenance_voltage(KEEP_ON);
#endif
}
/**
 * @brief 关闭维持电压
 * 
 */
void u_maintenance_voltage_off(void){   
#ifdef U_PWR_CTRL_EN
    u_maintenance_voltage(KEEP_OFF);
#endif
}

