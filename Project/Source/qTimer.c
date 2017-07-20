/*
******************************************************************************
* Copyright (c) 2017,学生开放实验室
* All rights reserved.
******************************************************************************
* 文件名称： qTimer.h
* 文件标识： OS
* 摘 要： 软件定时器模块函数声明
******************************************************************************
* 当前版本： 1.0
* 作 者： Quard
* 完成日期： 2017年7月20日
******************************************************************************
*/
#include "qmRTOS.h"

/******************************************************************************
 * 函数名称：软定时器初始化函数
 * 函数功能：初始化软定时器
 * 输入参数：qTimer * timer                    定时器结构指针
			uint32_t delayTicks               延时时间
			uint32_t durationTicks            当前延时时间
			void (*timerFunc) (void * arg)    定时器处理函数
			void * arg                        处理函数参数
			uint32_t config                   定时器配置
 * 输出参数：无
 ******************************************************************************/
void qTimerInit(qTimer * timer, uint32_t delayTicks, uint32_t durationTicks,
				void (*timerFunc) (void * arg), void * arg, uint32_t config)
{
	qNodeInit(&timer->linkNode);           //初始化连接结点
	timer->stsrtDelayTicks = delayTicks;   //初始计数值
	timer->durationTicks = durationTicks;  //周期计数参数
	timer->timerFunc = timerFunc;          //处理函数入口
	timer->arg = arg;                      //处理函数参数
	timer->config = config;                //配置参数
	
	if(delayTicks == 0)        //判断延时时间是否为0
	{
		timer->delayTicks = durationTicks;  //重载
	}
	else
	{
		timer->delayTicks = timer->stsrtDelayTicks;  //否则为初值
	}
	timer->state = qTimerCreated;           //标志状态为已创建
}

/************** (C) COPYRIGHT 2014-2018 学生开放实验室 *****END OF FILE*********/
