#if ((defined CONFIG_CPU_BR30) || (defined CONFIG_CPU_BR29))

#include "app_msg.h"
#include "device/key_driver.h"
#include "system/event.h"
#include "system/init.h"
#include "iokey.h"
#include "adkey.h"
#include "slidekey.h"
#include "irkey.h"
#include "touch_key.h"
#include "system/timer.h"
#include "asm/power_interface.h"
#include "app_config.h"
#include "rdec_key.h"
#include "asm/uart_dev.h"
// #include "include_lib/system/device/tent600_key.h"


#include "u_wireless_list.h"
#include "u_wireless.h"

extern uart_bus_t *uart_bus;

static W_INFO winfo={
    .id=SERIAL_CODE_ID_AMW,
    .flag=SDK_FLAG_2T1,
    .ks=KEY_EVE_MIC_VOL,
};

static void u_wifno_init(void){
    winfo.ks=KEY_EVE_MIC_VOL;
}

void u_wireldess_print(u8 *flag,u8 *buff){
    // u8 output_str[128];
    // printf("--%s--",flag);
    // for(int i=1;i<buff[0];i++){
    //     printf(" %d",buff[i]);
    // }
    printf("---%s [%d %d %d %d %d %d]",flag,buff[0],buff[1],buff[2],buff[3],buff[4],buff[5]);
    // printf("---%s %s",flag,buff+1);
}

/**
 * @brief 设置无线麦连接状态
 * 
 * @param dev 设备
 * @param state 状态
 * 
 * 断开、连接消息处调用
 * 
 *706  Tx                                   Rx
 *     BIG_EVENT_TRANSMITTER_CONNECT        BIS_EVENT_RECEIVER_CONNECT
 *     BIG_EVENT_TRANSMITTER_DISCONNECT     BIG_EVENT_RECEIVER_DISCONNECT
 *
 * 697  Rx
            switch (event->u.dev.event) {
            case ADAPTER_EVENT_CONNECT :
                if (event->u.dev.value == 1) {
                    printf("ADAPTER_EVENT_CONNECT_FIRST\n");
                    wireless_conn_status |= BIT(0);
                    u_fun_set_con_state(DEV_EVE_ONLIEN_MIC_2,true);
                } else {
                    printf("ADAPTER_EVENT_CONNECT_SECOND\n");
                    wireless_conn_status |= BIT(1);
                    u_fun_set_con_state(DEV_EVE_ONLIEN_MIC_1,true);
                }
 * 
 *697 Tx
 * 
        case DEVICE_EVENT_FROM_ADAPTER:
            switch (event->u.dev.event) {
            case ADAPTER_EVENT_CONNECT :
                printf("ADAPTER_EVENT_CONNECT\n");
                wireless_conn_status = 1;
                ui_update_status(STATUS_BT_CONN);
                sys_auto_shut_down_disable();
                u_fun_set_con_state(DEV_EVE_ONLIEN_MIC_1,true);
                break;
 * 
 */

void u_fun_set_con_state(DEV_EVE dev,bool state){
    if(DEV_EVE_ONLIEN_MIC<=dev && DEV_EVE_ONLIEN_MIC_ALL>dev){
        state?(winfo.cs|=BIT(dev)):(winfo.cs&= ~BIT(dev));
        //tx
        u_wifno_init();
    }
}

/**
 * @brief 获取预发数据
 * 
 * @param buff 获取数据存放的地址
 * @param int 数据长度
 */
static int u_default_data_get(u8 *buff){
    int len=0;
    SEND_LIN * head=u_getNode();
    if(head){
        memcpy(buff,head->buff,BROADCAST_CUSTOM_DATA_LEN);
        len=head->len;
        u_deleteNode(head);
    }
    return len;
}


/**
 * @brief 保存dev data
 * 
 * @param num_args 
 * @param ... 
 * -------------------------------------------------------
 * 包长度   |   用户id  |   事件id  |   数据    
 * -------------------------------------------------------
 * 1字节    |   1字节   |   1字节   |   N
 * -------------------------------------------------------
 */
static void u_default_save_dev(int num_args,...){
    va_list args;
    va_start(args, num_args);
    u8 sed_buff[num_args+3];
    for (int i = 0; i < num_args; i++){
        sed_buff[i+3] = va_arg(args, int);
    }
    va_end(args);

    sed_buff[0]=num_args+3;
    sed_buff[1]=winfo.id;
    sed_buff[2]=SERIAL_EVE_DEV;

    u_addNode(sed_buff);
}

/**
 * @brief save default key data
 * 
 * @param key 
 * @param value 
 * -------------------------------------------------------
 * 包长度   |   用户id  |   事件id  |   数据    
 * -------------------------------------------------------
 * 1字节    |   1字节   |   1字节   |   N
 * -------------------------------------------------------
 */
static void u_default_save_key(u8 key,u8 value){
    u8 sed_buff[]={
        1,
        winfo.id,
        SERIAL_EVE_KEY,
        winfo.ks,
        key,
        value,
    };
    sed_buff[0]=sizeof(sed_buff);

    u_addNode(sed_buff);
}

/**
 * @brief tx 预发自定义事件
 * 
 * @param num_args ...代表的参数个数
 * @param ... 
 * -------------------------------------------------------
 * 包长度   |   用户id  |   事件id  |   数据    
 * -------------------------------------------------------
 * 1字节    |   1字节   |   1字节   |   N
 * -------------------------------------------------------
 */
