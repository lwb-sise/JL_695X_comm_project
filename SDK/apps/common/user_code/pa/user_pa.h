//*********************************************************************************//
//						            功放配置	                                    //
//*********************************************************************************//
#ifdef U_PA_MODE_EN

#define M_PA_AUTO_MUTE_DET_MAX_CNT      	80  /* 功放自动mute检测时间 单位：ms*/

#ifdef  AUDIO_OUTPUT_AUTOMUTE
#undef  AUDIO_OUTPUT_AUTOMUTE
#endif
#define AUDIO_OUTPUT_AUTOMUTE 				1   //系统自动MUTE使能

#endif
/*********************************************************************************************/


typedef  enum {
    PA_MIC,//mic 上线
    PA_MANUAL,//手动 mute key
    PA_LINEIN,//linein pp puse
    PA_SYS_MUTE,//系统自动mute
}EX_FLAG;

typedef  enum {
    PA_NULL,//没有初始化
    PA_INIT,//功放io初始化
    // PA_UNIT,//功放卸载
    PA_UPDATE,//升级 输出高低 控制umute
    PA_POWER_OFF_IDLE,//假关机 正常控制mute功放 之后不能在进行更改
    PA_POWER_OFF_SOFT,//软关机 上下拉mute功放
    PA_MUTE,//静音
    PA_UMUTE,//解除静音
    PA_CLASS_AB,//AB类 模拟
    PA_CLASS_DLAY_AB,//延时切 AB类 模拟
    PA_CLASS_D,//D类 数字
    PA_CLASS_DLAY_D,//延时切 D类 数字
}PA_STATUS;

typedef struct _pa_io_{
    IO_INFO mute;
    IO_INFO abd;
}PA_IO;

typedef struct USER_PA_CTL_IO {
    bool en;//使能
    PA_IO io;//功放 io口
    u8 pa_index;//功放型号
    PA_STATUS status_mute;//功放mute状态
    PA_STATUS status_abd;//功放abd状态
    PA_STATUS status_cur;//当前功放状态 驱动

    u32 timerin;//进入 drive api 时间

    //各个状态下gpio对应的状态
    bool umute_to_abd_en;//umute 转化为ab、d类控制
    GPIO_CTL gpio_s_ab;//ab 类
    GPIO_CTL gpio_s_d;//d 类
    GPIO_CTL gpio_s_mute;//mute
    GPIO_CTL gpio_s_umute;//umute
    GPIO_CTL gpio_s_off;//关机

    bool exit;//退出
    bool pause;//暂停 auto mute pa
    u16 id_auto;//ap auto mute timer id
    u16 id_temp;//temp timer id
    u32 timer_keep;//保存一段功放状态 维持到的目标时间

    /*
        701测试ok 695、696没有该接口
        是否在流里面进行自动控制功放的处理
        如果是 需要每次运行 audio_app_stream_data_handler 调用一次 u_pa_automute_stream_run 
    */
    bool stream_run;
    u16 automute_stream_id;//流里面automute id

    // mute状态下播放提示音 功放控制已经umute 但是功放没有立刻出声音 此时在播放提示音之前等一段时间再解码播放
    u32 wait_time;//从mute 到 umute 功放出声音的时间 单位10ms

    volatile bool mic;//mic 上线
    volatile bool linein;//linein pp puse
    volatile bool manual;//手动 mute key
    volatile bool sys;//系统自动mute
}PA_INFO;

extern PA_INFO info_pa;


/**
 * @brief 功放io初始化
 * 
 */
void u_pa_io_init(void);

/**
 * @brief 功放自动mute初始化
 * 
 */
void u_pa_automute_init(void);

/**
 * @brief 功放控制总入口
 * PA_UPDATEz
 */
void u_pa_control(PA_STATUS ctrl);

/**
 * @brief auto mute 需要的外部mute信息
 * 
 * @param obj 设置项
 * @param status 项目状态
 */
void u_pa_ex_info_set(EX_FLAG obj,bool status);

/**
 * @brief 关闭功放自动mute
 * 
 */
void u_pa_automute_off(void);

/**
 * @brief 处理dac上电噗噗声
 * 
 */
void u_comm_dac_pupu(void);