//*********************************************************************************//
//						            公用参数配置	                                    //
//*********************************************************************************//
#ifndef _U_COMM_H_
#define _U_COMM_H_

//=================================== 其他操作 ===========================================

#ifdef TCFG_AUTO_SHUT_DOWN_TIME
#undef TCFG_AUTO_SHUT_DOWN_TIME
#define TCFG_AUTO_SHUT_DOWN_TIME			(10*60)   //没有蓝牙连接自动关机时间
#endif

#define USER_NO_ACTION_AUTO_OFF             1   //是否使能无操作自动关机（有别与无连接蓝牙）
#define USER_SHOW_BAT_NOT_PHONE             0   //只显示电量，没有通话功能


//=================================== 按键开机配置 ========================================

#define TCFG_POWER_ON_NEED_KEY              1   //是否使能系统自带按键开机
#define M_SW_POWER_ON_BY_KEY_LONG_PRESS_EN  0   //是否使能自定义按键开机/*导致io维持引脚掉电*/


#define WAKE_UP_PORT                        IO_PORTA_03 //按键唤醒脚

//=================================== 提示音/音量 ========================================

#define TCFG_MAX_VOL_PROMPT					1   //播放最大提示音
#define TCFG_MIN_VOL_PROMPT					1   //播放最小提示音

//=================================== 按键功能配置 ========================================

#define USE_CALL_DOUBLE_CLICK				1	//使能限制某些按键才有双击功能
#define DOUBLE_KEY_NUM						0   //限制有双击的按键1
#define DOUBLE_KEY_NUM1						5   //限制有双击的按键2

#define USE_CALL_POWER_KEY					0	
#define USE_SCO_LINK						0	
#define USE_HANGUP_KEY_FOR_TWS_PAIR			0

#ifdef R_UP

#define VDDIO_AD                        (0x3ffL)
#define ADKEY_NO_R                      (-1)
#define ADKEY_VOLTEND_NULL              VDDIO_AD//空阻值取的ad值
#define ADKEY_R_UP                      (R_UP)//上拉电阻
#define GET_ADKEY_AD_UP(r,up)           ((r!=ADKEY_NO_R)?(VDDIO_AD * r   /(r+up)):VDDIO_AD)//按键阻值换算成ad值
#define GET_ADKEY_VOLTAGE(a,b)          ((a + b) / 2)//平均值

#define ADKEY_VOLTAGE_UP(a,b,up)        GET_ADKEY_VOLTAGE(GET_ADKEY_AD_UP(a,up),GET_ADKEY_AD_UP(b,up))//两电阻ad值的平均值 需要传上拉电阻值
#define ADKEY_VOLTAGE(a,b)              ADKEY_VOLTAGE_UP(a,b,ADKEY_R_UP)//两电阻ad值的平均值 默认ADKEY_R_UP上拉电阻值

#endif

//=================================== RGB灯光配置 =========================================

#define M_USER_RGB_FM_EN                    1//FM模式是否开RGB灯
#define M_USER_RGB_DOUBLE_SPI               0//使能两组spi推灯

//=================================== 索爱定制功能  ========================================

#define USE_SUOAI_COMM                      0 //索爱定制功能总宏
#if USE_SUOAI_COMM
#define USE_SUOAI_BAT_TONE                  1 //使用索爱定制开机提示音
#define USE_SUOAI_LED_MODE                  1 //使能索爱定制指示灯效
#define USE_SUOAI_RGB_MODE                  1 //使能索爱定制炫彩灯模式 ---低电闪灯
#define USE_SUOAI_CHANGE_MODE_LIST          1 //使能索爱定制切换模式顺序 FM永远跟在蓝牙后面
#define USE_SUOAI_VOL_SAVE                  1//使能索爱定制音量保存
#define USE_BT_UNMUTE_ALLTIME               1 //蓝牙模式不MUTE

#endif
//======================================= TWS ==============================================

#define USER_TWS_PARING_EVERY_CLICK         1 //TWS任何时候都能组队，包括当前已经组队的状态下还能发起组队
#if USER_TWS_PARING_EVERY_CLICK
#define CONFIG_TWS_USE_COMMMON_ADDR         0      /* tws 使用公共地址 */
#endif
//======================================= DEV ==============================================

#define E_DEV_SD_ONLINE						0
#define E_DEV_USB_ONLINE					1
#define E_DEV_AUX_ONLINE					2

//======================================= MUSIC===============================================

#define DEV_SD_AND_USB_MODE_CHANGE          1 //进入音乐模式切模式要在U盘和T卡之间切换之后才切到其他模式

