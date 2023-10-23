#include "user_head.h"


#define LOG_TAG_CONST        U_SERIAL
#define LOG_TAG             "[U_SERIAL]"
#define LOG_ERROR_ENABLE
#define LOG_DEBUG_ENABLE
#define LOG_INFO_ENABLE
#define LOG_DUMP_ENABLE
#define LOG_CLI_ENABLE
#include "debug/user_debug.h"

#ifdef U_SERIAL_MODE_EN

SERIAL_INFO info_serial[2]={
    [0]={
    .en=true,
    .name="u_uart_task1",
    // .uid=U_SERIAL_CODE_AMW,
    .rx={.mult=NO_CONFIG_PORT,.pro=IO_PORTA_07},
    .tx={.mult=NO_CONFIG_PORT,.pro=IO_PORTA_07},
    .baud=115200,
    .timeout=5,
    .mode=SERIAL_MODE_TASK,
    },
    [1]={
    .en=true,
    .name="u_uart_task2",
    // .uid=U_SERIAL_CODE_AMW,
    .rx={.mult=NO_CONFIG_PORT,.pro=IO_PORTA_08},
    .tx={.mult=NO_CONFIG_PORT,.pro=IO_PORTA_08},
    .baud=115200,
    .timeout=5,
    .mode=SERIAL_MODE_TASK,
    },
};

void (*u_serial_code_table[SERIAL_CODE_ID_MAX][SERIAL_EVE_MAX])(u8 *buff,u16 len)={
    [SERIAL_CODE_ID_AMW]={u_serial_code_amw_dev,u_serial_code_amw_key,u_serial_code_amw_user},
    [SERIAL_CODE_ID_JHC]={u_serial_code_jhc_dev,u_serial_code_jhc_key,u_serial_code_jhc_user},
};

static SERIAL_BUFF buff[2]={
    [0]={
    .leng_rx=U_UART_RX_SIZE,
    .leng_tx=U_UART_TX_SIZE,
    .leng_dm=U_UART_DM_SIZE,
    },
    [1]={
    .leng_rx=U_UART_RX_SIZE,
    .leng_tx=U_UART_TX_SIZE,
    .leng_dm=U_UART_DM_SIZE,
    },
};


static void my_put_u8hex(u8 dat){
    u8 tmp;
    tmp = dat / 16;
    if (tmp < 10) {
        putchar(tmp + '0');
    } else {
        putchar(tmp - 10 + 'A');
    }
    tmp = dat % 16;
    if (tmp < 10) {
        putchar(tmp + '0');
    } else {
        putchar(tmp - 10 + 'A');
    }
    putchar(0x20);
}

/**
 * @brief rx code
 * 
 * @param index 用户id
 * @param buff rx buff
 * @param len rx read data leng
 */
static void u_serial_rx_code(u8 *buff,u16 len){
    #ifdef U_SERIAL_MODE_EN
    /**
     * @brief 
     * BUFF[0] rx传递过来的数据包大小
     * BUFF[1] 用户id
     * BUFF[2] 事件类型 SERIAL_EVE
     * BUFF[3~len-1] 事件对应的消息
     * 
     * BUFF[2]=SERIAL_EVE_DEV
     * BUFF[3]=dev：mic1  mic2
     * BUFF[4]=msg：v+  v-
     * 
     * SERIAL_EVE:
     * SERIAL_EVE_KEY
     * BUFF[2]=SERIAL_EVE_KEY
     * BUFF[3]=KEY_NUMBER 0~KEY_SER_NUM_MAX
     * BUFF[4]=KEY_EVE 0~SERIAL_EVE_KEY
     * 
     */
    // printf("--mcu-[%d %d %d %d %d]\r\n", 
    // buff[0],buff[1],buff[2],buff[3],buff[4]);

    u8 data_len=buff[0];
    u8 index=buff[1];
    u8 eve_id=buff[2];
    if((len<3 || !data_len) ||
    (index>=SERIAL_CODE_ID_MAX) ||
    (eve_id>=SERIAL_EVE_MAX)){
        return;
    }

    if(u_serial_code_table[index][eve_id]){//三个类型 
        u_serial_code_table[index][eve_id](buff+3,len-3);
    }
    #endif
}

