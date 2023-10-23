#ifndef _U_RGB_HSV_H_
#define _U_RGB_HSV_H_

#define MAX_H	(360)//色调最大值
#define MAX_S	(255)//饱和度最大值
#define MAX_V	(255)//明度最大值

#define MAX_H_F	(360.0f)  //色调最大浮点值
#define MAX_S_F	(255.0f) //饱和度最大浮点值
#define MAX_V_F	(255.0f) //明度最大浮点值
#define MAX_RGB_F (255.0f)//R\G\B最大取值

//HSV
typedef struct _USER_HSV_COLOUR_{
   volatile int h;//色调（H）0°～360°
   volatile int s;//饱和度（S）0~255
   volatile int v;//明度（V）0~255
}COLOUR_HSV;

//RGB
typedef struct _RGB_COLOUR_{
    u8 g;//绿
    u8 r;//红
    u8 b;//蓝
}COLOUR_RGB;

typedef struct _RGB_COLOUR_INT_{
    int g;//绿
    int r;//红
    int b;//蓝
}RGB_COLOUR_INT;

#define m_min(a, b, c)  (((a) < (b))? ((a) < (c) ? (a) : (c)) : ((b) < (c) ? (b) : (c)));
#define m_max(a, b, c)  (((a) > (b))? ((a) > (c) ? (a) : (c)) : ((b) > (c) ? (b) : (c)));
#define ofb_h(a) ( (a) >= 0 ? ((a) % MAX_H) : (MAX_H + (a) % MAX_H) )
#define ofb_s(a) ( (a) < 0 ? 0: ((a) >= MAX_S ? MAX_S : (a)))
#define ofb_v(a) ( (a) < 0 ? 0: ((a) >= MAX_V ? MAX_V : (a)))


int hsv2rgb(const COLOUR_HSV *hsv,COLOUR_RGB *rgb);
int rgb2hsv(const COLOUR_RGB *rgb,COLOUR_HSV *hsv);
#endif//_HEAD_H_