//======================================= LINEIN =============================================

#define USE_TONE_END_LINEIN_RESTART         1   //播放完提示音之后要恢复linein播放

//======================================= FM =================================================

#define USE_FM_LINEIN_MODE_AUTO_CHANGE      0   //是否收音模式插入linein不切入linein模式
#define USE_DEV_OUT_TO_BT                   0   //是否使能退出外设切回蓝牙模式
#define USE_FM_VOL_FADE_IN                  1   //是否使能FM音量渐进

#if USE_FM_VOL_FADE_IN
#define FM_FADE_IN_MAX_VOL                  10  //FM渐进最大音量

#endif 
//======================================= BAT  ================================================

#define BAT_VAL_CNT                       10 //电量更新滤波次数
#define BAT_PHONE_VAL_CNT                 3  //手机电量刷新滤波次数 （单位：10s/次）
#define BAT_LOW_TO_POWER_OFF_TIMES        5  //低电提醒播放多少次关机
#define BAT_LOW_POWER_OFF_PLAY_TONE       1  //低电关机前播一次低电提示音再关机
#define USER_BAT_LEVEL_SHOW               1  //是否要按照自己的电压值去分配手机显示的电量百分比
/*************************************** 电量显示 *********************************************/

#define BAT_LED_SHOW                      0  //电量LED指示灯显示   //开了中断占了4K空间

#if BAT_LED_SHOW

#define BAT_LED3_ON     JL_PORTB->DIE &= ~BIT(4);JL_PORTB->DIR &= ~BIT(4);JL_PORTB->OUT |= BIT(4);\
                        JL_PORTB->PD &= ~BIT(4);JL_PORTB->PU &= ~BIT(4);

#define BAT_LED2_ON     JL_PORTB->DIE &= ~BIT(4);JL_PORTB->DIR &= ~BIT(4);JL_PORTB->OUT &= ~BIT(4);\
                        JL_PORTB->PD &= ~BIT(4);JL_PORTB->PU &= ~BIT(4);


#define BAT_LED1_ON     JL_PORTB->DIE &= ~BIT(5);JL_PORTB->DIR &= ~BIT(5);JL_PORTB->OUT |= BIT(5);\
                        JL_PORTB->PD &= ~BIT(5);JL_PORTB->PU &= ~BIT(5);

#define BAT_LED0_ON     JL_PORTB->DIE &= ~BIT(5);JL_PORTB->DIR &= ~BIT(5);JL_PORTB->OUT &= ~BIT(5);\
                        JL_PORTB->PD &= ~BIT(5);JL_PORTB->PU &= ~BIT(5);

#define BAT_LED_CLAER(x)        {JL_PORTB->DIR |= BIT(x);JL_PORTB->PD &= ~BIT(x);JL_PORTB->PU &= ~BIT(x);} 

#endif

int timer0_init();
void bat_led_clear_all_without(u8 led);
void bat_led(u8 led_status);
void bat_led_scan(void);
u8 get_bat_led_level(u16 bat_val ,u16 power_shutdown_val ,u8 old_level);
u8 get_bat_led_level_by_bl(u8 cur_battery_level , u8 old_level);
u8 get_bat_level_by_vbat(u16 bat);


//======================================= COMM ================================================

#define USE_DEV_OUT_TO_BT                   1 //是否使能退出外设切回蓝牙模式
#define DEV_ONLINE_BK_POWERON_TONE          1 //防止上电就跳入外设打断开机提示音


//======================================= EFFECT =================================================
#ifdef U_EFFECT_MODE_EN
#define SYS_USER_EVENT 			0x2000

#define TONE_RES_PATH   TONE_RES_ROOT_PATH"tone/"
#define USER_FILE_NAME_AuxMode         TONE_RES_PATH"AuxMode.*"//linein模式
#define USER_FILE_NAME_BtMode          TONE_RES_PATH"BtMode.*"//蓝牙模式
#define USER_FILE_NAME_USBmode         TONE_RES_PATH"USBmode.*"//usb模式
#define USER_FILE_NAME_CardMode        TONE_RES_PATH"CardMode.*"//T卡模式
#define USER_FILE_NAME_GuanJi          TONE_RES_PATH"GuanJi.*"//关机
#define USER_FILE_NAME_KaiJi           TONE_RES_PATH"KaiJi.*"//开机

#define USER_FILE_NAME_AccomAd         TONE_RES_PATH"AccomAd.*"//伴奏调节
#define USER_FILE_NAME_ReverbAd        TONE_RES_PATH"ReverbAd.*"//混响调节
#define USER_FILE_NAME_MicAdjust       TONE_RES_PATH"MicAdjust.*"//话筒调节

