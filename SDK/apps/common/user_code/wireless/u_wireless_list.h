#ifndef _H_U_WIRELESS_LIST_H_
#define _H_U_WIRELESS_LIST_H_

typedef struct _U_SEND_LIN_ {
    u8* buff;//数据
    u32 len;//数据长度
    // SEND_LIN *next;//下一个
    struct _U_SEND_LIN_ * next;//下一个
}SEND_LIN;

// 删除节点
void u_deleteNode(SEND_LIN *del);
// 增加节点
SEND_LIN * u_addNode(u8* buff);
// 查找节点
SEND_LIN* u_findNode(SEND_LIN *check);
// 获取一个节点
SEND_LIN* u_getNode(void);
/**
 * @brief 删除所有节点
 * 
 */
void u_deleteNodeall(void);
/**
 * @brief 获取节点长度
 * 
 * @return u16 
 */
u16 u_getNode_len(void);
#endif