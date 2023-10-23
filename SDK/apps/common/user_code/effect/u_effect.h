// #include "u_config.h"
#ifndef _U_EFFECT_
#define _U_EFFECT_

#ifdef U_EFFECT_MODE_EN

#if ((defined CONFIG_CPU_BR25)||(defined CONFIG_CPU_BR23))
#define audio_dig_vol_group_vol_set(a,b,c,d)    audio_dig_vol_set(audio_dig_vol_group_hdl_get(a, b),c, d);
#endif
#define U_EFFECT_OPEN_TIME_GET 0
#define U_EFFECT_OPEN_TIME_SET 1

#define U_RUN_ERROR (-1)
#define U_RUN_SUCCE (0)
#define MIC_MIC_DIG "mic_mic"

//mic 检测类型
#define U_MIC_CHECK_IO      1//IO口检测
#define U_MIC_CHECK_SERIAL  2//串口检测 需要开串口模块

#define U_MIC_CHECK_CLASS   U_MIC_CHECK_SERIAL//U_MIC_CHECK_SERIAL

extern struct eff_parm  eff_mode[EFFECT_REVERB_PARM_MAX];//混响的8个模式

typedef enum {
    U_EFFECT_MIC_VOL,//MIC VOL
    U_EFFECT_ECHO,//ECHO
    U_EFFECT_REVERB,//REVERB
    U_EFFECT_ACCOMPANIMENT,//ACCOMPANIMENT
    U_EFFECT_MAX,//ACCOMPANIMENT
}EFFECT_CLASS;

typedef struct _USER_EFFECT_MODE_{
    u8 mode;//模式
    // bool en;//变音使能
    u8 *path;//提示音路径
}u_ef_m;
typedef struct USER_EFFECT_INFO{
    bool en;//使能
    bool front_6dB_en;//mic 前置6dB
    bool mute;//mic mute
    bool reset;//开混响是否重置数据
    u32 open_time;//打开混响的时间 用于清除刚开混响的前一段时间数据
    u8 mic_gain_level;//mic 后级增益等级
    // level_step step[U_EFFECT_MAX];
    level_map level[U_EFFECT_MAX];
    // u8 mic_vol;//mic vol
    // u8 mic_vol_max;//mic vol max
    // u8 echo;//回音延时
    // u8 echo_max;//回音延时最大值
    // u8 reverb;//混响深度
    // u8 reverb_max;//混响深度最大值
    // u8 accompaniment;//伴奏
    // u8 accompaniment_max;//伴奏最大值


    bool tool_mode_en;//外部工具配置模式切换使能
    u_ef_m  mode_table[20];//模式列表
    u8 mode_cn;//当前模式再table中的位置
    #if (U_MIC_CHECK_CLASS==U_MIC_CHECK_SERIAL)
    u8 on_line;
    u16 kid;//维持id
    #elif (U_MIC_CHECK_CLASS==U_MIC_CHECK_IO)
    IO_CHECK check_io[2];//检测io口
    #endif
}EFFECT_INFO;

void user_vocal_remove_dis(void);//关闭消人声

void user_vocal_remove_on(void);//打开消人声


void mic_reverb_bypass(bool bypass);


void mic_echo_bypass(bool bypass);

/**
 * @brief 
 * 
 * @param code U_EFFECT_OPEN_TIME_GET U_EFFECT_OPEN_TIME_SET
 * @return u32 OPEN TIMER
 */
u32 u_effect_open(u8 code);

/**
 * @brief mic check pause
 * 
 * @param ktimer 暂停时长
 */
void u_effect_mic_check_pause(u32 ktimer);

/**
 * @brief 获取 effect 调节的leve
 * 
 * @return int 
 */
int u_effect_level_get(EFFECT_CLASS mode);


/**
 * @brief 前置mic 6dB 是能 配置
 * 
 * @param en 默认0dB 使能配置
 */
void u_front_mic_6dB_info(bool en);

/**
 * @brief 跟据 mic 状态 开启/停止 混响
 * 
 */
void u_eff_mic_effect_start_or_stop(void);

/**
 * @brief 获取 mic 在线状态
 * 
 * @return int 0：不在线 -1：错误  1：mic1 在线 2: mic2 在线 3：mic1 mic2在线
 */
int u_effect_mic_onlin_get(void);


/**
 * @brief 设置 mic状态
 * 
 * @param on_line BIT(DEV_EVE_ONLIEN_MIC_1):MIC1 BIT(DEV_EVE_ONLIEN_MIC_2): MIC2
 */
void u_effect_mic_onlin_set(DEV_EVE dev,u8 on_line);

/**
 * @brief get mic gain level
 * 
 * 没有外部配置返回传入的默认值
 * 
 * @param in_level 传入默认值
 * @return int 
 * error: in_level
 * level:0(-8dB)~19(30dB)
 */

int u_mic_gain_level(u16 in_level);



/**
 * @brief effect mode switch
 * 
 * @param mode mode
 * @param tone_play tone play flag 
 */
void u_effect_mode_switch(u16 mode,bool tone_play);

/**
 * @brief 
 * 
 */
void u_effect_mode_none(void);


/**
 * @brief mic key io init
 * 
 */
void u_effect_mic_key_io_init(void);

/**
 * @brief mic check init
 * 
 */
void u_effect_mic_check_init(void);

/**
 * @brief 循环切换混响模式
 * 
 */
void u_effect_loop_mode_set(void);

// VOCAL_REMOVE
void u_effect_vocal_remove_eq(bool switch_flag);

int u_effect_mic_vol_get(void);

/**
 * @brief mic vol up/down
 * 
 * @param cmd U_CMD_UP/U_CMD_DOW
 */
void u_effect_mic_vol(bool cmd);

/**
 * @brief mic reverb up/down
 * 
 * @param cmd U_CMD_UP/U_CMD_DOW
 */
void u_effect_reverb(bool cmd);

/**
 * @brief mic accompaniment up/down
 * 
 * @param cmd U_CMD_UP/U_CMD_DOW
 */
void u_effect_accompaniment(bool cmd);



/**
 * @brief mic 状态变化 回调
 * 
 * @param priv 
 */
void u_effect_mic_state_change(void *priv);



/**
 * @brief 获取mic mute状态
 * 
 * @return true mute
 * @return false umute
 */
bool u_effect_mic_mute_get(void);

/**
 * @brief 设置mic mute
 * 
 */
void u_effect_mic_mute(void);

/**
 * @brief 设置mic umute
 * 
 */
void u_effect_mic_umute(void);


/**
 * @brief 设置 effect 调节的参数
 * 
 * @param mode 设置类型
 * @param value 设置参数
 * @return int 成功：0 其他：失败
 */
int u_effect_value_set(EFFECT_CLASS mode,int value);

/**
 * @brief 重载数据
 * 
 */
void u_effect_load_cfg(void);
#endif
#endif