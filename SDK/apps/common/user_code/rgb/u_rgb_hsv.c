#include "user_head.h"

#define LOG_TAG_CONST        U_RGB_HSV
#define LOG_TAG             "[U_RGB_HSV]"
#define LOG_ERROR_ENABLE
#define LOG_DEBUG_ENABLE
#define LOG_INFO_ENABLE
#define LOG_DUMP_ENABLE
#define LOG_CLI_ENABLE
#include "debug/user_debug.h"

int hsv2rgb(const COLOUR_HSV *hsv,COLOUR_RGB *rgb){
    // COLOUR_RGB rgb={0};

    if(!hsv || !rgb){
        return -1;
    }
    if((!(hsv->h) && !(hsv->s) && !(hsv->v) )){
        // memset(rgb,0,sizeof(COLOUR_RGB));
        rgb->r =0;
        rgb->g =0;
        rgb->b =0;
        return 0;
    }

    // float h = ofb_h(hsv->h)*(MAX_H_F/MAX_H)*1.0f;
    float h = (ofb_h(hsv->h)*MAX_H_F)/MAX_H*1.0f;
    float s = ofb_s(hsv->s)  / MAX_S_F;
    float v = ofb_v(hsv->v)  / MAX_S_F;
    float r, g, b; // 0.0-1.0
    int   hi = (int)(h / (MAX_H_F/6)) % 6;
    float f  = (h / (MAX_H_F/6)) - hi;
    float p  = v * (1.0f - s);
    float q  = v * (1.0f - s * f);
    float t  = v * (1.0f - s * (1.0f - f));

    switch(hi) {
        case 0: r = v, g = t, b = p; break;
        case 1: r = q, g = v, b = p; break;
        case 2: r = p, g = v, b = t; break;
        case 3: r = p, g = q, b = v; break;
        case 4: r = t, g = p, b = v; break;
        case 5: r = v, g = p, b = q; break;
    }

    rgb->r = (unsigned char)(r * MAX_RGB_F); // dst_r : 0-255
    rgb->g = (unsigned char)(g * MAX_RGB_F); // dst_r : 0-255
    rgb->b = (unsigned char)(b * MAX_RGB_F); // dst_r : 0-255

    return 0;
}


int rgb2hsv(const COLOUR_RGB *rgb,COLOUR_HSV *hsv){

    if(!rgb->r && !rgb->g && !rgb->b){
        return -1;
    }
    float h = 0, s = 0, v = 0;
    float r = 0, g = 0, b = 0;
    r = rgb->r / MAX_RGB_F;
    g = rgb->g / MAX_RGB_F;
    b = rgb->b / MAX_RGB_F;
    float max = m_max(r, g, b);
    float min = m_min(r, g, b);
    float diff = max - min;
    if (max == min){
        h = 0;
    }else if(max == r){
        if(b > g){
            h = (((MAX_H_F/6) *((g - b) / (max-min))) + MAX_H_F);
        }else{
            h = ((MAX_H_F/6) *((g - b) / (max-min)));
        }
    }else if(max == g){
        h = (((MAX_H_F/6) *((b - r) / (max-min))) + (MAX_H_F/3));
    }else if(max == b){
        h = ( ((MAX_H_F/6) *((r - g) / (max-min))) + ((MAX_H_F*2)/3));
    }
    h = h / MAX_H_F;
    if (max == 0){
        s = 0;
    }else{
        s = ((max-min) / max);
    }
    v = max;

    hsv->h = h * MAX_H;
    hsv->s = s * MAX_S;
    hsv->v = v * MAX_V;
    return hsv;
}