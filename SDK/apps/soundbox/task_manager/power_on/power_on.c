#include "system/includes.h"
#include "media/includes.h"
#include "app_config.h"
#include "tone_player.h"
#include "asm/charge.h"
#include "app_charge.h"
#include "app_main.h"
#include "ui_manage.h"
#include "vm.h"
#include "app_chargestore.h"
#include "user_cfg.h"
#include "ui/ui_api.h"
#include "app_task.h"
#include "key_event_deal.h"
#include "user_head.h"


#define LOG_TAG_CONST       APP_IDLE
#define LOG_TAG             "[APP_IDLE]"
#define LOG_ERROR_ENABLE
#define LOG_DEBUG_ENABLE
#define LOG_INFO_ENABLE
/* #define LOG_DUMP_ENABLE */
#define LOG_CLI_ENABLE
#include "debug.h"

#if DEV_ONLINE_BK_POWERON_TONE 
u8 status_power_on=0;
u8 flag_Dev_online=0;
#endif

static void  lcd_ui_power_on_timeout(void *p)
{
#if (TCFG_SPI_LCD_ENABLE)
    /* sys_key_event_enable(); */
    /* logo_time = timer_get_ms(); */
    /* while (timer_get_ms() - logo_time <= 2 * 1000) { //显示开机logo */
    /* os_time_dly(10); */
    /* } */

    UI_HIDE_WINDOW(ID_WINDOW_POWER_ON);
    UI_SHOW_WINDOW(ID_WINDOW_MAIN);
#if TCFG_APP_BT_EN
    app_task_switch_to(APP_BT_TASK);
#else
    app_task_switch_to(APP_MUSIC_TASK);
#endif
#endif

}

static void  lcd_ui_power_on()
{
#if (TCFG_SPI_LCD_ENABLE)
    int logo_time = 0;
    UI_SHOW_WINDOW(ID_WINDOW_POWER_ON);
    sys_timeout_add(NULL, lcd_ui_power_on_timeout, 1000);
#endif
}

static int power_on_init(void)
{
    ///有些需要在开机提示完成之后再初始化的东西， 可以在这里初始化
#if (TCFG_SPI_LCD_ENABLE)
    lcd_ui_power_on();//由ui决定切换的模式
    return 0;
#endif

#if DEV_ONLINE_BK_POWERON_TONE
    status_power_on = 1;
    //上电瞬间能检测到外设，防止开机提示音播放不完整
    if(flag_Dev_online & BIT(E_DEV_USB_ONLINE)){
        g_printf("-----enter usb----\n");
        app_task_switch_to(APP_MUSIC_TASK);
    }else if(flag_Dev_online & BIT(E_DEV_SD_ONLINE)){
        g_printf("----enter sd ------\n");
        app_task_switch_to(APP_MUSIC_TASK);

    }else
#if TCFG_APP_LINEIN_EN

    if(flag_Dev_online & BIT(E_DEV_AUX_ONLINE)){
        g_printf("-----enter linein----\n");
        app_task_switch_to(APP_LINEIN_TASK);
    }else

#endif//TCFG_APP_LINEIN_EN

#endif//DEV_ONLINE_BK_POWERON_TONE

#if TCFG_APP_BT_EN
    {
        app_task_switch_to(APP_BT_TASK);
    }
#else

#if TCFG_USB_APPLE_DOCK_EN //苹果iap协议使用pc模式
    app_task_switch_to(APP_PC_TASK);
#else
    app_task_switch_to(APP_SLEEP_TASK);
    /* app_task_switch_to(APP_PC_TASK); */
    /* app_task_switch_to(APP_MUSIC_TASK); */
    /* app_task_switch_to(APP_IDLE_TASK); */
    /* app_task_switch_to(APP_LINEIN_TASK);//如果带检测，设备不在线，则不跳转 */
#endif

#endif

    return 0;
}

static int power_on_unint(void)
{

    tone_play_stop();
    UI_HIDE_CURR_WINDOW();
    return 0;
}






static int poweron_sys_event_handler(struct sys_event *event)
{
    switch (event->type) {
    case SYS_KEY_EVENT:
        break;
    case SYS_BT_EVENT:
        break;
    case SYS_DEVICE_EVENT:
        break;
    default:
        return false;
    }
    return false;
}


static void  tone_play_end_callback(void *priv, int flag)
{
    int index = (int)priv;

    if (APP_POWERON_TASK != app_get_curr_task()) {
        log_error("tone callback task out \n");
        return;
    }

    switch (index) {
    case IDEX_TONE_POWER_ON:
        power_on_init();
        break;
    }
}

//=======================-电量播报-================================
#if USE_SUOAI_BAT_TONE
u16 poweron_bat_check_timer=0;
u16 bat_val_poweron=0;
extern u16 get_vbat_level(void);
/**
** 提示音的链表最后的一个参数必须是NULL,不然执行链表提示音播放的时候会跑死
*/
static u32 power_on_list[] ={
    (u32)TONE_POWER_ON,(u32)NULL,(u32)NULL
};