void u_default_save_user(int num_args,...){
    u8 sed_buff[num_args+3];
    // printf("num_args %d",num_args);
    va_list args;
    va_start(args, num_args);
    for (int i = 0; i < num_args; i++){
        sed_buff[i+3] = va_arg(args, int);
    }
    va_end(args);

    sed_buff[0]=num_args+3;
    sed_buff[1]=winfo.id;
    sed_buff[2]=SERIAL_EVE_USER;

    u_addNode(sed_buff);
    // u_wireldess_print("user",sed_buff);
}


/**********************************TX*******************************************/

/**
 * @brief tx拿预发数据
 * 
 * @param buff 拿到数据存放的地址
 * @param int 数据长度
 * 
 * 706:
 * u_wtx_data_get(transmit_buf + get_big_audio_transmit_data_len());
 * 替换
 * memcpy(transmit_buf + get_big_audio_transmit_data_len(), broadcast_data_sync, BROADCAST_CUSTOM_DATA_LEN);
 */
int u_wtx_data_get(u8 *buff){
    buff[0]=0;//在没有获取到发送数据时buff长度设置为0
    int ret=u_default_data_get(buff);
    //打印会影响发包 开打印有概率丢包
    // if(ret)u_wireldess_print("ceetx",buff);
    return ret;
}

/**
 * @brief tx 预发key事件
 * 
 * @param key 
 * @param value 
 * 调用方法
 * static void key_driver_scan(void *_scan_para){
 *     ....
 *     u_wtx_send_default_key(e.u.key.value,e.u.key.event);
 *     if (key_event_remap(&e)) {
 *      sys_event_notify(&e);
 *      ......
 *      }
 *}
 *    
 */
void u_wtx_send_default_key(u8 key,u8 value){   
    if(!winfo.cs)return;

    u_default_save_key(key,value);
}

/**
 * @brief tx 预发自定义事件
 * 
 * @param num_args ...代表的参数个数
 * @param ... 
 */
void u_wtx_send_default_user(int num_args,...){
    if(!winfo.cs)return;
    #if 1
    va_list args;
    va_start(args, num_args);
    // 在这里处理参数
    
    // 传递变长参数给 u_default_save_user
    u_default_save_user(num_args, args);

    va_end(args);
    #else
    va_list args;
    va_start(args, num_args);
    u8 sed_buff[num_args+3];
    // int len=va_arg(args, int);
    for (int i = 0; i < num_args; i++){
        sed_buff[i+3] = va_arg(args, int);
    }
    va_end(args);

    sed_buff[0]=num_args+3;
    sed_buff[1]=winfo.id;
    sed_buff[2]=SERIAL_EVE_USER;

    u_addNode(sed_buff);
    #endif
}

/**
 * 按键功能变更
 * 需自己添加到按键切换消息处理处
*/
void u_wtx_key_state_loogp_update(void){
    (winfo.ks+1)>=KEY_EVE_MAX?(winfo.ks=KEY_EVE_MIC_VOL):winfo.ks++;
    u_default_save_user(2,USER_EVE_PLAY_KEY_FUN,winfo.ks);
}

/**
 * 获取按键功能
 * 按键的某些功能在tx端实现
 * eg：
 * ks为KEY_EVE_MIC_VOL时 tx端实现mic vol音量加减
 * ks为其他在muc上处理
*/
KEY_EVE u_wtx_key_state_get(void){
    return winfo.ks;
}



/**********************************RX*******************************************/
static void  u_wrx_to_mcu_delay_send(void *prve){
    SEND_LIN * head=u_getNode();
    if(head && head->buff){
        if(winfo.rx_send)winfo.rx_send(head->buff,head->buff[0]);
        u_wireldess_print("rx rend",head->buff);
        u_deleteNode(head);
    }
}

/**
 * @brief 连接心跳
 * 
 * @param prve 
 */
static void u_wrx_heartbeat(void *prve){
    u_default_save_dev(3,DEV_EVE_ONLIEN_MIC_ALL,winfo.flag,winfo.cs);
}

/**
 * @brief 转发tx数据到mcu
 * 
 * @param buff tx在没有发送数据时发给过来的buf[0]即数据长度为0
 * 
 * 706
 * 在以下接口处调用
 * static int broadcast_rx_custom_data_callback(const void *const buf, size_t length, u8 big_hdl)
 * 
 * 697
 * 在master_audio_command_parse中调用该接口
 * 
     if (data[1]) {
#if ADAPTER_UART_DEMO
        u_wrx_forward_data(&data[1]);
#endif
    }

 */
void u_wrx_forward_data(u8 *buff){
    if(!buff)return;

    u8 len = buff[0];
    if(!len)return;

    u_addNode(buff);
}

/**
 * @brief 获取串口句柄
 * 
 */
static void u_get_uart_hld(void){
    if(uart_bus){
        winfo.rx_send=uart_bus->write;
    }else{
        sys_timer_add(NULL,u_get_uart_hld,20);
    }
}

static void u_rx_task(void *p_arg){

    // ADAPTER_UART_DEMO 
    winfo.send_id=sys_timer_add(NULL,u_wrx_to_mcu_delay_send,50);
    winfo.heartbeat_id=sys_timer_add(NULL,u_wrx_heartbeat,500);
    
    sys_timer_add(NULL,u_get_uart_hld,20);

    int msg[32];
    while(1){
        app_task_get_msg(msg, ARRAY_SIZE(msg), 1);
    }

}

/**
 * @brief 初始化rx to mcu scan
 * 
 * app_main.c api app_main 中调用
 */
void u_wrx_to_mcu_init(void){
    os_task_create(u_rx_task,NULL, 31, 1024*4, 128,"u_rx_task");
}

#endif