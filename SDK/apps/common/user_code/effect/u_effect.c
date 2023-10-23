#include "user_head.h"


#define LOG_TAG_CONST        U_EFFECT
#define LOG_TAG             "[U_EFFECT]"
#define LOG_ERROR_ENABLE
#define LOG_DEBUG_ENABLE
#define LOG_INFO_ENABLE
#define LOG_DUMP_ENABLE
#define LOG_CLI_ENABLE
#include "debug/user_debug.h"

#ifdef U_EFFECT_MODE_EN

EFFECT_INFO info_effect={//混响
    .en=true,
    .reset=true,
    .front_6dB_en=0,
    .mic_gain_level=8,
    //EFFECT_REVERB_PARM_MAX,//VOICE_CHANGER_MAX,
    .mode_cn=VOICE_EFFECT_REVERB_PARM_MAX,

    .level={//加减等级
        [U_EFFECT_MIC_VOL]={.level=10,.backup=10,.lmax=10,.lmin=0,},
        [U_EFFECT_ECHO]={.level=10,.backup=10,.lmax=10,.lmin=0,},
        [U_EFFECT_REVERB]={.level=10,.backup=10,.lmax=10,.lmin=0,},
    },

    //io 检测mic
    #if (U_MIC_CHECK_CLASS==U_MIC_CHECK_SERIAL)
    .on_line=false,
    #elif (U_MIC_CHECK_CLASS==U_MIC_CHECK_IO)
    .check_io={
        [0]={
        .en = true,
        .name="mic 1",
        .io.pro=IO_PORTA_07,
        .io.mult=NO_CONFIG_PORT,
        .mode=IO_CHECK_MODE_IO,
        .up_dow=IO_CHECK_UP,
        .threshold=IO_CHECK_ON_LINE_L,
        .filter_n=3,
        .time=60,
        },
        [1]={
        .en = true,
        .name="mic 2",
        .io.pro=IO_PORTA_08,
        .io.mult=NO_CONFIG_PORT,
        .mode=IO_CHECK_MODE_IO,
        .up_dow=IO_CHECK_UP,
        .threshold=IO_CHECK_ON_LINE_L,
        .filter_n=3,
        .time=70,
        },
    },
    #endif

    #if 1
    .tool_mode_en=1,
    .mode_cn=0,
    .mode_table={
    {VOICE_EFFECT_REVERB_PARM_3,USER_FILE_NAME_ZhuanYe},
    {VOICE_EFFECT_REVERB_PARM_2,USER_FILE_NAME_KTV},
    {VOICE_EFFECT_REVERB_PARM_4,USER_FILE_NAME_NanBianNv},
    {VOICE_EFFECT_REVERB_PARM_5,USER_FILE_NAME_NvBianNan},
    {VOICE_EFFECT_REVERB_PARM_6,USER_FILE_NAME_WaWaYin},
    {VOICE_EFFECT_REVERB_PARM_7,USER_FILE_NAME_MoYin},
    },
    #else
    .mode_cn=VOICE_CHANGER_MODE_MAX,
    .mode_table={
    {VOICE_CHANGER_MODE_NONE},
    {VOICE_CHANGER_MODE_UNCLE},
    {VOICE_CHANGER_MODE_GODDESS},
    {VOICE_CHANGER_MODE_BABY},
    {VOICE_CHANGER_MODE_MAGIC},
    {VOICE_CHANGER_MODE_MONSTER},
    {VOICE_CHANGER_MODE_DONALD_DUCK},
    {VOICE_CHANGER_MODE_MINIONS},
    {VOICE_CHANGER_MODE_ROBOT},
    {VOICE_CHANGER_MODE_WHISPER},
    {VOICE_CHANGER_MODE_MELODY},
    {VOICE_CHANGER_MODE_FEEDBACK},
    },
    #endif
};

#ifdef U_EFFECT_MODE_EN
static EFFECT_INFO *info=&info_effect;
#endif

#if (defined(CONFIG_CPU_BR23)||defined(CONFIG_CPU_BR25))
#define u_mic_0dB_en    audio_mic_0dB_en
#elif (defined(CONFIG_CPU_BR28))
#define u_mic_0dB_en    audio_adc_mic_0dB_en
#endif

