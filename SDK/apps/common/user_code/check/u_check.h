#ifndef __U_CHECK_H__
#define __U_CHECK_H__

typedef struct _io_check_{
    bool en;//检测使能
    u8 *name;
    IO_INFO io;//io口
    int up_dow;//0:不开上下拉 1:开上拉 -1:开下拉
    int mode;//0:io检测 1:ad检测
    /**
     * @brief mode 0
     * threshold 为1 高在线
     * threshold 为0 低在线
     * 
     * @brief mode 1
     * threshold 的正负为判断符，正为大于threshold 负为小于threshold
     * threshold<0  ad值小于|threshold| 在线
     * threshold>0 ad值大于threshold 在线
     */
    int threshold;
    int value;//读取io 状态/ad 值
    u32 ch;//mode 为1 时需要ad ch
    bool old_state;//上一次状态
    bool cur_state;//当前状态
    bool on_line;//是否在线
    u8 filter_n;//滤波次数
    u8 cn;//计数

    u32 time;//scan time
    u16 id;//timer id
    // u32 ktime;//维持时间
    u16 kid;//维持id
    void (*func)(void *priv);

}IO_CHECK;

/**
 * @brief 检测初始化
 * 
 * @param check_io 
 * @return int 0：成功 其他：失败
 */
int u_check_io_init(IO_CHECK *check_io);

/**
 * @brief 关闭检测
 * 
 * @param check_io 
 */
void u_check_io_off(IO_CHECK *check_io);
#endif