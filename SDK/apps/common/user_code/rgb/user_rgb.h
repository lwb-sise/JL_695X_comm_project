#ifndef __USER_RGB_H
#define __USER_RGB_H


#define USER_RGB_NUM         6  //灯的总数量
#define USER_RGB_NUM_1       4   //另一组灯的数量

#define CIRCLE_RGB_NUMBER   (6) //圆点的数量
#define USER_RGB_ROW_NUM     6  //每行灯的数量

#define RGB_LOW_ENERGY    10
#define RGB_MID_ENERGY    1000
#define RGB_HIG_ENERGY    3000



 enum e_rgb_status
{
    E_RGB_MODE_0 = 0x00,
    E_RGB_MODE_1,
    E_RGB_MODE_2,
    E_RGB_MODE_3,
    E_RGB_MODE_4,
    E_RGB_MODE_5,
    E_RGB_MODE_6,
    E_RGB_MODE_7,
    E_RGB_MODE_8,
    E_RGB_MODE_9,
    E_RGB_MODE_10,
    E_RGB_MODE_11,
    E_RGB_MODE_12,
    E_RGB_MODE_13,
    E_RGB_MODE_14,
    E_RGB_MODE_MAX,
    E_RGB_MODE_POWEROFF,
} ;

typedef struct __USER_RGB_STATUS{
    u8 rgb_led_init;///rgb 初始化标志位
    u8 rgb_led_work_mode;///rgb 模式
    u8 rgb_cur_work_mode;///rgb 当前模式
    u8 rgb_led_work_max;///rgb 模式最大值
    u16 rgb_led_scan_flag;///rgb 扫描标志位
    u8 fm_flag;///fm 标志位
    u8 rgb_r;///rgb 红
    u8 rgb_g;///rgb 绿
    u8 rgb_b;///rgb 蓝
    u32 energy_flash;/// 能量值
    u16 rgb_flash_cnt;///计数器
    u8 rgb_show_num;///rgb亮灯的数量
    COLOUR_HSV colour_hsv; //hsv
    // COLOUR_RGB colour_grb; //rgb
}USER_RGB_STATUS;

typedef struct _RGB_INFO_
{
    u8 exit_flag;          //退出标志
    u8 on_off;             //开关灯标志 其他：开灯 0关灯
    u8 send_cn;            //发送rgb数据次数
    u8 Normal_mode;        //灯效模式
    u32 time;              //模式扫描时间
    u16 light_n;           //亮灯数量
    u32 energy;
    COLOUR_HSV  hsv_v[USER_RGB_NUM]; //hsv
} RGB_INFO;

extern int audio_dac_energy_get(void);//获取能量值

extern USER_RGB_STATUS user_rgb;
extern RGB_INFO *_this_;

extern u8 light_off;
extern u8 reback_light;

void user_rgb_mode_set(u8 mode);
u8 get_user_rgb_work_mode(void);

void user_rgb_init(void);
void user_rgb_deal(void);

void rgb_led_scan(void *priv);
void user_rgb_speed_deal(void);

void user_rgb_clear();

//=============from led_spi.c=====================
void led_spi_init(void);//初始化spi
void led_spi_rgb_to_24byte(u8 r, u8 g, u8 b, u8 *buf, int idx);//rgb转24字节
void led_spi_send_rgbbuf(u8 *rgb_buf, u16 led_num);//发送rgb数据
extern u8 spi_dat_buf[24 * USER_RGB_NUM] __attribute__((aligned(4)));//24字节数据缓存

#if M_USER_RGB_DOUBLE_SPI
    extern u8 spi_dat_buf1[24 * USER_RGB_NUM_1] __attribute__((aligned(4)));
    void led_spi_send_rgbbuf1(u8 *rgb_buf, u16 led_num);//发送rgb数据
    void user_rgb_deal1(void);
#endif

#endif