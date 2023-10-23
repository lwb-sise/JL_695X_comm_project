#include "user_head.h"


#define LOG_TAG_CONST        U_CHECK
#define LOG_TAG             "[U_CHECK]"
#define LOG_ERROR_ENABLE
#define LOG_DEBUG_ENABLE
#define LOG_INFO_ENABLE
#define LOG_DUMP_ENABLE
#define LOG_CLI_ENABLE
#include "debug/user_debug.h"

/**
 * @brief 高低电平判断
 * 
 * @param check_io 
 * @return true 在线
 * @return false 下线
 */
static bool io_check_diagnosis(IO_CHECK *check_io){
    check_io->value = gpio_read(check_io->io.pro);

    if(check_io->threshold){
        if(check_io->value){
            return true;
        }
    }else if(!check_io->value){
        return true;
    }

    return false;
}

/**
 * @brief ad判断
 * 
 * @param check_io 
 * @return true 在线
 * @return false 下线
 */
static bool ad_check_diagnosis(IO_CHECK *check_io){
    check_io->value = adc_get_value(check_io->ch);
    
    log_debug("---check io ch=%d ad=%d",check_io->ch,check_io->value);
    if(check_io->threshold<0){
        if(check_io->value<(-check_io->threshold)){
            return true;
        }
    }else if(check_io->value>check_io->threshold){
        return true;
    }

    return false;
}

/**
 * @brief 检测扫描
 * 
 * @param check_io 
 */
static void check_scan(IO_CHECK *check_io){
    if(!check_io->en)return;
    
    if(IO_CHECK_MODE_IO==check_io->mode){
        check_io->on_line=io_check_diagnosis(check_io);
    }else{
        check_io->on_line=ad_check_diagnosis(check_io);
    }

    if(check_io->on_line!=check_io->old_state){
        check_io->cn++;
    }else{
        check_io->cn=0;
    }

    if(check_io->cn>check_io->filter_n){
        check_io->old_state=check_io->on_line;
        check_io->cn=0;
        if(check_io->func ){
            check_io->func(check_io);
        }
    }
}

/**
 * @brief io检测初始化
 * 
 * @param check_io 
 * @return int 0：成功 其他：失败
 */
static int ad_check_init(IO_CHECK *check_io){
    log_debug("gpio check mode ad init");
    GPIO_CTL ctl=USER_GPIO_IN_AD;
    if(IO_CHECK_UP==check_io->up_dow){
        ctl=USER_GPIO_IN_AD_PU;
    }else if(IO_CHECK_DOW==check_io->up_dow){
        ctl=USER_GPIO_IN_AD_PD;
    }

    u_comm_gpio_set(check_io->io.pro,ctl,0);

    int ch=u_comm_get_gpio_ad_ch(check_io->io.pro);
    if(ch<0){
        log_error("get gpio ad ch error");
        return -1;
    }
    check_io->ch=ch;
    adc_add_sample_ch(check_io->ch);

    return 0;
}

/**
 * @brief ad检测初始化
 * 
 * @param check_io 
 * @return int 0：成功 其他：失败
 */
static int io_check_init(IO_CHECK *check_io){
    log_debug("gpio check mode io init");

    GPIO_CTL ctl=USER_GPIO_IN_IO;
    if(IO_CHECK_UP==check_io->up_dow){
        ctl=USER_GPIO_IN_IO_PU;
    }else if(IO_CHECK_DOW==check_io->up_dow){
        ctl=USER_GPIO_IN_IO_PD;
    }

    u_comm_gpio_set(check_io->io.pro,ctl,0);
    return 0;
}

/**
 * @brief 检测初始化
 * 
 * @param check_io 
 * @return int 0：成功 其他：失败
 */
int u_check_io_init(IO_CHECK *check_io){
    int ret=-1;

    if(!check_io){
        return ret;
    }

    if(check_io->name){
        log_debug("gpio check io name=%s\r\n",check_io->name);
    }

    u_comm_gpio_set(check_io->io.mult,USER_GPIO_HI,0);

    if(IO_CHECK_MODE_IO==check_io->mode){
        log_debug("gpio check mode io");
        ret=io_check_init(check_io);
    }else{
        log_debug("gpio check mode ad");
        ret=ad_check_init(check_io);
    }

    if(check_io->time>0){
        log_debug("check timer=%d",check_io->time);
        check_io->id=my_time_add(check_io,check_scan,check_io->time);
    }
    return ret;
}

/**
 * @brief 关闭检测
 * 
 * @param check_io 
 */
void u_check_io_off(IO_CHECK *check_io){
    if(check_io->id){
        my_time_del(check_io->id);
        check_io->id=0;
    }
}