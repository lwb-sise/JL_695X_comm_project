#include "device/key_driver.h"
#include "system/event.h"
#include "system/init.h"
#include "slidekey.h"
#include "asm/power_interface.h"

#include "u_wireless.h"

static SEND_LIN *head=NULL;

#ifndef NULL
#define NULL    	(void *)0
#endif

// 增加节点
SEND_LIN * u_addNode(u8* buff){
    if(!buff)return NULL;

    SEND_LIN *newNode = (SEND_LIN*)malloc(sizeof(SEND_LIN));
    newNode->buff = malloc(buff[0]);
    newNode->len = buff[0];
    newNode->next = NULL;
    
    if(!newNode || !newNode->buff){
        if(!newNode->buff)free(newNode->buff);
        if(!newNode)free(newNode);
        return NULL;
    }

    memcpy(newNode->buff,buff,buff[0]);

    if (head == NULL) {
        head = newNode;
    } else {
        SEND_LIN *temp = head;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = newNode;
    }
    return newNode;
}

// 删除节点
void u_deleteNode(SEND_LIN *del){
    if (head == NULL || !del) {
        return;
    }

    SEND_LIN *temp = head;
    SEND_LIN *prev = NULL;

    if (temp != NULL && temp == del) {
        head = temp->next;
        if(temp->buff)free(temp->buff);
        if(temp)free(temp);
        return;
    }

    while (temp != NULL && temp != del) {
        prev = temp;
        temp = temp->next;
    }

    if (temp == NULL) {
        return;
    }

    prev->next = temp->next;
    if(temp->buff)free(temp->buff);
    if(temp)free(temp);
}

/**
 * @brief 删除所有节点
 * 
 */
void u_deleteNodeall(void){
    while(head != NULL){
        u_deleteNode(head);
    }
}

// 查找节点
SEND_LIN* u_findNode(SEND_LIN *check) {
    SEND_LIN *temp = head;
    while (temp != NULL) {
        // printf("------cur=%x next=%x,",temp,temp->next);
        if (temp == check) {
            return temp;
        }
        temp = temp->next;
    }
    return NULL;
}

// 获取一个节点
SEND_LIN* u_getNode(void){
    SEND_LIN *temp = head;
    while (temp && temp->next) {
        temp = temp->next;
    }
    return head;
}

/**
 * @brief 获取节点长度
 * 
 * @return u16 
 */
u16 u_getNode_len(void){
    SEND_LIN *temp = head;
    u16 len = 0;
    while (temp != NULL) {
        len++;
        temp = temp->next;
    }
    return len;
}