#ifdef U_EFFECT_MODE_EN
/**
 * @brief 
 * 
 * @param code U_EFFECT_OPEN_TIME_GET U_EFFECT_OPEN_TIME_SET
 * @return u32 OPEN TIMER
 */
u32 u_effect_open(u8 code){
    u32 ret=0;
#ifdef U_EFFECT_MODE_EN
    if(!(info && info->en)){
        return ret;
    }

    if(U_EFFECT_OPEN_TIME_SET==code){
        ret=info->open_time=sys_timer_get_ms();
    }
#endif
    return ret;
}

/**
 * @brief 前置mic 6dB 使能 配置
 * 
 * @param en 默认0dB 使能配置
 */
void u_front_mic_6dB_info(bool en){
    
    u_effect_open(U_EFFECT_OPEN_TIME_SET);

#ifdef U_EFFECT_MODE_EN
    if(!info || !info_effect.en)return;
#if 0 
#if TCFG_AUDIO_ADC_MIC_CHA & AUDIO_ADC_MIC_0
            audio_adc_mic_0dB_en(!info_effect.front_6dB_en);//关闭mic前置6dB增益，默认打开
#endif
#if TCFG_AUDIO_ADC_MIC_CHA & AUDIO_ADC_MIC_1
            audio_adc_mic1_0dB_en(!info_effect.front_6dB_en);//关闭mic前置6dB增益，默认打开
#endif
#if TCFG_AUDIO_ADC_MIC_CHA & AUDIO_ADC_MIC_2
            audio_adc_mic2_0dB_en(!info_effect.front_6dB_en);//关闭mic前置6dB增益，默认打开
#endif
#if TCFG_AUDIO_ADC_MIC_CHA & AUDIO_ADC_MIC_3
            audio_adc_mic3_0dB_en(!info_effect.front_6dB_en);//关闭mic前置6dB增益，默认打开
#endif
#endif

    if(!info_effect.en){
        return u_mic_0dB_en(!en);
    }else{
        u_mic_0dB_en(!info_effect.front_6dB_en);
    }
#else
    u_mic_0dB_en(!en);
#endif
}

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

int u_mic_gain_level(u16 in_level){
#ifdef U_EFFECT_MODE_EN
    if(!info_effect.en || 
    info_effect.mic_gain_level>19){
        return in_level;
    }
    return info_effect.mic_gain_level;
#else
    return in_level;
#endif
}


/**
 * @brief 获取mic mute状态
 * 
 * @return true mute
 * @return false umute
 */
bool u_effect_mic_mute_get(void){
    if(!info || !info->en ) return false;
    return info->mute;
}

/**
 * @brief 设置mic mute
 * 
 */
void u_effect_mic_mute(void){
    if(!info || !info->en ) return;
    info->mute=true;
}

/**
 * @brief 设置mic umute
 * 
 */
void u_effect_mic_umute(void){
    if(!info || !info->en ) return;
    info->mute=false;
}

/**
 * @brief get loop effect mode
 * 
 * @return int 
 * error:-1
 * other
 */
static int u_effect_loop_cnt(void){
#ifdef U_EFFECT_MODE_EN
    if(!info->en){
        return -1;
    }

    // info_effect.mode=mic_effect_get_cur_mode();

    log_debug("effect.cn=%d",info_effect.mode_cn);
    int mode_max;
    if(info_effect.tool_mode_en){
        mode_max=VOICE_EFFECT_REVERB_PARM_MAX;
    }else{
        mode_max=VOICE_CHANGER_MODE_MAX;
    }

    for(int i=1;i<=mode_max;i++){
        if(info_effect.mode_table[(i+info_effect.mode_cn)%mode_max].mode){
            info_effect.mode_cn=(i+info_effect.mode_cn)%mode_max;
            break;
        }
    }
    
    log_debug("effect.cn ---- =%d",info_effect.mode_cn);
    return info_effect.mode_cn;
#else
    return 0;
#endif
}


/**
 * @brief effect mode switch
 * 
 * @param mode mode
 * @param tone_play tone play flag 
 */
