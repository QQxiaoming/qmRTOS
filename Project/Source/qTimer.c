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

#if QMRTOS_ENABLE_TIMER 

static qList qTimerHardList;       //硬中断中定时器列表
static qList qTimerSoftList;       //定时器任务中定时器列表
static qSem qTimerProtectSem;      //任务访问软定时器列表信号量
static qSem qTimerTickSem;         //通知定时器任务中断发生

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

/******************************************************************************
 * 函数名称：软定时器启动函数
 * 函数功能：启动软定时器
 * 输入参数：qTimer * timer                    定时器结构指针
 * 输出参数：无
 ******************************************************************************/
void qTimerStart(qTimer * timer)
{
	switch(timer->state)                   //判断状态
	{
		case qTimerCreated:                //如果定时器已经创建或停止，则启动
		case qTimerStopped:
			timer->delayTicks = timer->stsrtDelayTicks ? timer->stsrtDelayTicks : timer->durationTicks;   //判断起始是否为0，为0选择周期值，否则选择起始值
			timer->state = qTimerStarted;  //设置定时器已经启动
		    if(timer->config == TIMER_CONFIG_TYPE_HARD)    //判断定时器类型
			{
				uint32_t status = qTaskEnterCritical();          //进入临界区
				
				qListAddFirst(&qTimerHardList, &timer->linkNode);//将定时器加入硬定时器列表
				
				qTaskExitCritical(status);                       //退出临界区
			}
			else
			{
				qSemWait(&qTimerProtectSem, 0);                  //等待保护信号量
				
				qListAddFirst(&qTimerSoftList, &timer->linkNode);//将定时器加入软定时器列表
				
				qSemNotify(&qTimerProtectSem);                   //释放保护信号量
			}
			break;
		default:                           //否则不启动
			break;
	}
}

/******************************************************************************
 * 函数名称：软定时器停止函数
 * 函数功能：停止软定时器
 * 输入参数：qTimer * timer                    定时器结构指针
 * 输出参数：无
 ******************************************************************************/
void qTimerStop(qTimer * timer)
{
	switch(timer->state)                  //判断状态
	{
		case qTimerStarted:               //如果定时器已经启动或运行，则停止
		case qTimerRunning:
			if(timer->config == TIMER_CONFIG_TYPE_HARD)    //判断定时器类型
			{
				uint32_t status = qTaskEnterCritical();          //进入临界区
				
				qListRemove(&qTimerHardList, &timer->linkNode);  //将定时器加入硬定时器列表
				
				qTaskExitCritical(status);                       //退出临界区
			}
			else
			{
				qSemWait(&qTimerProtectSem, 0);                  //等待保护信号量
				
				qListRemove(&qTimerSoftList, &timer->linkNode);  //将定时器加入软定时器列表
				
				qSemNotify(&qTimerProtectSem);                   //释放保护信号量
			}
			break;
		default:
			break;
	}
}

/******************************************************************************
 * 函数名称：软定时器删除函数
 * 函数功能：删除软定时器
 * 输入参数：qTimer * timer                    定时器结构指针
 * 输出参数：无
 ******************************************************************************/
void qTimerDestroy(qTimer * timer)
{
	qTimerStop(timer);                     //停止定时器
	timer->state = qTimerDestroyed;        //设置其为删除状态
}

/******************************************************************************
 * 函数名称：软定时器状态查询函数
 * 函数功能：查询软定时器状态
 * 输入参数：qTimer * timer                    定时器结构指针
			qTimerInfo * timerInfo            状态查询结构指针
 * 输出参数：无
 ******************************************************************************/
void qTimerGetInfo(qTimer * timer, qTimerInfo * info)
{
	uint32_t status = qTaskEnterCritical();          //进入临界区
				
	info->stsrtDelayTicks = timer->stsrtDelayTicks;  //拷贝信息
	info->durationTicks = timer->durationTicks;
	info->timerFunc = timer->timerFunc;
	info->arg = timer->arg;
	info->config = info->config;
	info->state = timer->state;
				
	qTaskExitCritical(status);                       //退出临界区
}

/******************************************************************************
 * 函数名称：软定时器列表处理函数
 * 函数功能：处理软定时器列表
 * 输入参数：qList * timerList
 * 输出参数：无
 ******************************************************************************/
