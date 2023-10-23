#ifndef _HEAD_H_
#define _HEAD_H_

// #include "u_define.h"

#include "cpu.h"
#ifdef CONFIG_CPU_BR28
#include "asm/br28.h"//701
#endif
#ifdef CONFIG_CPU_BR25
#include "asm/br25.h"//696
#endif
#ifdef CONFIG_CPU_BR23
#include "asm/br23.h"//695
#endif
#include "typedef.h"

#include "malloc.h"//打印内存使用情况

#include "dac.h"
#include "generic/typedef.h"

#include "gpio.h"
#include "asm/adc_api.h"


#include "audio_config.h"
#include "tone_player.h"

#include "audio_adc.h"
#include "bt_tws.h"
#include "classic/tws_api.h"

#include "app_task.h"
#include "audio_dec_bt.h"
#include "music_player.h"

#include "key_driver.h"
#include "key_event_deal.h"
#include "app_config.h"

#include "system/timer.h"
#include "timer.h"

#include "app_power_manage.h"

#include "asm/power_interface.h"

#include "asm/rtc.h"

#include "generic/typedef.h"
#include "application/audio_vocal_remove.h"

#ifdef CONFIG_CPU_BR25
#include "application/audio_eq.h"
#endif

#include "mic_effect.h"

#include "user_cfg.h"

#include "app_main.h"
extern APP_VAR app_var;

#include "btstack/avctp_user.h"
#include "asm/spi.h"
#include "system/includes.h"
#include "media/includes.h"

#ifndef __U_APPLICATION_H__
#define __U_APPLICATION_H__
#include "user_application.h"
#endif

#include "Precompiled.h"

#define my_timeout_add sys_timeout_add
#define my_timeout_del  sys_timeout_del
#define my_time_add sys_timer_add
#define my_time_modify  sys_timer_modify
#define my_time_del sys_timer_del

#define MY_SIZE(x)  (sizeof(x)/sizeof(x[0]))


// sdx : 0：sd0 1：sd1
// sd_io ： 0：CMD脚 1：CLK脚 2：DAT脚
extern u8 sd_io_suspend(u8 sdx, u8 sd_io);
extern u8 sd_io_resume(u8 sdx, u8 sd_io);
extern AUDIO_CONFIG audio_cfg;
extern u32 adc_set_sample_freq(u32 ch, u32 ms);


#endif//_HEAD_H_
