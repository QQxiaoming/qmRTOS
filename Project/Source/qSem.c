/*
******************************************************************************
* Copyright (c) 2017,学生开放实验室
* All rights reserved.
******************************************************************************
* 文件名称： qSem.c
* 文件标识： OS
* 摘 要： 计数信号量模块函数定义
******************************************************************************
* 当前版本： 1.0
* 作 者： Quard
* 完成日期： 2017年7月18日
******************************************************************************
*/
#include "qmRTOS.h"

/******************************************************************************
 * 函数名称：计数信号量初始化函数
 * 函数功能：初始化计数信号量
 * 输入参数：qSem * sem              计数信号量结构指针
			uint32_t startCount     初始计数值
			uint32_t maxCount		最大计数值
 * 输出参数：无 
 ******************************************************************************/
void qSemInit(qSem * sem, uint32_t startCount, uint32_t maxCount)
{
	qEventInit(&sem->event, qEventTypeSem);  //初始化事件块
	
	sem->maxCount = maxCount;     //设置最大计数值
	if(maxCount == 0)             //判断最大值是否为0
	{
		sem->count = startCount;  //如果等于0，则表示没有上限，设置初始计数值
	}
	else
	{
		sem->count = (startCount > maxCount) ? maxCount : startCount;//否则将起始值和最大值中较大的一个设置为起始值
	}
}

/******************************************************************************
 * 函数名称：任务等待计数信号量函数
 * 函数功能：任务等待计数信号量或消耗资源
 * 输入参数：qSem * sem              计数信号量结构指针
			uint32_t waitTicks      等待的时间
 * 输出参数：错误代码
 ******************************************************************************/
uint32_t qSemWait(qSem * sem, uint32_t waitTicks)
{
	uint32_t status = qTaskEnterCritical();          //进入临界区
		
	if(sem->count > 0)  //判断计数值是否大于0
	{ 
		--sem->count;    //如果大于0，则减1，表示资源被消耗
		qTaskExitCritical(status);                       // 退出临界区
		return qErrorNoError;      //返回无错误
	}
	else
	{
		qEventWait(&sem->event, currentTask, (void *)0, qEventTypeSem, waitTicks);  //将当然任务加入等待
		qTaskExitCritical(status);                       // 退出临界区
		
		qTaskSched();   //进行任务调度
		return currentTask->waitEventResult;        //返回当前任务的处理结果
	}
}

/******************************************************************************
 * 函数名称：无等待计数信号量处理函数
 * 函数功能：任务消耗资源，无等待
 * 输入参数：qSem * sem              计数信号量结构指针
 * 输出参数：错误代码
 ******************************************************************************/
uint32_t qSemNoWaitGet(qSem * sem)
{
	uint32_t status = qTaskEnterCritical();          //进入临界区
		
	if(sem->count > 0)  //判断计数值是否大于0
	{ 
		--sem->count;    //如果大于0，则减1，表示资源被消耗
		qTaskExitCritical(status);                       // 退出临界区
		return qErrorNoError;      //返回无错误
	}
	else
	{
		qTaskExitCritical(status);                       // 退出临界区
		return qErrorResourceUnavaliable;              //返回资源不可用
	}
}

/******************************************************************************
 * 函数名称：通知计数信号量函数
 * 函数功能：资源释放通知计数信号量
 * 输入参数：qSem * sem              计数信号量结构指针
 * 输出参数：无
 ******************************************************************************/
void qSemNotify(qSem * sem)
{
	uint32_t status = qTaskEnterCritical();          //进入临界区
	
	if(qEventWaitCount(&sem->event) > 0)  //判断是否有任务在等待
	{
		qTask * task = qEventWakeUp(&sem->event, (void *)0, qErrorNoError);   //如果有任务则将其唤醒
		if(task->prio > currentTask->prio)  //判断唤醒的任务是否优先级高于当前任务
		{
			qTaskSched();     //如果是，则调度
		}
	}
	else                                    //否则信号量计数器加1
	{
		++sem->count;
		if((sem->maxCount != 0) && (sem->count > sem->maxCount))     //判断是否超过上限
		{
			sem->count = sem->maxCount;          //如果超过了，则赋值为最大值
		}
	}
	qTaskExitCritical(status);                       // 退出临界区
}

/******************************************************************************
 * 函数名称：计数信号量清空函数
 * 函数功能：删除计数信号量
 * 输入参数：qSem * sem              计数信号量结构指针
 * 输出参数：被删除的个数
 ******************************************************************************/
uint32_t qSemDestroy(qSem * sem)
{
	uint32_t status = qTaskEnterCritical();          //进入临界区
	
	uint32_t count = qEventRemoveAll(&sem->event, (void *)0, qErrorDel);     //删除所有任务
	sem->count = 0;            //清零计数器
	
	qTaskExitCritical(status);                       //退出临界区
	
	if(count > 0)  //判断是否有任务被移除
	{  
		qTaskSched();     //如果有则进行任务调度
	}
	
	return count;         //返回被删除的个数
}
	
/******************************************************************************
 * 函数名称：计数信号量状态查询函数
 * 函数功能：查询计数信号量
 * 输入参数：qSem * sem              计数信号量结构指针
			qSemInfo * info         计数信号量查询状态结构指针
 * 输出参数：无
 ******************************************************************************/
void qSemGetInfo(qSem * sem, qSemInfo * info)
{
	uint32_t status = qTaskEnterCritical();          //进入临界区
	
	info->count = sem->count;         //拷贝相关参数
	info->maxCount = sem->maxCount;
	info->taskCount = qEventWaitCount(&sem->event);
	
	qTaskExitCritical(status); 
}

/************** (C) COPYRIGHT 2014-2018 学生开放实验室 *****END OF FILE*********/
