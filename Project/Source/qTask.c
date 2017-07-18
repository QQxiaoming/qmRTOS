/*
******************************************************************************
* Copyright (c) 2017,学生开放实验室
* All rights reserved.
******************************************************************************
* 文件名称： qTask.c
* 文件标识： OS
* 摘 要： 任务管理模块函数定义
******************************************************************************
* 当前版本： 1.0
* 作 者： Quard
* 完成日期： 2017年7月18日
******************************************************************************
*/
#include "qmRTOS.h"

/******************************************************************************
 * 函数名称：任务初始化函数
 * 函数功能：任务初始化
 * 输入参数：qTask * task             任务结构指针
			void (*entry) (void *)   任务入口函数地址
			void *parm               传递给任务的参数地址
			uint32_t prio            任务优先级
			qTaskStack * stack       任务堆栈地址
 * 输出参数：无 
 ******************************************************************************/
void qTaskInit(qTask * task , void (*entry) (void *), void *param ,uint32_t prio, qTaskStack * stack )
{
	//堆栈内容初始化，传递末端的地址，内核按满递减方式增长。
	*(--stack) = (unsigned long)(1 << 24);              //XPSR, 设置了Thumb模式，恢复到Thumb状态而非ARM状态运行
	*(--stack) = (unsigned long)entry;                  //PC, 程序的入口地址
    *(--stack) = (unsigned long)0x14;                   //R14(LR), 任务不会通过return xxx结束自己，所以未用
    *(--stack) = (unsigned long)0x12;                   //R12, 未用
    *(--stack) = (unsigned long)0x3;                    //R3, 未用
    *(--stack) = (unsigned long)0x2;                    //R2, 未用
    *(--stack) = (unsigned long)0x1;                    //R1, 未用
    *(--stack) = (unsigned long)param;                  //R0 = param, 传给任务的入口函数
    *(--stack) = (unsigned long)0x11;                   //R11, 未用
    *(--stack) = (unsigned long)0x10;                   //R10, 未用
    *(--stack) = (unsigned long)0x9;                    //R9, 未用
    *(--stack) = (unsigned long)0x8;                    //R8, 未用
    *(--stack) = (unsigned long)0x7;                    //R7, 未用
    *(--stack) = (unsigned long)0x6;                    //R6, 未用
    *(--stack) = (unsigned long)0x5;                    //R5, 未用
    *(--stack) = (unsigned long)0x4;                    //R4, 未用
	
	task->slice = QMRTOS_SLTIC_MAX;                     //时间片初始化为最大值
	task->stack = stack;                                //保存最终的值
	task->delayTicks = 0;								//初始任务延时个数为0
	task->prio = prio;                                  //设置任务的优先级
	task->state = QMRTOS_TASK_STATE_RDY;                //设置任务为就绪状态
	task->suspendCount = 0;                             //初始挂起计数器为0
	task->clean = (void (*)(void *))0;                  //初始清理函数为空
	task->cleanParam = (void *)0;                       //初始清理函数的参数为空
    task->requestDeleteFlag = 0;                        //请求删除标记

    task->waitEvent = (qEvent *)0;                      //没有等待事件
    task->eventMsg = (void *)0;                         //没有等待事件
    task->waitEventResult = qErrorNoError;              //没有等待事件错误
	
	task->eventFlags = 0;                               //标志为空
	task->waitFlagsType = 0;                            //无类型
	
	qNodeInit(&(task->delayNode));                      //初始化延时结点
	qNodeInit(&(task->linkNode));                       //初始化链接结点
	
	qTaskSchedRdy(task);                                //将任务加入队列并就绪
}

/******************************************************************************
 * 函数名称：任务挂起函数
 * 函数功能：挂起任务
 * 输入参数：qTask * task             任务结构指针
 * 输出参数：无 
 ******************************************************************************/
void qTaskSuspend(qTask * task)
{
	uint32_t statue = qTaskEnterCritical();   //进入临界区

	if(!(task->state & QMRTOS_TASK_STATE_DELAYED))  //判断任务是否在延时状态
	{
		if(++task->suspendCount <= 1)               //判断任务挂起计数器是否大于1，并使得计数器+1
		{
			task->state |= QMRTOS_TASK_STATE_SUSPEND;  //如果计数器大于1，则将挂起标志置1
			qTaskSchedUnRdy(task);                     //将任务从就绪表删除
			if(task == currentTask)  //判断该任务是否是当然任务
			{
				qTaskSched();         //如果是当前任务，则进行调度
			}	
		}
	}
	
	qTaskExitCritical(statue);
}

/******************************************************************************
 * 函数名称：任务恢复函数
 * 函数功能：恢复挂起任务
 * 输入参数：qTask * task             任务结构指针
 * 输出参数：无 
 ******************************************************************************/