#define USER_FILE_NAME_XRSG            TONE_RES_PATH"XRSG.*"//消人声关
#define USER_FILE_NAME_XRSK            TONE_RES_PATH"XRSK.*"//消人声开
#define USER_FILE_NAME_MicClose        TONE_RES_PATH"MicClose.*"//mic mute
#define USER_FILE_NAME_MicOpen         TONE_RES_PATH"MicOpen.*"//mic umute


#define USER_FILE_NAME_Mic1_Connect    TONE_RES_PATH"Mic1Con.*"//mic1 connect
#define USER_FILE_NAME_Mic1_Disconnect TONE_RES_PATH"Mic1Dis.*"//mic1 dsconnect

#define USER_FILE_NAME_Mic2_Connect    TONE_RES_PATH"Mic2Con.*"//mic2 connect
#define USER_FILE_NAME_Mic2_Disconnect TONE_RES_PATH"Mic2Dis.*"//mic2 dsconnect

//音效模式
#define USER_FILE_NAME_KTV             TONE_RES_PATH"KTV.*"//KTV
#define USER_FILE_NAME_MoYin           TONE_RES_PATH"MoYin.*"//魔音
#define USER_FILE_NAME_NanBianNv       TONE_RES_PATH"NanBianNv.*"//男变女
#define USER_FILE_NAME_NvBianNan       TONE_RES_PATH"NvBianNan.*"//女变男
#define USER_FILE_NAME_WaWaYin         TONE_RES_PATH"WaWaYin.*"//萝莉音
#define USER_FILE_NAME_ZhuanYe         TONE_RES_PATH"ZhuanYe.*"//专业模式

#define USER_FILE_NAME_Connect         TONE_RES_PATH"Connect.*"//蓝牙连接
#define USER_FILE_NAME_Disconnect      TONE_RES_PATH"Disconnect.*"//蓝牙断开连接
#define USER_FILE_NAME_LowPower        TONE_RES_PATH"LowPower.*"//低电

#define USER_FILE_NAME_DU              TONE_RES_PATH"Du.*"
#define USER_FILE_NAME_DUDU            TONE_RES_PATH"DuDu.*"

typedef enum {
    U_CMD_DOW,//-
    U_CMD_UP,//+

    U_CMD_SET,//设置
    U_CMD_GET,//获取

    U_CMD_ADD,//增加
    U_CMD_DEL,//去除
}U_CMD;

enum {
    VOICE_CHANGE_ECHO=VOICE_CHANGER_MAX+1,//echo
    VOICE_CHANGE_REVERB,//reverb
};

typedef enum {
    VOCAL_REMOVE_HDL_BT,
    VOCAL_REMOVE_HDL_LINEIN,
    VOCAL_REMOVE_HDL_MUSIC,
    VOCAL_REMOVE_HDL_FM,
    VOCAL_REMOVE_HDL_PC,

    VOCAL_REMOVE_HDL_MAX,
}VOCAL_HDL;

enum {
    USER_MSG_START=KEY_USER_START,//在KEY_NULL上一行添加 KEY_USER_START,//不要在其下方插入任何消息

    USER_KARAOKE_CHANGER_NONE,//原声
    USER_AUDIO_TONE_CTRL,//魔音
    USER_MIC_ONLINE_OR_OFFLINE,//mic 上下线消息处理

    USER_AUDIO_VOCAL_REMOVE_SWITCH,//消原音切换
    USER_AUDIO_VOCAL_REMOVE_ON,//消原音 开
    USER_AUDIO_VOCAL_REMOVE_OFF,//消原音 关

    // USER_KARAOKE_EARPHONE_KTV,      //混响
    USER_KARAOKE_EARPHONE_ECHO,//指定回声
    USER_KARAOKE_EARPHONE_REVERB,//指定混响

	//金和成key消息
    KEY_ACCOMPANIMENT_UP,//伴奏加
    KEY_ACCOMPANIMENT_DOWN,//伴奏减

    KEY_PLAY_TONE_MIC1_CONNECT,//MIC1 连接
    KEY_PLAY_TONE_MIC1_DSCONNECT,//MIC1 断开
    KEY_PLAY_TONE_MIC2_CONNECT,//MIC2 连接
    KEY_PLAY_TONE_MIC2_DSCONNECT,//MIC2 断开

