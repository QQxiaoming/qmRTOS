/*
******************************************************************************
* Copyright (c) 2017,学生开放实验室
* All rights reserved.
******************************************************************************
* 文件名称： qTimer.c
* 文件标识： OS
* 摘 要： 软件定时器模块函数定义
******************************************************************************
* 当前版本： 1.0
* 作 者： Quard
* 完成日期： 2017年7月20日
******************************************************************************
*/
#ifndef QTIMER_H
#define QTIMER_H

#include "qEvent.h"

typedef enum _qTimerState
{
	qTimerCreated,                  //已经创建
	qTimerStarted,                  //已经开始
	qTimerRunning,                  //已经运行
	qTimerStopped,                  //已经停止
	qTimerDestroyed                 //已经删除
}qTimerState;

typedef struct _qTimer
{
	qNode linkNode;                 //定时器连接结点
	uint32_t stsrtDelayTicks;       //初始延时计数
	uint32_t durationTicks;         //周期计数值
	uint32_t delayTicks;            //当前计数值
	void (*timerFunc) (void * arg); //回调处理函数
	void * arg;                     //回调函数参数
	uint32_t config;                //定时器配置参数
	qTimerState state;              //定时器状态
}qTimer;

typedef struct _qTimerInfo
{
	uint32_t stsrtDelayTicks;       //初始延时计数
	uint32_t durationTicks;         //周期计数值
	void (*timerFunc) (void * arg); //回调处理函数
	void * arg;                     //回调函数参数
	uint32_t config;                //定时器配置参数
	qTimerState state;              //定时器状态
}qTimerInfo;

#define TIMER_CONFIG_TYPE_HARD        (1 << 0)     //中断定时器
#define TIMER_CONFIG_TYPE_SOFT        (0 << 0)     //任务定时器

void qTimerInit(qTimer * timer, uint32_t delayTicks, uint32_t durationTicks,
				void (*timerFunc) (void * arg), void * arg, uint32_t config);

void qTimerStart(qTimer * timer);
void qTimerStop(qTimer * timer);
void qTimerDestroy(qTimer * timer);
void qTimerGetInfo(qTimer * timer, qTimerInfo * Info);

void qTimerModuieTickNotify(void);
void qTimerModuleInit(void);

#endif /*QTIMER_H*/

/************** (C) COPYRIGHT 2014-2018 学生开放实验室 *****END OF FILE*********/
