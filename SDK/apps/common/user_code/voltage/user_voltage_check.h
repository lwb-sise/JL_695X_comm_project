//*********************************************************************************//
//						            外部电量检测	                                    //
//*********************************************************************************//



typedef struct _CHECK_VBAT_INFO_{
    bool en;//模块使能
    IO_INFO io;
    u32 ad_ch;//ad检测通道
    u32 sample_freq;//采样频率 ms

    u16 up;//靠近电池的电阻比值 值
    u16 dow;//靠近地的电阻比值 值
}CHECK_VBAT_INFO;


/**
 * @brief 电池电压检测初始化
 * 
 */
void u_check_vbat_init(void);
/**
 * @brief 获取电池电压
 * 
 * @param in_vbat 默认传入值
 * @return int 
 * err：in_vbat 其他：电池电压
 */
int u_get_vbat(int in_vbat);