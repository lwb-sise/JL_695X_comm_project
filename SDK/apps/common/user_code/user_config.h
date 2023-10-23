//*********************************************************************************//
//						            自定义功能配置	                                    //
//						      不使能的模块直接整个注释掉即可	                                    //
//*********************************************************************************//

//配置打印
#define U_DEBUG                 1

#ifndef CONFIG_DEBUG_U
#define CONFIG_DEBUG_U(x)         (x & U_DEBUG)
#endif

#define U_PA_MODE_EN                //功放使能    4k


#define U_RGB_MODE_EN               //炫彩灯使能  4k

#define U_PWR_CTRL_EN               //电源维持使能

#define U_VBAT_MODE_EN              //外部电量检测使能

// #define U_EFFECT_MODE_EN            //K歌功能使能

// #define U_SERIAL_MODE_EN            //串口通信使能

// #define U_EFFECT_TWS_EN         //K歌对箱功能使能