/**
 * @brief 发送串口数据
 * 
 * @param arg 
 * @param buf 发送的数据
 * @param len 发送数据的长度
 */
static void u_uart_send_data(void *arg,u8 *buf, u32 len){
    SERIAL_INFO * serial=arg;
	if(serial && serial->udev && len<=serial->buff->leng_tx){
        g_printf("uart send data len=%d",len);
        memset(serial->buff->pTxBuffe,0,serial->buff->leng_tx);
        memcmp(serial->buff->pTxBuffe,buf,len);
		serial->udev->write(serial->buff->pTxBuffe, len);
	}
}
/**
 * @brief 数据接收任务
 * 
 * @param arg 
 */
static void u_task(void *arg){
    SERIAL_INFO * serial=arg;
    const uart_bus_t *uart_bus = serial->udev;
    int ret;

    printf("uart_u_task start name=%s p=%x\n",serial->name,serial);
    while (1) {
        memset(serial->buff->pRxBuffe,0x0,serial->buff->leng_rx);//数据清零
        //uart_bus->read()在尚未收到串口数据时会pend信号量，挂起task，直到UART_RX_PND或UART_RX_OT_PND中断发生，post信号量，唤醒task
        serial->packer_rx.length = uart_bus->read(serial->buff->pRxBuffe, serial->buff->leng_rx, 0);
        if(!serial->packer_rx.length)continue;
        u_serial_rx_code(serial->buff->pRxBuffe,serial->packer_rx.length);//RX发给DSP的数据
    }
}

static void u_serial_isr_cb(void *arg, u32 status){
    const uart_bus_t *ubus = arg;
    struct sys_event e;
    SERIAL_INFO *serial=NULL;

    for(int i=0;i<MY_SIZE(info_serial);i++){
        if(ubus==info_serial[i].udev){
            serial=&info_serial[i];
        }
    }
    if(!serial)return;

    //当CONFIG_UARTx_ENABLE_TX_DMA（x = 0, 1）为1时，不要在中断里面调用ubus->write()，因为中断不能pend信号量
    if (status == UT_RX) {
        // log_debug("uart_rx_isr\n");
        e.type = SYS_USER_EVENT;
        e.arg = (void *)DEVICE_EVENT_FROM_UART_RX_OVERFLOW;
        e.u.dev.event = DEVICE_EVENT_CHANGE;
        e.u.dev.value = (int)serial;
        sys_event_notify(&e);
    }else if (status == UT_RX_OT) {
        // log_debug("uart_rx_ot_isr\n");
        e.type = SYS_USER_EVENT;
        e.arg = (void *)DEVICE_EVENT_FROM_UART_RX_OUTTIME;
        e.u.dev.event = DEVICE_EVENT_CHANGE;
        e.u.dev.value = (int)serial;
        sys_event_notify(&e);
    }
}



/**
 * @brief open dev
 * 
 */
static int u_uart_dev_open(SERIAL_INFO *info_serial){
    if(!info_serial || !info_serial->en) return -1;

    struct uart_platform_data_t ut = {0};
    ut.tx_pin = info_serial->tx.pro;
    ut.rx_pin = info_serial->rx.pro;
    ut.baud = info_serial->baud;
    ut.rx_timeout = 10;
    if(SERIAL_MODE_ISR==info_serial->mode){
        ut.isr_cbfun = u_serial_isr_cb;
    }else if(SERIAL_MODE_TASK==info_serial->mode){        
        info_serial->task=u_task;//注释掉则是使用终端方式处理
    }
    ut.rx_cbuf = info_serial->buff->pDmBuffe;
    ut.rx_cbuf_size = info_serial->buff->leng_dm;
    ut.frame_length = info_serial->buff->leng_dm/2;
    info_serial->udev = (uart_bus_t *)uart_dev_open(&ut);
    if (info_serial->udev == NULL) {
        info_serial->en = false;
        log_error("open uart dev err \n");
        return -1;
    }

    //需要将串口IO上拉，不可删除，会影响唤醒RX功能
    u_serial_wakeup_rx();

    log_debug("u uart dev %s open ok",info_serial->name);
    return 0;
}
#endif

