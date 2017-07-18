/*
******************************************************************************
* Copyright (c) 2017,学生开放实验室
* All rights reserved.
******************************************************************************
* 文件名称： qMemBlock.c
* 文件标识： OS
* 摘 要： 内存块模块函数定义
******************************************************************************
* 当前版本： 1.0
* 作 者： Quard
* 完成日期： 2017年7月18日
******************************************************************************
*/
#include "qmRTOS.h"

/******************************************************************************
 * 函数名称：存储块初始化函数
 * 函数功能：初始化存储块
 * 输入参数：qMemBlock * memBlock       存储块结构指针
			uint8_t * memStart         起始地址
			uint32_t blockSize         存储块大小
			uint32_t blockCnt          存储块数量
 * 输出参数：无 
 ******************************************************************************/
void qMemBlockInit(qMemBlock * memBlock, uint8_t * memStart, uint32_t blockSize, uint32_t blockCnt)
{
	uint8_t * memBlockStart = (uint8_t *)memStart;                    //存储块起始地址
	uint8_t * memBlockEnd = memBlockStart + blockSize * blockCnt;     //存储块结束地址

	if(blockSize < sizeof(qNode))      //判断存储块是否能存放结点
	{
		return;
	}
	
	qEventInit(&memBlock->event, qEventTypeMemBlock);   //初始化事件控制块
	memBlock->memStart = memStart;            //设置起始地址
	memBlock->blockSize = blockSize;          //设置存储块大小
	memBlock->maxCount = blockCnt;            //设置存储块数量
	
	qListInit(&memBlock->blockList);          //初始化存储块链表
	while(memBlockStart < memBlockEnd)        //判断存储空间是否合理
	{
		qNodeInit((qNode *)memBlockStart);    //将起始空间初始化成结点
		qListAddLast(&memBlock->blockList, (qNode *)memBlockStart);    //插入到存储块链表
		
		memBlockStart += blockSize;           //起始指针后移
	}
}

/******************************************************************************
 * 函数名称：存储块等待函数
 * 函数功能：任务需要存储块进入等待
 * 输入参数：qMemBlock * memBlock       存储块结构指针
			void **mem                 存储块地址
			uint32_t waitTicks         等待时间
 * 输出参数：返回错误代码
 ******************************************************************************/
uint32_t qMemBlockWait(qMemBlock * memBlock, uint8_t **mem, uint32_t waitTicks)
{
	uint32_t status = qTaskEnterCritical();          //进入临界区
	
	if(qListCount(&memBlock->blockList) > 0)    //判断存储块列表是否存在存储块
	{
		*mem = (uint8_t *)qListRemoveFirst(&memBlock->blockList); //取出一个存储块
		qTaskExitCritical(status);                   //退出临界区
		return qErrorNoError;      //返回没有错误
	}
	else
	{
		qEventWait(&memBlock->event, currentTask, (void *)0, qEventTypeMemBlock, waitTicks);//否则没有存储块，当前任务开始等待存储块
		qTaskExitCritical(status);                       //退出临界区

		qTaskSched();     //进行调度
		
		*mem = currentTask->eventMsg;          //任务取出存储块
		return currentTask->waitEventResult;   //返回等待结果
	}
}

/******************************************************************************
 * 函数名称：任务获取存储块无等待函数
 * 函数功能：任务需要存储块无等待
 * 输入参数：qMemBlock * memBlock       存储块结构指针
			void **mem                 存储块地址
 * 输出参数：返回错误代码
 ******************************************************************************/
uint32_t qMemBlockNoWaitGet(qMemBlock * memBlock, void **mem)
{
	uint32_t status = qTaskEnterCritical();          //进入临界区
	
	if(qListCount(&memBlock->blockList) > 0)    //判断存储块列表是否存在存储块
	{
		*mem = (uint8_t *)qListRemoveFirst(&memBlock->blockList); //取出一个存储块
		qTaskExitCritical(status);                   //退出临界区
		return qErrorNoError;      //返回没有错误
	}
	else
	{
		qTaskExitCritical(status);                       //退出临界区
		return qErrorResourceUnavaliable;   //返回资源不可用
	}
}

/******************************************************************************
 * 函数名称：释放存储块函数
 * 函数功能：释放存储块
 * 输入参数：qMemBlock * memBlock       存储块结构指针
			uint8_t *mem                 存储块地址
 * 输出参数：错误代码
 ******************************************************************************/
uint32_t qMemBlockNotify(qMemBlock * memBlock, uint8_t *mem)
{
	uint32_t status = qTaskEnterCritical();          //进入临界区

	if(qEventWaitCount(&memBlock->event) > 0)         //判断是否有任务在等待
	{
		qTask * task = qEventWakeUp(&memBlock->event, (void *)mem, qErrorNoError);  //将存储块传给任务
		if(task->prio < currentTask->prio)    //判断任务优先级
		{
			qTaskSched();         //如果释放的任务优先级高则调度
		}
	}
	else
	{
		qListAddLast(&memBlock->blockList, (qNode *)mem);   //否则将存储块插入队列
	}
	qTaskExitCritical(status);                   //退出临界区
	return qErrorNoError;           //返回没有错误
}

/******************************************************************************
 * 函数名称：删除存储块函数
 * 函数功能：删除存储块
 * 输入参数：qMemBlock * memBlock       存储块结构指针
 * 输出参数：删除的任务数
 ******************************************************************************/
uint32_t qMemBlockDestroy(qMemBlock * memBlock)
{
	uint32_t status = qTaskEnterCritical();          //进入临界区

	uint32_t count = qEventRemoveAll(&memBlock->event, (void *)0, qErrorDel);  //清空事件块所有的任务
	
	qTaskExitCritical(status);                   //退出临界区
	
	if(count > 0)              //判断是否删除了任务
	{
		qTaskSched();          //如果删除了等待任务则进行调度
	} 
	return count;              //返回删除的个数
}

/******************************************************************************
 * 函数名称：查询存储块状态信息函数
 * 函数功能：查询存储块状态信息
 * 输入参数：qMemBlock * memBlock       存储块结构指针
            qMemBlockInfo * info       邮箱状态查询结构指针
 * 输出参数：删除的任务数
 ******************************************************************************/
void qMemBlockGetInfo(qMemBlock * memBlock, qMemBlockInfo * info)
{
    uint32_t status = qTaskEnterCritical();

    // 拷贝需要的信息
    info->count = qListCount(&memBlock->blockList);
    info->maxCount = memBlock->maxCount;
	info->blockSize = memBlock->blockSize;
    info->taskCount = qEventWaitCount(&memBlock->event);

    qTaskExitCritical(status);
}

/************** (C) COPYRIGHT 2014-2018 学生开放实验室 *****END OF FILE*********/
