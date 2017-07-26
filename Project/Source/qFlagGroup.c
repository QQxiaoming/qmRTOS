/*
******************************************************************************
* Copyright (c) 2017,学生开放实验室
* All rights reserved.
******************************************************************************
* 文件名称： qFlagGroup.c
* 文件标识： OS
* 摘 要： 事件标志组模块函数定义
******************************************************************************
* 当前版本： 1.0
* 作 者： Quard
* 完成日期： 2017年7月18日
******************************************************************************
*/
#include "qmRTOS.h"

#if QMRTOS_ENABLE_FLAGGROUP 
/******************************************************************************
 * 函数名称：事件标志组初始化函数
 * 函数功能：初始化事件标志组
 * 输入参数：qFlagGroup * flagGroup      事件标志组结构指针
			uint32_t flags              初始事件标志
 * 输出参数：无
 ******************************************************************************/
void qFlagGroupInit(qFlagGroup * flagGroup, uint32_t flags)
{
	qEventInit(&flagGroup->event, qEventTypeFlagGroup);  //初始化事件控制块
	flagGroup->flags = flags;                             //赋值标志组
}

/******************************************************************************
 * 函数名称：检查事件标志组函数
 * 函数功能：检查事件标志组是否满足
 * 输入参数：qFlagGroup * flagGroup      事件标志组结构指针
			uint32_t type               等待的类型（宏定义中的情况）
			uint32_t flags              任务的事件标志
 * 输出参数：返回错误情况
 ******************************************************************************/
static uint32_t qFlagGroupCheckAndConsume(qFlagGroup * flagGroup, uint32_t type, uint32_t * flags)
{
	uint32_t srcFlags = *flags;                             //取出事件标志
	uint32_t isSet = type & QFLAGGROUP_SET;                 //取出是否需要的是设置或清除标志
	uint32_t isAll = type & QFLAGGROUP_ALL;                 //取出是否需要的是全部或任意标志
	uint32_t isConsume = type & QFLAGGROUP_CONSUME;         //取出是否会消耗

	uint32_t calcFlag = isSet ? (flagGroup->flags & srcFlags) : (~flagGroup->flags & srcFlags); //有哪些类型的标志位出现
																								//flagGroup->flags & flags：计算出哪些位为1
																								//~flagGroup->flags & flags:计算出哪位为0

	if (((isAll != 0) && (calcFlag == srcFlags)) || ((isAll == 0) && (calcFlag != 0)))  //判断所有标志位出现, 或者做任意标志位出现，满足条件
	{
		if (isConsume)                         //是否消耗掉标志位
		{
			if (isSet)                         //是否是需要设置标志
			{
				flagGroup->flags &= ~srcFlags; //清除为1的标志位，变成0
			}
			else 
			{
				flagGroup->flags |= srcFlags;  //清除为0的标志位，变成1
			}
		}
		*flags = calcFlag;                     //将新标志返回
		return qErrorNoError;                  //返回没有错误
	}
	*flags = calcFlag;                         //将新标志返回
	return qErrorResourceUnavaliable;          //返回资源不可用
}

/******************************************************************************
 * 函数名称：事件标志组等待函数
 * 函数功能：事件标志等待操作
 * 输入参数：qFlagGroup * flagGroup      事件标志组结构指针
			uint32_t waitType           等待事件类型
			uint32_t requestFlag        等待的标志
			uint32_t * resultFlag       最终等待到了那些标志
			uint32_t waitTicks          等待超时时间
 * 输出参数：返回错误情况
 ******************************************************************************/
uint32_t qFlagGroupWait(qFlagGroup * flagGroup, uint32_t waitType, uint32_t requestFlag, uint32_t * resultFlag, uint32_t waitTicks)
{
	uint32_t result;
	uint32_t flags = requestFlag;
	uint32_t status = qTaskEnterCritical();          //进入临界区
	
	result = qFlagGroupCheckAndConsume(flagGroup, waitType, &flags);  //进行检查
	if(result != qErrorNoError)  //判断是否不正确正确
	{
		currentTask->waitFlagsType = waitType; //如果不正确，则赋值当前任务的等待类型和等待标志
		currentTask->eventFlags = requestFlag;
		qEventWait(&flagGroup->event, currentTask, (void *)0, qEventTypeFlagGroup, waitTicks);  //将任务加入等待队列
		
		qTaskExitCritical(status);                       //退出临界区
		
		qTaskSched();        //进行调度
		
		*resultFlag = currentTask->eventFlags;       //取回等待的标志
		result = currentTask->waitEventResult;       //取出返回结果
	}
	else
	{
		*resultFlag = flags;                         //如果正确，那么直接将事件标志返回
		qTaskExitCritical(status);                       //退出临界区
	}
	return result;       //返回错误情况
}
/******************************************************************************
 * 函数名称：检查事件标志（无等待）函数
 * 函数功能：检查事件标志
 * 输入参数：qFlagGroup * flagGroup      事件标志组结构指针
			uint32_t waitType           等待事件类型
			uint32_t requestFlag        等待的标志
			uint32_t * resultFlag       最终等待到了那些标志
 * 输出参数：返回错误情况
 ******************************************************************************/