void u_effect_mode_switch(u16 mode,bool tone_play){
#ifdef U_EFFECT_MODE_EN
    log_debug("effect mode size=%d",MY_SIZE(info->mode_table));
    if(!info || !info->en || mode>=MY_SIZE(info->mode_table)){
        return;
    }

    info->mode_cn=mode;
    mode=info->mode_table[mode].mode-1;
    if(info_effect.tool_mode_en){
        mic_effect_change_mode(mode);//外部工具配置效果切换
    }else{
        mic_voicechange_switch(mode);//程序固定效果切换 VOICE_CHANGER_MODE_MAX
    }

    #if (defined(TCFG_MIC_VOICE_CHANGER_ENABLE) && TCFG_MIC_VOICE_CHANGER_ENABLE)
    int bypass=eff_mode[mode].voicechanger_parm.is_bypass;
    log_debug("voic bypass=%d\r\n",bypass);
    // audio_voice_changer_bypass(mode,bypass);
    #endif

    if(tone_play && info->mode_table[info->mode_cn].path){
        log_debug("path=%s",info->mode_table[info->mode_cn].path);
        tone_play_by_path(info->mode_table[info->mode_cn].path, 1);
    }

    #endif
}

/**
 * @brief 循环切换混响模式
 * 
 */
void u_effect_loop_mode(void){
#ifdef U_EFFECT_MODE_EN

    if(!info_effect.en){
        return ;
    }

    u_effect_loop_cnt();

    u_effect_mode_switch(info->mode_cn,1);

    log_debug("mic effect mode=%d",info->mode_cn);
#endif
}

/**
 * @brief 原声
 * 
 */
void u_effect_mode_none(void){
    #if (defined(TCFG_MIC_EFFECT_ENABLE) && TCFG_MIC_EFFECT_ENABLE)
    r_printf("u_effect_mode_none");
    // log_debug("u_effect_mode_ktw");
    mic_voicechange_switch(VOICE_CHANGER_NONE);
    #endif
}

/**
 * @brief 获取 mic 在线状态
 * 
 * @return int 0：不在线 -1：错误  1：mic1 在线 2: mic2 在线 3：mic1 mic2在线
 */
int u_effect_mic_onlin_get(void){
    int ret=-1;
#ifdef U_EFFECT_MODE_EN
    if(!info_effect.en){
        return ret;
    }

    #if (U_MIC_CHECK_CLASS==U_MIC_CHECK_SERIAL)
    return info->on_line;
    #elif (U_MIC_CHECK_CLASS==U_MIC_CHECK_IO)    
    ret=u_effect_mic_io_onlin_get(info);
    #endif
#endif
    return ret;
}

/**
 * @brief 设置 mic状态
 * 
 * @param on_line BIT(DEV_EVE_ONLIEN_MIC_1):MIC1 BIT(DEV_EVE_ONLIEN_MIC_2): MIC2
 */
void u_effect_mic_onlin_set(DEV_EVE dev,u8 on_line){
#ifdef U_EFFECT_MODE_EN

    #if (U_MIC_CHECK_CLASS==U_MIC_CHECK_SERIAL)
    if(timer_get_sec()<7)return;

    if(info->on_line!=on_line){
    
        if((on_line&BIT(DEV_EVE_ONLIEN_MIC_1))!=(info->on_line&BIT(DEV_EVE_ONLIEN_MIC_1))){
            log_debug("-mic1-%s--",(on_line&BIT(DEV_EVE_ONLIEN_MIC_1))?"mic1 online":"mic1 off");
            app_task_put_key_msg((on_line&BIT(DEV_EVE_ONLIEN_MIC_1))?KEY_PLAY_TONE_MIC1_CONNECT:KEY_PLAY_TONE_MIC1_DSCONNECT, 0);      
        }

        if((on_line&BIT(DEV_EVE_ONLIEN_MIC_2))!=(info->on_line&BIT(DEV_EVE_ONLIEN_MIC_2))){
            log_debug("-mic2-%s--",(on_line&BIT(DEV_EVE_ONLIEN_MIC_2))?"mic2 online":"mic2 off");
            app_task_put_key_msg((on_line&BIT(DEV_EVE_ONLIEN_MIC_2))?KEY_PLAY_TONE_MIC2_CONNECT:KEY_PLAY_TONE_MIC2_DSCONNECT, 0);
        }

        info->on_line=on_line;
        //串口任务、中断 调用在开混响 堆栈 内存不足死机
        // u_effect_mic_state_change(NULL);
        // sys_timeout_add(NULL,u_effect_mic_state_change,100);
        app_task_put_key_msg(USER_MIC_ONLINE_OR_OFFLINE, 0);
        if(!info->on_line){           
            app_task_put_key_msg(USER_AUDIO_VOCAL_REMOVE_OFF, 0);
        }
    }
    #endif

#endif
}

