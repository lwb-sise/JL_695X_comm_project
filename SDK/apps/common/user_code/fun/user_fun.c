#include "user_head.h"

/**
 * @brief 上电初始化
 * 
 */
void user_project_init(){

    //电源维持初始化
    u_maintenance_voltage_init();

    // 功放初始化
    u_pa_io_init();



}

/**
 * @brief 开机 功能初始化
 *
 */
void user_fun_init(void){
    
    // 处理dac上电噗噗声
    // u_comm_dac_pupu();//导致升级完成后复位，重复升级过程

    //自动mute初始化
    u_pa_automute_init();
#if AUDIO_VOCAL_REMOVE_EN
    //关闭消人声     
    user_vocal_remove_dis();
#endif
    //炫彩灯初始化
    user_rgb_init();

    //串口初始化
    u_serial_init();
#if BAT_LED_SHOW
    //外部电量指示灯中断初始化
    timer0_init();
#endif
    //ram打印
    // sys_timer_add(NULL,mem_stats,1000); 


}

/**
 * @brief 关机动作
 * 
 */
void user_fun_exit(){

    if (!light_off)
    {
        light_off=1;//关闭rgb灯
    }
    user_rgb_deal();

    u_pa_automute_off();//关闭功放

    
}