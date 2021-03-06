/*
******************************************************************************
* Copyright (c) 2017,学生开放实验室
* All rights reserved.
******************************************************************************
* 文件名称： qMbox.h
* 文件标识： OS
* 摘 要： 邮箱函数声明
******************************************************************************
* 当前版本： 1.0
* 作 者： Quard
* 完成日期： 2017年7月18日
******************************************************************************
*/
#ifndef QMBOX_H
#define QMBOX_H

#include "qEvent.h"

#define qMBoxSendNormal    0x00        //写入方式：普通写入
#define qMBoxSendFront      0x01        //高优先级写入


typedef struct _qMbox
{
	qEvent event;          //事件结构
	uint32_t count;        //计数器
	uint32_t read;         //消息读索引
	uint32_t write;        //消息写索引
	uint32_t maxCount;     //最大数量
	void ** msgBuffer;     //二级指针，指向存放消息的地址,数据缓存区
}qMbox;

typedef struct _qMboxInfo
{
	uint32_t count;        //计数器
	uint32_t maxCount;     //最大数量
	uint32_t taskCount;    //等待的任务数量
}qMboxInfo;

void qMboxInit(qMbox * mbox, void **msgBuffer, uint32_t maxCount);
uint32_t qMboxWait(qMbox * mbox, void **msg, uint32_t waitTicks);
uint32_t qMboxNoWaitGet(qMbox * mbox, void **msg);
uint32_t qMboxNotify(qMbox * mbox, void * msg, uint32_t notifyOption);
void qMboxFlush(qMbox * mbox);
uint32_t qMboxDestroy(qMbox * mbox);
void qMboxGetInfo (qMbox * mbox, qMboxInfo * info);
#endif /*QMBOX_H*/

/************** (C) COPYRIGHT 2014-2018 学生开放实验室 *****END OF FILE*********/