/**
 * @brief mic check pause
 * 
 * @param ktimer 暂停时长
 */
void u_effect_mic_check_pause(u32 ktimer){
#ifdef U_EFFECT_MODE_EN
    if(!info) return;

    if(mic_effect_get_status())mic_effect_stop();

    #if (U_MIC_CHECK_CLASS==U_MIC_CHECK_SERIAL)    
    u_effect_mic_check_serial_pause(ktimer,info);
    #elif (U_MIC_CHECK_CLASS==U_MIC_CHECK_IO)
    u_effect_mic_check_io_pause(ktimer,info);
    #endif

#endif
}



/**
 * @brief 跟据 mic 状态 开启/停止 混响
 * 
 */
void u_eff_mic_effect_start_or_stop(void){
#ifdef U_EFFECT_MODE_EN
#if (defined(TCFG_MIC_EFFECT_ENABLE)&&TCFG_MIC_EFFECT_ENABLE && \
(TCFG_MIC_EFFECT_SEL != MIC_EFFECT_MEGAPHONE)  )

    if(u_effect_mic_onlin_get()>0){
        log_debug("mic on line");
        if(tone_get_status()){
            u_effect_mic_check_pause(1000);
        }else  if(!mic_effect_get_status()){
            mic_effect_start();
            u_effect_load_cfg();
            u_pa_ex_info_set(PA_MIC,1);
        }
        
        u_effect_mic_umute();
    }else if(mic_effect_get_status()){
        log_debug("mic off line");
        u_pa_ex_info_set(PA_MIC,0);
        mic_effect_stop();
    }

#endif
#endif
}

/**
 * @brief mic 状态变化 回调
 * 
 * @param priv 
 */
void u_effect_mic_state_change(void *priv){
#ifdef U_EFFECT_MODE_EN
    if(!info_effect.en && (app_get_curr_task()>=APP_BT_TASK && app_get_curr_task()<=APP_LINEIN_TASK)){
        return;
    }
    #if (U_MIC_CHECK_CLASS==U_MIC_CHECK_IO)
    IO_CHECK *check_io = priv;

    log_debug("%s check io online=[%d]",check_io->name,u_effect_mic_onlin_get());
    #endif
    u_eff_mic_effect_start_or_stop();
#endif
}


/**
 * @brief mic check init
 * 
 */
void u_effect_mic_check_init(void){
#ifdef U_EFFECT_MODE_EN

    if(!info_effect.en){
        return;
    }

    #if (U_MIC_CHECK_CLASS==U_MIC_CHECK_SERIAL)
    info->on_line=0;
    #elif (U_MIC_CHECK_CLASS==U_MIC_CHECK_IO)
    if(info_effect.check_io[0].en)u_effect_mic_check_io_init(&info_effect.check_io[0],u_effect_mic_state_change);
    if(info_effect.check_io[1].en)u_effect_mic_check_io_init(&info_effect.check_io[1],u_effect_mic_state_change);
    #endif

    log_debug("mic check initok");
    
    #endif
}



/**
 * @brief mic check off
 * 
 */
void u_effect_mic_check_off(void){
#ifdef U_EFFECT_MODE_EN

    if(!info_effect.en){
        return;
    }

    #if (U_MIC_CHECK_CLASS==U_MIC_CHECK_SERIAL)

    #elif (U_MIC_CHECK_CLASS==U_MIC_CHECK_IO)
    u_effect_mic_check_io_off(&info_effect.check_io[0]);
    u_effect_mic_check_io_off(&info_effect.check_io[1]);
    #endif

    info_effect.en=false;
    
    #endif
}


