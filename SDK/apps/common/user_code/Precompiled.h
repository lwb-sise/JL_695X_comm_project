/**
 * @file Precompiled.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-07-17
 * 
 * @copyright Copyright (c) 2023
 * 作用：程序编译与外部预编译用到的宏定义
 * 注意：该文件只能是宏定义的数字类型  不能有其他的定义
 */

#ifndef  __U_INFO_MACRO_H__
#define  __U_INFO_MACRO_H__

#ifndef NO_CONFIG_PORT
#define NO_CONFIG_PORT						(-1)
#endif

#ifndef U_ERROR
#define U_ERROR						(-1)
#endif

#ifndef ENABLE
#define ENABLE 1
#endif
#ifndef DISABLE
#define DISABLE 0
#endif


#define IO_CHECK_MODE_AD    (1)//AD检测
#define IO_CHECK_MODE_IO    (0)//IO检测
#define IO_CHECK_UP         (1)//上拉检测
#define IO_CHECK_DOW        (-1)//下拉检测
#define IO_CHECK_ON_LINE_H  (1)//高电平在线
#define IO_CHECK_ON_LINE_L  (0)//低电平在线


/****************************project********************************/
#define POWER_SUPPLY                         0//电源
#define AMPLIFLER                            1//功放
#define LAMP                                 2//灯光
#define EFFECT                               3//音效
#define KNOB_VOL                             4//系统音量旋钮
#define KNOB_BASS                            5//低音旋钮
#define KNOB_TREBLE                          6//高音旋钮
#define KNOB_ECHO                            7//混响旋钮
#define KNOB_MIC                             8//mic音量旋钮
#define KNOB_GUITAR                          9//吉他音量旋钮


/**************************parameter*********************************/

/*********AMPLIFLER******/
/*
可以注释掉不需要的功放控制驱动，注释掉的功放驱动不会被编译
*/
#define LTK_5313                1
#define LTK_5302                2
#define LTK_5325                3
#define LTK_5328                4
#define MIX_2018                5
#define MIX_2917                6
#define CS_83702E               7
#define CS_8509                 8
#define CS_8679E                9
#define CS_83705                10
#define CS_8376                 11
#define CS_38300E               12
#define HT_4831                 13
#define HT_4832                 14
#define HT_8731                 15
#define HT_326                  16
#define PA_DRIVE_MAX            17

/*********LAMP***********/
/***MODE_SPE****/
#define USER_RGB_MODE_MIX               0   //模式最小值
#define USER_RGB_MODE_OFF               1   //关灯
#define USER_RGB_MODE_POWER_OFF         2   //关机模式 根据不同灯板配置 调用对应的显示    
#define USER_RGB_MODE_POWER_ON          3   //开机模式 根据不同灯板配置 调用对应的显示
#define USER_RGB_MODE_VOL               4   //音量显示
#define USER_RGB_MODE_BASS              5   //bass
#define USER_RGB_MODE_ON_OFF            6   //ON/OFF
#define USER_RGB_MODE_EXIT              7  //EXIT
#define USER_RGB_MODE_SPE_MAX           8 //特殊模式的最大id
/***MODE_NOR****/
#define USER_RGB_MODE_NOR_MIN           9   //用户模式最小id
#define USER_RGB_MODE_NOR_0             10  //用户模式0
#define USER_RGB_MODE_NOR_1             11  //用户模式1
#define USER_RGB_MODE_NOR_2             12  //用户模式2
#define USER_RGB_MODE_NOR_3             13  //用户模式3
#define USER_RGB_MODE_NOR_4             14  //用户模式4
#define USER_RGB_MODE_NOR_5             15  //用户模式4
#define USER_RGB_MODE_NOR_6             16  //用户模式4
#define USER_RGB_MODE_NOR_7             17  //用户模式4
#define USER_RGB_MODE_NOR_8             18  //用户模式4
#define USER_RGB_MODE_NOR_9             19  //用户模式4
#define USER_RGB_MODE_NOR_10            20  //用户模式4
#define USER_RGB_MODE_NOR_11            21  //用户模式4
#define USER_RGB_MODE_NOR_12            22  //用户模式4
#define USER_RGB_MODE_NOR_13            23  //用户模式4
#define USER_RGB_MODE_NOR_14            24  //用户模式4
#define USER_RGB_MODE_NOR_15            25  //用户模式4
#define USER_RGB_MODE_NOR_16            26  //用户模式4
#define USER_RGB_MODE_NOR_17            27  //用户模式4
#define USER_RGB_MODE_NOR_18            28  //用户模式4
#define USER_RGB_MODE_NOR_19            29  //用户模式4
#define USER_RGB_MODE_NOR_20            30  //用户模式4
#define USER_RGB_MODE_NOR_21            31  //用户模式4
#define USER_RGB_MODE_NOR_22            32  //用户模式4
#define USER_RGB_MODE_NOR_23            33  //用户模式4
#define USER_RGB_MODE_NOR_24            34  //用户模式4
#define USER_RGB_MODE_MAX               35  //模式最大值

/*********EFFECT***********/
//701n MIC LEVEL 0(-8dB)~19(30dB)
#define MIC_LEVEL_MIX                   0//-8dB
#define MIC_LEVEL_MAX                   19//30dB

/************ 两种 模式切换 1.程序固定参数的模式切换 2.外部配置效果模式的切换****************
 * 两种切换接口不一样 需要程序修改
*/

//程序固定参数模式 TOOL_MODE配置为0
#define VOICE_CHANGER_MODE_NONE         1 //原声
#define VOICE_CHANGER_MODE_UNCLE        2 //大叔
#define VOICE_CHANGER_MODE_GODDESS      3 //女神
#define VOICE_CHANGER_MODE_BABY         4 //娃娃音
#define VOICE_CHANGER_MODE_MAGIC        5 //魔音女声
#define VOICE_CHANGER_MODE_MONSTER      6 //怪兽音
#define VOICE_CHANGER_MODE_DONALD_DUCK  7 //唐老鸭
#define VOICE_CHANGER_MODE_MINIONS      8 //小黄人
#define VOICE_CHANGER_MODE_ROBOT        9 //机器音
#define VOICE_CHANGER_MODE_WHISPER      10 //气音
#define VOICE_CHANGER_MODE_MELODY       11//固定旋律音
#define VOICE_CHANGER_MODE_FEEDBACK     12//调制音
#define VOICE_CHANGER_MODE_REVERB       13//调制音
#define VOICE_CHANGER_MODE_ECHO         14//固定旋律音
#define VOICE_CHANGER_MODE_MAX          15//MAX

//多少种外部配置效果 TOOL_MODE配置为1
#define VOICE_EFFECT_REVERB_PARM_1   1//配置1
#define VOICE_EFFECT_REVERB_PARM_2   2//配置2
#define VOICE_EFFECT_REVERB_PARM_3   3//配置3
#define VOICE_EFFECT_REVERB_PARM_4   4//配置4
#define VOICE_EFFECT_REVERB_PARM_5   5//配置5
#define VOICE_EFFECT_REVERB_PARM_6   6//配置6
#define VOICE_EFFECT_REVERB_PARM_7   7//配置7
#define VOICE_EFFECT_REVERB_PARM_8   8//配置8
#define VOICE_EFFECT_REVERB_PARM_MAX 9//MAX

#endif