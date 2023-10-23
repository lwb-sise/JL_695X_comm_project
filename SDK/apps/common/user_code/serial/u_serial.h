#ifndef __U_SERIAL_H__
#define __U_SERIAL_H__

#define U_UART_TASK_1   "uart_task_1"
#define U_UART_TASK_2   "uart_task_2"

#define U_UART_RX_SIZE        (32)//0x100
#define U_UART_TX_SIZE        (32)//0x30
#define U_UART_DM_SIZE        (64)//0x100
typedef struct _SERIAL_BUFF_{
    u8 pRxBuffe[U_UART_RX_SIZE] __attribute__((aligned(4)));
    u8 pTxBuffe[U_UART_TX_SIZE] __attribute__((aligned(4)));
    u8 pDmBuffe[U_UART_DM_SIZE] __attribute__((aligned(4)));
    int leng_rx;
    int leng_tx;
    int leng_dm;
}SERIAL_BUFF;

typedef enum _heartbeat_enum_{
    HEARTBEAT_INIT,//初始化
    HEARTBEAT_PACK,//包
    HEARTBEAT_CHECK,//查看是否触发
    HEARTBEAT_EXIT,//退出
}HEARTBEAT_ENUM;

typedef  enum {
 /* 
    SERIAL_MODE_ISR
    需要在app_default_event_deal接口中添加
    case SYS_USER_EVENT:
        r_printf("SYS_USER_EVENT\r\n");
        u_event_handler(event);
    break;
*/
    SERIAL_MODE_ISR,//中断中处理
    SERIAL_MODE_TASK,//任务中处理
    // SERIAL_MODE_ALL,//中断与任务中处理
}SERIAL_MODE;


typedef struct {
    //head
    u16 preamble;
    u8  type;
    u16 length;
    u8  crc8;
    u16 crc16;
    u8 *payload;
}serial_packer;

typedef struct _SERIAL_INFO_{
    u8 * name;
    // u8 uid;//客户id U_SERIAL_CODE_MAX
    bool en;//模块使能
    IO_INFO rx;
    IO_INFO tx;
    SERIAL_BUFF *buff;
    SERIAL_MODE mode;//串口接收数据处理方式
    u32 leng_rx;
    u32 leng_tx;
    u32 leng_dm;
    u32 timeout;
    u32 baud;//
    serial_packer packer_rx;
    uart_bus_t * udev;

    void (*task)(void *arg);
}SERIAL_INFO;

/**
 * @brief 串口io设置为io输入下来
 * 
 */
void u_serial_pull_down_io(void);

/**
 * @brief 唤醒 rx
 * 
 */
void u_serial_wakeup_rx(void);

/**
 * @brief 自定义串口处理handl
 * 
 * @param e 
 */
void u_serial_device_event_handler(struct sys_event *e);

/**
 * @brief 串口初始化
 * 
 * @return int 0：成功 其他：失败
 */
int u_serial_init(void);

/**
 * @brief 关闭串口
 * 
 */
void u_serial_close(void);

// void u_serial_data_rx_handler(struct device_event *dev);
#endif