static void qTimerCallFuncList(qList * timerList)
{
	qNode * node;
	for(node = timerList->headNode.nextNode; node != &(timerList->headNode); node = node->nextNode)  //遍历结点
	{
		qTimer * timer = qNodeParent(node, qTimer, linkNode);       //取出定时器结构指针
		if((timer->delayTicks == 0) || (--timer->delayTicks == 0))  //判断定时器延时时间是否为0
		{
			timer->state = qTimerRunning;     //设置定时器运行
			timer->timerFunc(timer->arg);     //调用定时器回调函数
			timer->state = qTimerStarted;     //设置定时器开始
			
			if(timer->durationTicks > 0)  //判断定时器是否周期运行
			{
				timer->delayTicks = timer->durationTicks;   //重载定时器
			}
			else
			{
				qListRemove(timerList, &timer->linkNode);   //将定时器移除
				timer->state = qTimerStopped;               //设置定时器停止
			}
		}
	}
}

static qTask qTimerTask;
static qTaskStack qTimerTaskStack[QMRTOS_TIMERTASK_STACK_SIZE];
/******************************************************************************
 * 函数名称：软定时器任务入口函数
 * 函数功能：软定时器任务入口
 * 输入参数：void * param
 * 输出参数：无
 ******************************************************************************/
static void qTimerSoftTaskEntry(void * param)
{
	dprintf("this is TimerSoftTask\n");
	for(;;)
	{
		qSemWait(&qTimerTickSem, 0);         //等待系统节拍中断发送通知
		
		qSemWait(&qTimerProtectSem, 0);      //等待软定时器列表可以使用的通知
		
		qTimerCallFuncList(&qTimerSoftList); //对软定时器列表进行扫描并处理
		
		qSemNotify(&qTimerProtectSem);       //通知信号量可用
	}
}

/******************************************************************************
 * 函数名称：定时器任务通知函数
 * 函数功能：通知定时器任务
 * 输入参数：无
 * 输出参数：无
 ******************************************************************************/
void qTimerModuieTickNotify(void)
{
	uint32_t status = qTaskEnterCritical();          //进入临界区
				
	qTimerCallFuncList(&qTimerHardList);             //对硬定时器列表进行扫描并处理
				
	qTaskExitCritical(status);                       //退出临界区
	
	qSemNotify(&qTimerTickSem);                      //通知软定时器任务
}

/******************************************************************************
 * 函数名称：软定时器初始化函数
 * 函数功能：软定时器初始化
 * 输入参数：无
 * 输出参数：无
 ******************************************************************************/
void qTimerModuleInit(void)
{
#if QMRTOS_ENABLE_SEM == 0                    //使用软定时器需要打开信号量模块剪裁
	#error "Sem module is not open"
#endif /*QMRTOS_ENABLE_SEM == 0*/
	qListInit(&qTimerHardList);               //初始化定时器列表
	qListInit(&qTimerSoftList);
	qSemInit(&qTimerProtectSem, 1, 1);        //初始化保护信号量,只能使用一次初始可以使用
	qSemInit(&qTimerTickSem, 0, 0);           //初始化通知信号量,无限制
}

/******************************************************************************
 * 函数名称：软定时器任务初始化函数
 * 函数功能：软定时器任务初始化
 * 输入参数：无
 * 输出参数：无
 ******************************************************************************/
void qTimerInitTask(void)
{
#if QMRTOS_TIMERTASK_PRTO >= (QMRTOS_PRO_COUNT - 1)      //软定时器任务不能超出优先级个数
	#error "The proprity of timer tasker must greater then (QMRTOS_PRO_COUNT - 1)"
#endif /*QMRTOS_TIMERTASK_PRTO >= (QMRTOS_PRO_COUNT - 1)*/
	qTaskInit(&qTimerTask, qTimerSoftTaskEntry, (void *)0, QMRTOS_TIMERTASK_PRTO, qTimerTaskStack, QMRTOS_TIMERTASK_STACK_SIZE);  //初始化软定时器任务
}	

#endif /*QMRTOS_ENABLE_TIMER*/
/************** (C) COPYRIGHT 2014-2018 学生开放实验室 *****END OF FILE*********/
