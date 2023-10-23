#include "typedef.h"
#include "system/event.h"
#include "system/includes.h"
#include "key_event_deal.h"


#define IO_TX_PIN   IO_PORTB_05
#define IO_RX_PIN   IO_PORTB_05
#define BAUD_RATE   1000000

u8 uart_cbuf[64] __attribute__((aligned(4)));
u8 uart_rxbuf[16] __attribute__((aligned(4)));
u8 uart_txbuf[16] __attribute__((aligned(4)));
uart_bus_t *uart_bus = NULL;

static void uart_send_data(u8 *buf, u32 len)
{
	if(uart_bus){
        printf("rx send data=%s len=%d\r\n",buf,len);
		uart_bus->write(buf, len);
	}
}

void delay_send_data(void){
    uart_bus->read(uart_rxbuf, 16, 0);
    u8 key=uart_rxbuf[0]-'0';
    printf("--rx---%d---",key);
    memset(uart_txbuf,0,16);
    // sprintf(uart_txbuf,"-[%02d]-",key);
    static u8 c_buf[16]="abcd";
    uart_send_data(c_buf,strlen(c_buf));
}
static void uart_isr_hook(void *arg, u32 status){
    if(!uart_bus){
        printf("uart error---\r\n");
        return;
    }

	if (status == UT_RX) {
        printf("uart_rx_isr\n");
        sys_timeout_add(NULL,delay_send_data,2);
//	    app_task_put_key_msg(KEY_UART_RECEIVE, 0);
	}else if (status == UT_RX_OT) {
        printf("uart_rx_ot_isr\n");
        sys_timeout_add(NULL,delay_send_data,2);
	}
}
void uart_close(void)
{
    printf("uart_close");
    if (uart_bus) {
        uart_dev_close(uart_bus);
    }
}

void pull_down_uart_io(void)
{
    gpio_set_direction(IO_TX_PIN,1);
    gpio_set_pull_up(IO_TX_PIN,0);
    gpio_set_pull_down(IO_TX_PIN,1);
    gpio_set_die(IO_TX_PIN,1);
}

void uart_init_test(){
    r_printf("uart_init_test");

    // pull_down_uart_io();

    struct uart_platform_data_t ut = {0};
    ut.tx_pin = IO_TX_PIN;
    ut.rx_pin = IO_RX_PIN;
    ut.baud = BAUD_RATE;
    ut.rx_timeout = 10;
    ut.isr_cbfun = uart_isr_hook;
    ut.rx_cbuf = uart_cbuf;
    ut.rx_cbuf_size = 64;
    ut.frame_length = 10;
    uart_bus = (uart_bus_t *)uart_dev_open(&ut);
    if (uart_bus == NULL) {
         printf("open uart dev err\n");
         return;
    }
    //��Ҫ������IO����������ɾ������Ӱ�컽��RX����
    gpio_set_pull_down(ut.tx_pin,0);
    gpio_set_pull_up(ut.tx_pin,1);
}