    KEY_PLAY_TONE_MIC_VOL_MAX,//播放“话筒音量已最大”
    KEY_PLAY_TONE_MIC_VOL_MIN,//播放“话筒音量已最小”提示音
    KEY_PLAY_TONE_MIC_OPEN,//播放“话筒开”提示音
    KEY_PLAY_TONE_MIC_CLOSE,//播放“话筒关”提示音
    KEY_REVERB_CHANGE_MODE,//音效切换(切换喊话效果 eg:男变女 女变男 等)
    KEY_PLAY_TONE_MIC_ADJUST,//播放"话筒调节"提示音
    KEY_PLAY_TONE_REVERB_ADJUST,//播放"混响调节"提示音
    KEY_PLAY_TONE_ACCOMPANIMENT_ADJUST,//播放"伴奏调节"提示音
    
    KEY_MIC_VOL_MUTE,//话筒MUTE
    KEY_MIC_VOL_ON,//话筒MUTE 开
    KEY_MIC_VOL_OFF,//话筒MUTE 关


    KEY_MIC_VOL_UP,//话筒加(预留，暂时没用到)
    KEY_MIC_VOL_DOWN,//话筒减(预留，暂时没用到)
    KEY_REVERB_UP,//混响加
    KEY_REVERB_DOWN,//混响减
    KEY_MAGIC,//魔音键
    KEY_DODGE_EN,//闪避开关

    USER_LIGHT_LED_SWITCH,//LED模式切换
    USER_LIGHT_LED_ON,//LED 开
    USER_LIGHT_LED_OFF,//LED 关

    USER_LIGHT_RGB_SWITCH,//RGB模式切换
    USER_LIGHT_RGB_OFF_ON,//RGB开关
    USER_LIGHT_RGB_SET,//RGB模式设置
    USER_LIGHT_RGB_KEEP,//RGB 维持模式

    USER_EQ_SWITCH,//EQ切换
    USER_EQ_SET,//EQ设置
    USER_TWS_MIC_STATUS,//TWS mic 状态同步

    KEY_PA_MUTE_UMUTE_SWITCH,//功放umute 、mute切换看是否有噗噗声
    USER_TEST,
};

typedef struct _USER_LEVEL_VOL_U8_{
    int value;//当前值
    u8 max;//最大等级
    u8 min;//最小等级
}level_step;

typedef struct _USER_LEVEL_VOL_INT_{
    // bool step;//步进
    int level;//等级
    int backup;//等级备份
    int lmax;//最大等级
    int lmin;//最小等级
    int mlevel;//映射等级
    int mlmax;//映射最大等级
    int mlmin;//映射最小等级
}level_map;
#endif
//======================================= GPIO =================================================
typedef enum {
    USER_GPIO_HI=1,//高阻态 High impedance
    USER_GPIO_OUT_H,//输出高 io实现
    USER_GPIO_OUT_L,//输出低 io实现
    USER_GPIO_IN_IO,//DIE 设为1 检测高低电平
    USER_GPIO_IN_AD,//DIE 设为0 检测ad值
    USER_GPIO_IN_IO_PU,//上拉 IO 输入
    USER_GPIO_IN_IO_PD,//下拉 IO 输入
    USER_GPIO_IN_AD_PU,//上拉 AD 输入
    USER_GPIO_IN_AD_PD,//下拉 AD 输入
    USER_GPIO_SET_MAX,
}GPIO_CTL;

typedef struct _io_info_{
    u32 pro;//io口
    u32 mult;//复用口
}IO_INFO;

#define IO_CHECK_MODE_AD    (1)//AD检测
#define IO_CHECK_MODE_IO    (0)//IO检测
#define IO_CHECK_UP         (1)//上拉检测
#define IO_CHECK_DOW        (-1)//下拉检测
#define IO_CHECK_ON_LINE_H  (1)//高电平在线
#define IO_CHECK_ON_LINE_L  (0)//低电平在线

/**
 * @brief 寄存器方式设置gpio状态
 *
 * @param gpio io口
 * @param ctl 需要设置的状态
 * @param delay_cnt 设置io状态之后延时时间 在设置为输入立马读取ad值时需要用到
 */
// AT(.amr_code)
void u_comm_gpio_set(u32 gpio,GPIO_CTL ctl,int delay_cnt);
//======================================= OTHER ==================================================
extern u8 flag_power_on_key;
extern u8 flag_power_key_press;
extern u8 flag_vol_hold_press;
extern u8 flag_music_can_exit;
extern u8 status_power_on;
extern u8 flag_Dev_online;



/**
 * @brief 无操作自动关机
 * 
 */
void bt_check_power_off_deal(void);

int u_app_common_key_msg_deal(struct sys_event *event);
void low_power_off_deal();

#endif//_U_COMMON_H_