uint32_t qFlagGroupNoWaitGet(qFlagGroup * flagGroup, uint32_t waitType, uint32_t requestFlag, uint32_t * resultFlag)
{
	uint32_t flags = requestFlag;
	uint32_t status = qTaskEnterCritical();          //进入临界区
	
	uint32_t result = qFlagGroupCheckAndConsume(flagGroup, waitType, &flags);  //进行检查

	qTaskExitCritical(status);                       //退出临界区
	
	*resultFlag = flags;       //取回事件标志
	return qErrorNoError;      //返回没有错误
}

/******************************************************************************
 * 函数名称：事件标志组通知函数
 * 函数功能：通知事件标志组事件发生
 * 输入参数：qFlagGroup * flagGroup      事件标志组结构指针
			uint8_t isSet
			uint32_t flags              初始事件标志
 * 输出参数：无
 ******************************************************************************/
void qFlagGroupNotify(qFlagGroup * flagGroup, uint8_t isSet, uint32_t flags)
{
	qList * waitList;          //定义链表指针
	qNode * node;              //定义结点变量
	qNode * nextNode;
	uint8_t sched = 0;
	uint32_t status = qTaskEnterCritical();          //进入临界区
	
	if(isSet)      //判断是否是设置标志
	{
		flagGroup->flags |= flags;    //如果是则进行设置
	}
	else
	{
		flagGroup->flags &= ~flags;    //否则进行清零
	}

	waitList = &flagGroup->event.waitList;     //取出链表地址
	for(node = waitList->headNode.nextNode; node != &(waitList->headNode); node = nextNode)  //遍历链表中的各个结点
	{
		uint32_t result;
		qTask * task = qNodeParent(node, qTask, linkNode);        //取出对应结点任务的指针
		uint32_t flags = task->eventFlags;                        //将任务标志赋值给标志
		nextNode = node->nextNode;                                //指向下一结点
        
        result = qFlagGroupCheckAndConsume(flagGroup, task->waitFlagsType, &flags);   // 检查标志
        if (result == qErrorNoError)                              //判断是否正确
		{
            task->eventFlags = flags;                              //设置标志
			qEventWakeUpTask(&flagGroup->event, task, (void *)0, qErrorNoError);   // 唤醒任务
            sched = 1;                              //是否需要调度标记
        }
    }
    if (sched)                                       // 如果有任务就绪，则执行一次调度
    { 
        qTaskSched();
    }
	qTaskExitCritical(status);                       //退出临界区
}

/******************************************************************************
 * 函数名称：事件标志组删除函数
 * 函数功能：删除事件标志组
 * 输入参数：qFlagGroup * flagGroup      事件标志组结构指针
 * 输出参数：返回任务数量
 ******************************************************************************/
uint32_t qFlagGroupDestroy (qFlagGroup * flagGroup)
{
    uint32_t status = qTaskEnterCritical();          //进入临界区

    uint32_t count = qEventRemoveAll(&flagGroup->event, (void *)0, qErrorDel);  //清空事件控制块中的任务

    qTaskExitCritical(status);                       //退出临界区

    if (count > 0)      //判断是否有任务就绪
    {
        qTaskSched();   //如果有则调度
    }
    return count;       //删除的任务数
}

/******************************************************************************
 * 函数名称：事件标志组通知函数
 * 函数功能：通知事件标志组事件发生
 * 输入参数：qFlagGroup * flagGroup      事件标志组结构指针
			qFlagGroupInfo * info       事件标志组查询结构指针
 * 输出参数：无
 ******************************************************************************/
void qFlagGroupGetInfo (qFlagGroup * flagGroup, qFlagGroupInfo * info)
{
    uint32_t status = qTaskEnterCritical();    //进入临界区


    info->flags = flagGroup->flags;            //拷贝需要的信息
    info->taskCount = qEventWaitCount(&flagGroup->event);

    qTaskExitCritical(status);                 //退出临界区
}

#endif /*QMRTOS_ENABLE_FLAGGROUP*/
/************** (C) COPYRIGHT 2014-2018 学生开放实验室 *****END OF FILE*********/