static const u32 bat0_9[] = {
    (u32)TONE_BAT1,
    (u32)TONE_BAT2,
    (u32)TONE_BAT3,
    (u32)TONE_BAT4,
    (u32)TONE_BAT5,
    (u32)TONE_BAT6,
    (u32)TONE_BAT7,
    (u32)TONE_BAT8,
    (u32)TONE_BAT9,
    (u32)TONE_BAT10,
} ;


/**
 * @brief 筛选出对应电量的提示音
 * 
 */
static void  bat_level_list_add()
{
    log_info("========bat_level_list_add=========");
    if(bat_val_poweron>=400)
        power_on_list[1]=bat0_9[9];
    else if(bat_val_poweron>=395)
        power_on_list[1]=bat0_9[8];
    else if(bat_val_poweron>=390)
        power_on_list[1]=bat0_9[7];
    else if(bat_val_poweron>=385)
        power_on_list[1]=bat0_9[6];
    else if(bat_val_poweron>=380)
        power_on_list[1]=bat0_9[5];
    else if(bat_val_poweron>=375)
        power_on_list[1]=bat0_9[4];
    else if(bat_val_poweron>=370)
        power_on_list[1]=bat0_9[3];
    else if(bat_val_poweron>=350)
        power_on_list[1]=bat0_9[2];
    else if(bat_val_poweron>=338)
        power_on_list[1]=bat0_9[1];
    else
        power_on_list[1]=bat0_9[0];
        
}

/**
 * @brief 选出最大值
 * 
 * @param in_vlaue 
 * @return int 
 */
int u_filter_vbat_level(int in_vlaue){
    #define USER_SAVE_TIMER (1000)//ms
    #define USER_SAVE_MAX_VBAT   (10)//周期N=USER_SAVE_TIMER*USER_SAVE_MAX_VBAT ms

    int vabt_max = 0;
    static int table_max[USER_SAVE_MAX_VBAT]={0};

    u32 cur_ms = (timer_get_ms()/USER_SAVE_TIMER)%USER_SAVE_MAX_VBAT;
    static u32 old_ms = 0;
    if(old_ms!=cur_ms){
        table_max[cur_ms] = 0;
        old_ms = cur_ms;
    }

    if(table_max[cur_ms]<in_vlaue){
        table_max[cur_ms] = in_vlaue;
    }

    //最大值
    for(int i = 0;i<USER_SAVE_MAX_VBAT;i++){
        if(vabt_max<table_max[i]){
            vabt_max = table_max[i];
        }
    }

    return vabt_max;
}

/**
 * @brief 播放开机提示音
 * 
 */
void power_on_tone_play(){
    log_info("-------%s-------",__func__);
    extern int tone_play_open_with_callback_base(const char **list, u8 follow, u8 preemption, void (*evt_handler)(void *priv, int flag), void *evt_priv, int sync_confirm_time);
    int err = tone_play_open_with_callback_base(power_on_list, 1, 1, tone_play_end_callback, (void *)IDEX_TONE_POWER_ON, 0);
    if (err) { //提示音没有,播放失败，直接init流程
        power_on_init();
    }
}

/**
 * @brief 开机电量检测 
 * 
 * @param priv 
 */
static void poweron_bat_check(void *priv)
{
    static u8 poweron_bat_check_cnt=0;
    u16 bat_val_temp;
    bat_val_temp = get_vbat_level();
    printf(">>>>>>>>>>>>>>>>>>>bat_val_temp====%d\n",bat_val_temp);
    bat_val_temp= u_filter_vbat_level(bat_val_temp);//取最大值
    bat_val_poweron=bat_val_temp;       
        

    poweron_bat_check_cnt++;
    if(poweron_bat_check_cnt>=10)
    {
        poweron_bat_check_cnt=0;
        if(poweron_bat_check_timer){
            sys_timeout_del(poweron_bat_check_timer);
            poweron_bat_check_timer = 0;
            bat_level_list_add();
            extern void power_on_tone_play();
            power_on_tone_play();
        }
    }
    else
    {   
        poweron_bat_check_timer = sys_timeout_add(NULL, poweron_bat_check, 10);
    }
}


#endif

void app_poweron_task()
{
    int msg[32];

    UI_SHOW_MENU(MENU_POWER_UP, 0, 0, NULL);
    u_pa_control(PA_MUTE);
    os_time_dly(10);
    tone_play(TONE_POWER_ON_MUTE,1);
    os_time_dly(20);
#if USE_SUOAI_BAT_TONE
    if(poweron_bat_check_timer==0)
    {
        poweron_bat_check_timer = sys_timeout_add(NULL, poweron_bat_check, 10);
    }
#else
    int err =  tone_play_with_callback_by_name(tone_table[IDEX_TONE_POWER_ON], 1, tone_play_end_callback, (void *)IDEX_TONE_POWER_ON);
#endif     
    /* if (err) { //提示音没有,播放失败，直接init流程 */
    /* power_on_init(); */
    /* } */


    while (1) {
        app_task_get_msg(msg, ARRAY_SIZE(msg), 1);
        switch (msg[0]) {
        case APP_MSG_SYS_EVENT:
            if (poweron_sys_event_handler((struct sys_event *)(msg + 1)) == false) {
                app_default_event_deal((struct sys_event *)(&msg[1]));    //由common统一处理
            }
            break;
        default:
            break;
        }

        if (app_task_exitting()) {
            power_on_unint();
            return;
        }
    }

}