/**
 * @brief 自定义串口处理handl
 * 需要在app_default_event_deal接口中添加
     case SYS_USER_EVENT:
        r_printf("SYS_USER_EVENT\r\n");
        u_event_handler(event);
        break;
 * @param e 
 */
void u_serial_device_event_handler(struct sys_event *e){
#ifdef U_SERIAL_MODE_EN
    SERIAL_INFO *serial=e->u.dev.value;

    serial->packer_rx.length = serial->udev->read(serial->buff->pRxBuffe, serial->buff->leng_rx, 0);
    log_debug("---{%x %x} {%x} read data leng=%d data[0]=%d",
    info_serial,
    info_serial+1,
    serial,serial->packer_rx.length,
    serial->buff->pRxBuffe[0]);

    if(serial->packer_rx.length)u_serial_rx_code(serial->buff->pRxBuffe,serial->packer_rx.length);
    return;
#endif
}

/**
 * @brief 串口 io 前期初始化
 * 
 * @param info_serial 
 * @return int 
 */
static int u_serial_data_init(SERIAL_INFO *info_serial){

    if(!info_serial || \
        NO_CONFIG_PORT==info_serial->rx.pro || \
        !info_serial->baud){
        info_serial->en = false;
        return -1;
    }

    u_comm_gpio_set(info_serial->rx.mult,USER_GPIO_HI,0);
    u_comm_gpio_set(info_serial->tx.mult,USER_GPIO_HI,0);

    info_serial->packer_rx.payload=info_serial->buff->pRxBuffe;

    return 0;
}

/**
 * @brief 串口初始化
 * 
 * @return int 0：成功 其他：失败
 */
int u_serial_init(void){

#ifdef U_SERIAL_MODE_EN
    log_debug("u serial init ling ----serial number=%d",MY_SIZE(info_serial));
    u8 ret=0;
    SERIAL_INFO *serial=NULL;
    for(int i=0;i<MY_SIZE(info_serial);i++){
        info_serial[i].buff=&buff[i];
        serial=&info_serial[i];
        if(u_serial_data_init(serial)){
            serial->en=false;
            ret=BIT(i);
        }else if(u_uart_dev_open(serial)){
            serial->en=false;
            ret=BIT(i);
        }else if(serial->task){
            os_task_create(serial->task, serial, 31, 128, 64,   serial->name);
        }
    }

    if(ret&BIT(0) && ret&BIT(1)){
        return -1;
    }

    log_debug("u serial init ok");

#endif
    return 0;
}

/**
 * @brief 串口io设置为io输入下来
 * 
 */
void u_serial_pull_down_io(void){
    #ifdef U_SERIAL_MODE_EN
    SERIAL_INFO *serial=NULL;

    for(int i=0;i<MY_SIZE(info_serial);i++){
        serial=&info_serial[i];
        if(serial->en)u_comm_gpio_set(serial->rx.pro,USER_GPIO_IN_IO_PD,0);
    }
    #endif
}

/**
 * @brief 唤醒 rx
 * 
 */
void u_serial_wakeup_rx(void){
    #ifdef U_SERIAL_MODE_EN
    SERIAL_INFO *serial=NULL;
    for(int i=0;i<MY_SIZE(info_serial);i++){
        serial=&info_serial[i];
        if(serial->en){
            gpio_set_pull_down(serial->rx.pro,0);
            gpio_set_pull_up(serial->rx.pro,1);
        }
    }
    #endif
}

/**
 * @brief 关闭串口
 * 
 */
void u_serial_close(void){
    #ifdef U_SERIAL_MODE_EN
    SERIAL_INFO *serial=NULL;
    for(int i=0;i<MY_SIZE(info_serial);i++){
        serial=&info_serial[i];
        if(serial->en)u_comm_gpio_set(serial->rx.pro,USER_GPIO_IN_IO_PD,0);
        if(serial->udev){
            uart_dev_close(serial->udev);
            os_task_del_req(serial->name);
            printf("uart %s close\r\n",serial->name);
            serial->udev=NULL;
        }
    }
    serial->en=false;
    #endif
}