void u_effect_p_eq(struct eq_seg_info *eq_addr,u8 number){
    int gain,q;
    for(int i=0;i<number;i++){
        gain=eq_addr[i].gain*100;        
        q=eq_addr[i].q;

        log_debug("--i=%d--index=%d freq=%d  gain=%d type=%d q=%d \r\n",i,
        eq_addr[i].index,
        eq_addr[i].freq,
        gain,
        eq_addr[i].iir_type,
        q
        );
    }
}

/**
 * @brief 人声消除
 * 
 * @param switch_flag 1：消人声开  0：消人声关
 */
void u_effect_vocal_remove_eq(bool switch_flag){
    extern const struct eq_seg_info *eq_type_tab[EQ_MODE_MAX+1];
    if(switch_flag){
        eq_mode_set(EQ_MODE_NORMAL);
        //u_effect_p_eq((struct eq_seg_info *)eq_type_tab[EQ_MODE_CUSTOM+1],10);
    }else{
        eq_mode_set(EQ_MODE_CUSTOM);
        //u_effect_p_eq((struct eq_seg_info *)eq_type_tab[EQ_MODE_CUSTOM],10);
    }

}

/**
 * @brief 获取 effect 调节的等级
 * 
 * @return int 
 */
int u_effect_level_get(EFFECT_CLASS mode){
#ifdef U_EFFECT_MODE_EN
    if(!info||!info->en||U_EFFECT_MIC_VOL>mode||mode>=U_EFFECT_MAX){
        return U_RUN_ERROR;
    }

    return info->level[mode].level;
#endif    
}


/**
 * @brief 设置 effect 调节的参数
 * 
 * @param mode 设置类型
 * @param value 设置参数
 * @return int 成功：0 其他：失败
 */
int u_effect_value_set(EFFECT_CLASS mode,int value){
#ifdef U_EFFECT_MODE_EN
    if(!info||!info->en||U_EFFECT_MIC_VOL>mode||mode>=U_EFFECT_MAX){
        return U_RUN_ERROR;
    }

    level_map *mp=&info->level[mode];
    if(U_EFFECT_MIC_VOL==mode){
        mp->mlmax= effect_dvol_default_parm.vol_max;
        mp->mlmin = 0;
    }else if(U_EFFECT_ECHO==mode){
        u8 voimode=info->mode_table[info_effect.mode_cn].mode-1;
        mp->mlmax=eff_mode[voimode].echo_parm.parm.decayval;
        mp->mlmin=0;
    }else if(U_EFFECT_REVERB==mode){
        u8 voimode=info->mode_table[info_effect.mode_cn].mode-1;
        mp->mlmax=eff_mode[voimode].echo_parm.parm.wetgain;
        mp->mlmin=0;
    }else if(U_EFFECT_ACCOMPANIMENT==mode){
    }

    if(mp->lmax<value){
        value=mp->lmax;
    }else if(mp->lmin>value){
        value=mp->lmin;
    }
    info->level[mode].level=value;

    if(mp->lmax-mp->lmin){
        mp->mlevel=
        (mp->level*(mp->mlmax-mp->mlmin))/(mp->lmax-mp->lmin);
    }else{
        return U_RUN_ERROR;
    }

    if(U_EFFECT_MIC_VOL==mode){
        extern void *sys_digvol_group;
        if(sys_digvol_group){
            audio_dig_vol_group_vol_set(sys_digvol_group,MIC_MIC_DIG,3, info->level[mode].mlevel);
            u_effect_mic_umute();
        }
        return U_RUN_SUCCE;
    }else if(U_EFFECT_ECHO==mode){
        mic_effect_set_echo_decay(mp->mlevel);
    }else if(U_EFFECT_REVERB==mode){
        mic_effect_set_reverb_wet(mp->mlevel);
        u_effect_value_set(U_EFFECT_ECHO,mp->level);
    }else if(U_EFFECT_ACCOMPANIMENT==mode){
        return U_RUN_SUCCE;
    }else{
        return U_RUN_ERROR;
    }

    if(!tone_get_status()){
        if(mp->level==mp->lmin){
            // if(tone_play_by_path(TONE_RES_ROOT_PATH"tone/min.*", 1)){
            //     tone_play_by_path(TONE_NORMAL, 0);
            // }
            tone_play_by_path(tone_table[IDEX_TONE_MAX_VOL], 1);
        }else if(mp->level==mp->lmax){
            // if(tone_play_by_path(TONE_RES_ROOT_PATH"tone/max.*", 1)){
            //     tone_play_by_path(TONE_NORMAL, 0);
            // }
            tone_play_by_path(tone_table[IDEX_TONE_MAX_VOL], 1);
        }
    }
#endif
    return U_RUN_SUCCE;
}

