/*
******************************************************************************
* Copyright (c) 2017,学生开放实验室
* All rights reserved.
******************************************************************************
* 文件名称： qSem.h
* 文件标识： OS
* 摘 要： 计数信号量函数声明
******************************************************************************
* 当前版本： 1.0
* 作 者： Quard
* 完成日期： 2017年7月18日
******************************************************************************
*/
#ifndef QSEM_H
#define QSEM_H

#include "qEvent.h"

typedef struct _qSem
{
	qEvent event;
	uint32_t count;
	uint32_t maxCount;
}qSem;

typedef struct _qSemInfo
{
	uint32_t count;
	uint32_t maxCount;
	uint32_t taskCount;     //等待的任务个数
}qSemInfo;

void qSemInit(qSem * sem, uint32_t startCount, uint32_t maxCount);
uint32_t qSemWait(qSem * sem, uint32_t waitTicks);
uint32_t qSemNoWaitGet(qSem * sem);
void qSemNotify(qSem * sem);
uint32_t qSemDestroy(qSem * sem);
void qSemGetInfo(qSem * sem, qSemInfo * info);
	
#endif /*QSEM_H*/

/************** (C) COPYRIGHT 2014-2018 学生开放实验室 *****END OF FILE*********/
