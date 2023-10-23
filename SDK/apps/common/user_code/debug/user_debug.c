#include "user_head.h"


const bool log_tag_const_v_U_FUN AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);
const bool log_tag_const_i_U_FUN AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);
const bool log_tag_const_d_U_FUN AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);
const bool log_tag_const_w_U_FUN AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);
const bool log_tag_const_e_U_FUN AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);

const bool log_tag_const_v_U_COMM AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);
const bool log_tag_const_i_U_COMM AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);
const bool log_tag_const_d_U_COMM AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);
const bool log_tag_const_w_U_COMM AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);
const bool log_tag_const_e_U_COMM AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);

const bool log_tag_const_v_U_CHECK AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);
const bool log_tag_const_i_U_CHECK AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);
const bool log_tag_const_d_U_CHECK AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(FALSE);
const bool log_tag_const_w_U_CHECK AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);
const bool log_tag_const_e_U_CHECK AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);

const bool log_tag_const_v_U_INFO AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);
const bool log_tag_const_i_U_INFO AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);
const bool log_tag_const_d_U_INFO AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);
const bool log_tag_const_w_U_INFO AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);
const bool log_tag_const_e_U_INFO AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);

#ifdef U_LAMP_MODE_EN
const bool log_tag_const_v_U_RGB AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);
const bool log_tag_const_i_U_RGB AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);
const bool log_tag_const_d_U_RGB AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);
const bool log_tag_const_w_U_RGB AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);
const bool log_tag_const_e_U_RGB AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);

const bool log_tag_const_v_U_RGB_DRIVE AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);
const bool log_tag_const_i_U_RGB_DRIVE AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);
const bool log_tag_const_d_U_RGB_DRIVE AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);
const bool log_tag_const_w_U_RGB_DRIVE AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);
const bool log_tag_const_e_U_RGB_DRIVE AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);

const bool log_tag_const_v_U_RGB_HSV AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);
const bool log_tag_const_i_U_RGB_HSV AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);
const bool log_tag_const_d_U_RGB_HSV AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);
const bool log_tag_const_w_U_RGB_HSV AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);
const bool log_tag_const_e_U_RGB_HSV AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);

const bool log_tag_const_v_U_RGB_MID AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);
const bool log_tag_const_i_U_RGB_MID AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);
const bool log_tag_const_d_U_RGB_MID AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);
const bool log_tag_const_w_U_RGB_MID AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);
const bool log_tag_const_e_U_RGB_MID AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);

const bool log_tag_const_v_U_RGB_MODE AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);
const bool log_tag_const_i_U_RGB_MODE AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);
const bool log_tag_const_d_U_RGB_MODE AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);
const bool log_tag_const_w_U_RGB_MODE AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);
const bool log_tag_const_e_U_RGB_MODE AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);

const bool log_tag_const_v_U_RGB_FUN AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);
const bool log_tag_const_i_U_RGB_FUN AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);
const bool log_tag_const_d_U_RGB_FUN AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);
const bool log_tag_const_w_U_RGB_FUN AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);
const bool log_tag_const_e_U_RGB_FUN AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);
#endif

#ifdef U_PA_MODE_EN
const bool log_tag_const_v_U_PA AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);
const bool log_tag_const_i_U_PA AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);
const bool log_tag_const_d_U_PA AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);
const bool log_tag_const_w_U_PA AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);
const bool log_tag_const_e_U_PA AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);

const bool log_tag_const_v_U_PA_DRIVE AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);
const bool log_tag_const_i_U_PA_DRIVE AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);
const bool log_tag_const_d_U_PA_DRIVE AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);
const bool log_tag_const_w_U_PA_DRIVE AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);
const bool log_tag_const_e_U_PA_DRIVE AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);
#endif

#ifdef U_EFFECT_MODE_EN
const bool log_tag_const_v_U_EFFECT AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);
const bool log_tag_const_i_U_EFFECT AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);
const bool log_tag_const_d_U_EFFECT AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);
const bool log_tag_const_w_U_EFFECT AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);
const bool log_tag_const_e_U_EFFECT AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);
#endif

#ifdef U_SERIAL_MODE_EN
const bool log_tag_const_v_U_SERIAL AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);
const bool log_tag_const_i_U_SERIAL AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);
const bool log_tag_const_d_U_SERIAL AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);
const bool log_tag_const_w_U_SERIAL AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);
const bool log_tag_const_e_U_SERIAL AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);

const bool log_tag_const_v_U_SERIAL_CODE AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);
const bool log_tag_const_i_U_SERIAL_CODE AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);
const bool log_tag_const_d_U_SERIAL_CODE AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);
const bool log_tag_const_w_U_SERIAL_CODE AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);
const bool log_tag_const_e_U_SERIAL_CODE AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);
#endif

#ifdef U_VBAT_MODE_EN
const bool log_tag_const_v_U_CHECK_VBAT AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);
const bool log_tag_const_i_U_CHECK_VBAT AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);
const bool log_tag_const_d_U_CHECK_VBAT AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);
const bool log_tag_const_w_U_CHECK_VBAT AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);
const bool log_tag_const_e_U_CHECK_VBAT AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);
#endif

#ifdef U_MAINTENANCE_MODE_EN
const bool log_tag_const_v_U_MAINTENANCE_VOLTAGE AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);
const bool log_tag_const_i_U_MAINTENANCE_VOLTAGE AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);
const bool log_tag_const_d_U_MAINTENANCE_VOLTAGE AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);
const bool log_tag_const_w_U_MAINTENANCE_VOLTAGE AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);
const bool log_tag_const_e_U_MAINTENANCE_VOLTAGE AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);
#endif

#ifdef U_KNOB_MODE_EN
const bool log_tag_const_v_U_KNOB AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);
const bool log_tag_const_i_U_KNOB AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);
const bool log_tag_const_d_U_KNOB AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);
const bool log_tag_const_w_U_KNOB AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);
const bool log_tag_const_e_U_KNOB AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);
#endif


#ifdef U_SPECTRUM_EN
const bool log_tag_const_v_U_SPECTRUM AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);
const bool log_tag_const_i_U_SPECTRUM AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);
const bool log_tag_const_d_U_SPECTRUM AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);
const bool log_tag_const_w_U_SPECTRUM AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);
const bool log_tag_const_e_U_SPECTRUM AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);
#endif

#ifdef U_APP_EN
const bool log_tag_const_v_U_APP AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);
const bool log_tag_const_i_U_APP AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);
const bool log_tag_const_d_U_APP AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);
const bool log_tag_const_w_U_APP AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);
const bool log_tag_const_e_U_APP AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);
#endif

const bool log_tag_const_v_U_TWS AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);
const bool log_tag_const_i_U_TWS AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);
const bool log_tag_const_d_U_TWS AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);
const bool log_tag_const_w_U_TWS AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);
const bool log_tag_const_e_U_TWS AT(.LOG_TAG_CONST) = CONFIG_DEBUG_U(TRUE);