void qTaskWakeUp(qTask * task)
{
	uint32_t statue = qTaskEnterCritical();   //进入临界区

	if(task->state & QMRTOS_TASK_STATE_SUSPEND)   //判断任务是否被挂起
	{
		if(--task->suspendCount == 0)          //递减挂起计数，并判断是否为0
		{
			task->state &= ~QMRTOS_TASK_STATE_SUSPEND;   //清除挂起标志
			qTaskSchedRdy(task);                        //将任务就绪
			qTaskSched();                              //进行调度
		}
	}
	
	qTaskExitCritical(statue);
}

/******************************************************************************
 * 函数名称：任务清理回调函数设置函数
 * 函数功能：设置任务清理回调
 * 输入参数：qTask * task                  任务结构指针
			void (*clean)(void * param)   清理回调入口函数
			void * param                  回调函数参数
 * 输出参数：无 
 ******************************************************************************/
void qTaskSetCleanCallFunc(qTask * task, void (*clean)(void * param), void * param)
{
	task->clean = clean;        //对任务回调函数设置
	task->cleanParam = param;   //对参数设置
}

/******************************************************************************
 * 函数名称：任务强制删除函数
 * 函数功能：强制删除任务
 * 输入参数：qTask * task                  任务结构指针
 * 输出参数：无 
 ******************************************************************************/
void qTaskForceDelete(qTask * task)
{
	uint32_t statue = qTaskEnterCritical();   //进入临界区

	if(task->state & QMRTOS_TASK_STATE_DELAYED)  //判断任务是否是延时状态
	{
		qTimeTaskRomove(task);                   //如果在延时状态，则将其移除延时队列
	}
	else if(!(task->state & QMRTOS_TASK_STATE_SUSPEND))  //判断任务是否在挂起状态
	{
		qTaskSchedRemove(task);                  //如果不在挂起状态，则将其从优先级任务队列中移除
	}
	
	if(task->clean)              //判断是否有清理回调函数
	{
		task->clean(task->cleanParam);            //调用清理函数，注意此处写法
	}
	
	if(currentTask == task)   //判断该任务是否是当前任务
	{   
		qTaskSched();        //如果是则进行任务调度
	}
	
	qTaskExitCritical(statue);
}

/******************************************************************************
 * 函数名称：任务请求删除函数
 * 函数功能：设置请求删除标记
 * 输入参数：qTask * task                  任务结构指针
 * 输出参数：无 
 ******************************************************************************/
void qTaskRequestDelete(qTask * task)
{
	uint32_t statue = qTaskEnterCritical();   //进入临界区

	task->requestDeleteFlag = 1;        //设置任务请求删除标记
	
	qTaskExitCritical(statue);
}

/******************************************************************************
 * 函数名称：检查任务请求删除标记函数
 * 函数功能：检查任务请求删除标记
 * 输入参数：无
 * 输出参数：delete 请求标记 
 ******************************************************************************/
uint8_t qTaskIsRequestedDeleted(void)
{
	uint8_t delete;
	
	uint32_t statue = qTaskEnterCritical();   //进入临界区

	delete = currentTask->requestDeleteFlag;        //返回当前任务请求删除标记
	
	qTaskExitCritical(statue);
	
	return delete;
}

/******************************************************************************
 * 函数名称：任务删除自己函数
 * 函数功能：删除当前任务
 * 输入参数：无
 * 输出参数：无
 ******************************************************************************/
void qTaskDeleteSelf(void)
{
	uint32_t statue = qTaskEnterCritical();   //进入临界区

	qTaskSchedRemove(currentTask);   //将当然任务移除优先级就绪队列
	
	if(currentTask->clean)              //判断是否有清理回调函数
	{
		currentTask->clean(currentTask->cleanParam);            //调用清理函数，注意此处写法
	}
	
	qTaskSched();  //调用任务调度函数
	
	qTaskExitCritical(statue);
}

/******************************************************************************
 * 函数名称：任务状态查询函数
 * 函数功能：读取当前任务状态
 * 输入参数：无
 * 输出参数：无
 ******************************************************************************/
void qTaskGetInfo(qTask * task, qTaskInfo * info)
{
	uint32_t statue = qTaskEnterCritical();   //进入临界区

	info->delayTicks = task->delayTicks;      //拷贝当前任务状态
	info->prio = task->prio;
	info->slice = task->slice;
	info->state = task->state;
	info->suspendCount = task->suspendCount;
	
	qTaskExitCritical(statue);
}

/************** (C) COPYRIGHT 2014-2018 学生开放实验室 *****END OF FILE*********/