/**
 * @brief 重载数据
 * 
 */
void u_effect_load_cfg(void){
#ifdef U_EFFECT_MODE_EN
    if(!info || !info->en){
        return;
    }

    //数据重置
    if(info->reset){
        log_debug("level table size=%d\r\n",MY_SIZE(info->level));
        for(int i=0;i<MY_SIZE(info->level);i++){
            // info->level[i].level=info->level[i].backup;
            info->level[i].level=info->level[i].lmax;
        }
        info->mode_cn=0;
    }

    u_effect_mode_switch(info->mode_cn,0);

    if(U_RUN_ERROR!=u_effect_level_get(U_EFFECT_MIC_VOL)){
        u_effect_value_set(U_EFFECT_MIC_VOL,u_effect_level_get(U_EFFECT_MIC_VOL));
    }
#endif
}

/**
 * @brief mic vol up/down
 * 
 * @param cmd U_CMD_UP/U_CMD_DOW
 */
void u_effect_mic_vol(bool cmd){
#ifdef U_EFFECT_MODE_EN
    if(U_RUN_ERROR==u_effect_level_get(U_EFFECT_MIC_VOL))return;

    if(U_CMD_UP==cmd){
        u_effect_value_set(U_EFFECT_MIC_VOL,u_effect_level_get(U_EFFECT_MIC_VOL)+1);
    }else{        
        u_effect_value_set(U_EFFECT_MIC_VOL,u_effect_level_get(U_EFFECT_MIC_VOL)-1);
    }

    if(info->level[U_EFFECT_MIC_VOL].mlevel==info->level[U_EFFECT_MIC_VOL].mlmax/*话筒音量最大*/){
        if (tone_get_status() == 0) {
            tone_play(TONE_NORMAL,1);
        }
    }

    if(info->level[U_EFFECT_MIC_VOL].mlevel==info->level[U_EFFECT_MIC_VOL].mlmin/*话筒音量最小*/){
        if (tone_get_status() == 0) {
            tone_play(TONE_NORMAL,1);
        }
    }
#endif
}

/**
 * @brief mic reverb up/down
 * 
 * @param cmd U_CMD_UP/U_CMD_DOW
 */
void u_effect_reverb(bool cmd){
#ifdef U_EFFECT_MODE_EN
    if(U_RUN_ERROR==u_effect_level_get(U_EFFECT_REVERB))return;

    if(U_CMD_UP==cmd){
        u_effect_value_set(U_EFFECT_REVERB,u_effect_level_get(U_EFFECT_REVERB)+1);
    }else{
        u_effect_value_set(U_EFFECT_REVERB,u_effect_level_get(U_EFFECT_REVERB)-1);
    }
    log_debug("reverb leve=%d",u_effect_level_get(U_EFFECT_REVERB));
#endif
}


/**
 * @brief mic accompaniment up/down
 * 
 * @param cmd U_CMD_UP/U_CMD_DOW
 */
void u_effect_accompaniment(bool cmd){
    // if(U_RUN_ERROR==u_effect_level_get(U_EFFECT_REVERB)){
    //     return;
    // }

    if(U_CMD_UP==cmd){
        app_task_put_key_msg(KEY_VOL_UP, 0);
    }else{        
        app_task_put_key_msg(KEY_VOL_DOWN, 0);  
    }
    // log_debug("accompaniment leve=%d",u_effect_level_get(U_EFFECT_ACCOMPANIMENT));
}

#endif//U_EFFECT_MODE_EN

#endif