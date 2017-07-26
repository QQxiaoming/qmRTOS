/*
******************************************************************************
* Copyright (c) 2017,学生开放实验室
* All rights reserved.
******************************************************************************
* 文件名称： qmRTOS.h
* 文件标识： OS
* 摘 要： OS函数声明
******************************************************************************
* 当前版本： 1.0
* 作 者： Quard
* 完成日期： 2017年7月18日
******************************************************************************
*/
#ifndef QMRTOS_H
#define QMRTOS_H

#include <stdint.h>

#include "qConfig.h"
#include "qLib.h"
#include "qTask.h"
#include "qEvent.h"
#include "qSem.h"
#include "qMbox.h"
#include "qMemBlock.h"
#include "qFlagGroup.h"
#include "qMutex.h"
#include "qTimer.h"
#include "qHooks.h"

#include "Debug/Debug.h"

#define   TICK_PER_SEC         (1000 / QMRTOS_SYSTICK_MS)

typedef enum _qError{          //错误码
	qErrorNoError,
	qErrorTimeout,
	qErrorResourceUnavaliable,
	qErrorDel,
	qErrorResourceFull,
	qErrorOwner
}qError;

extern qTask * currentTask;   
extern qTask * nextTask;     

void qTaskRunFirst (void);
void qTaskSwitch (void);

uint32_t qTaskEnterCritical(void);
void qTaskExitCritical(uint32_t status);

void qTaskSchedInit(void);
void qTaskSchedDisable(void);
void qTaskSchedEnable(void);
void qTaskSchedRdy(qTask * task);
void qTaskSchedUnRdy(qTask * task);
void qTaskSchedRemove(qTask * task);
void qTaskSched(void);
void qTimeTaskWait(qTask * task, uint32_t ticks);
void qTimeTaskWakeUp(qTask * task);
void qTimeTaskRomove(qTask * task);
void qTaskSystemTickHandler(void);
void qTaskDelay(uint32_t delay);


void qSetSysTickPeriod(uint32_t ms);
void qInitApp(void);

float qCpuUsageGet(void);

#endif /*QMRTOS_H*/

/************** (C) COPYRIGHT 2014-2018 学生开放实验室 *****END OF FILE*********/
