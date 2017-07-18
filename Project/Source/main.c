/*
******************************************************************************
* Copyright (c) 2017,学生开放实验室
* All rights reserved.
******************************************************************************
* 文件名称： main.c
* 文件标识： OS
* 摘 要： 主函数定义
******************************************************************************
* 当前版本： 1.0
* 作 者： Quard
* 完成日期： 2017年7月18日
******************************************************************************
*/
#include "qmRTOS.h"
#include "api_inspect_entry.h"

qTask * currentTask;   //指示当前任务的指针
qTask * nextTask;      //指向下一个任务的指针
qTask * idleTask;      //指向空闲任务的指针

qBitmap taskPrioBitmap;// 任务优先级的标记位置结构

qList taskTable[QMRTOS_PRO_COUNT];  //任务列表

uint8_t schedLockCount;//调度锁计数器 

qList qTaskDelayedList; //延时队列


/******************************************************************************
 * 函数名称：查询当前就绪的优先级最高任务函数
 * 函数功能：获取当前最高优先级且可运行的任务
 * 输入参数：无
 * 输出参数：优先级最高的且可运行的任务指针
 ******************************************************************************/
qTask * qTaskHighestReady (void) 
{
    uint32_t highestPrio = qBitmapGetFirstSet(&taskPrioBitmap);     //找到优先级最高的位
	qNode * node =qListFirst(&taskTable[highestPrio]);              //找到该优先级下第一个任务结点
    return qNodeParent(node, qTask, linkNode);                      //找到该任务结点下任务的指针并返回  
}

/******************************************************************************
 * 函数名称：内核初始化函数
 * 函数功能：初始化OS
 * 输入参数：无
 * 输出参数：无 
 ******************************************************************************/
void qTaskSchedInit(void)
{
	int i;
	
	schedLockCount = 0;                                //初始化调度锁计数器为0
	qBitmapInit(&taskPrioBitmap);                      //初始化位图
	for(i = 0; i < QMRTOS_PRO_COUNT; i++)
	{
		qListInit(&taskTable[i]);                      //扫描任务就绪表并初始化所有链表
	}
}

/******************************************************************************
 * 函数名称：调度锁上锁函数
 * 函数功能：禁止任务调度
 * 输入参数：无
 * 输出参数：无 
 ******************************************************************************/
void qTaskSchedDisable(void)
{
	uint32_t status = qTaskEnterCritical();             //临界区保护
	
	if(schedLockCount < 255)                            //调度锁计数器加1，即锁定任务调度
	{
		schedLockCount++;
	}
	
	qTaskExitCritical(status);
}

/******************************************************************************
 * 函数名称：调度锁解锁函数
 * 函数功能：允许任务调度
 * 输入参数：无
 * 输出参数：无 
 ******************************************************************************/
void qTaskSchedEnable(void) 
{
    uint32_t status = qTaskEnterCritical();      //临界区保护

    if (schedLockCount > 0) 
    {
        if (--schedLockCount == 0) 
        {
            qTaskSched(); 
        }
    }

    qTaskExitCritical(status);
}

/******************************************************************************
 * 函数名称：就绪任务函数
 * 函数功能：添加任务到任务就绪表
 * 输入参数：qTask * task    任务指针
 * 输出参数：无
 ******************************************************************************/
void qTaskSchedRdy(qTask * task)
{
	qListAddFirst(&(taskTable[task->prio]), &(task->linkNode));  //向链表中插入任务结点
	qBitmapSet(&taskPrioBitmap, task->prio);   //置位位图中优先级位的相应位
}

/******************************************************************************
 * 函数名称：非就绪任务函数
 * 函数功能：删除任务就绪表中任务
 * 输入参数：qTask * task    任务指针
 * 输出参数：无 
 ******************************************************************************/
void qTaskSchedUnRdy(qTask * task)
{
	qListRemove(&taskTable[task->prio], &(task->linkNode));  //从链表中删除对应任务
	if(qListCount(&taskTable[task->prio]) == 0)               //判断链表里是否还有任务
	{
		qBitmapClear(&taskPrioBitmap, task->prio);   //如果没有任务则清除位图中优先级位的相应位
	}
}

/******************************************************************************
 * 函数名称：将任务从队列移除函数
 * 函数功能：从队列删除任务
 * 输入参数：qTask * task    任务指针
 * 输出参数：无 
 ******************************************************************************/
void qTaskSchedRemove(qTask * task)
{
	qListRemove(&taskTable[task->prio], &(task->linkNode));  //从链表中删除对应任务
	if(qListCount(&taskTable[task->prio]) == 0)               //判断链表里是否还有任务
	{
		qBitmapClear(&taskPrioBitmap, task->prio);   //如果没有任务则清除位图中优先级位的相应位
	}
}
	
/******************************************************************************
 * 函数名称：任务调度函数
 * 函数功能：决定cpu在那些任务之间运行，如何分配
 * 输入参数：无
 * 输出参数：无 
 ******************************************************************************/
void qTaskSched(void)
{
	qTask * tempTask;
	
	uint32_t status = qTaskEnterCritical();          //对任务调度函数进行保护
		
    if (schedLockCount > 0)                          // 若调度器已经被上锁，则不进行调度，直接退出
    {
        qTaskExitCritical(status);
        return;
    }
    // 找到优先级最高的任务，如果其优先级比当前任务的还高，那么就切换到这个任务
    tempTask = qTaskHighestReady();
    if (tempTask != currentTask) 
    {
        nextTask = tempTask;
        qTaskSwitch();   
    }

    qTaskExitCritical(status);                       // 退出临界区
}
	
/******************************************************************************
 * 函数名称：延时队列初始化函数
 * 函数功能：调用链表初始化函数对延时队列初始化
 * 输入参数：无
 * 输出参数：无 
 ******************************************************************************/
void qTaskDelayedInit(void)
{
	qListInit(&qTaskDelayedList);                     //初始化延时队列
}

/******************************************************************************
 * 函数名称：等待任务函数
 * 函数功能：将需要延时的任务的结点插入到延时队列
 * 输入参数：qTask * task     任务指针
			uint32_t ticks   需要延时的ticks数
 * 输出参数：无 
 ******************************************************************************/
void qTimeTaskWait(qTask * task, uint32_t ticks)
{
	task->delayTicks = ticks;                             //延时时间 
	qListAddLast(&qTaskDelayedList, &(task->delayNode));   //将对应任务插入到延时队列队尾
	task->state |= QMRTOS_TASK_STATE_DELAYED;             //将任务状态标志位置为延时状态 
}

/******************************************************************************
 * 函数名称：唤醒任务函数
 * 函数功能：将延时结束的任务的结点从延时队列移除
 * 输入参数：qTask * task     任务指针
 * 输出参数：无  
 ******************************************************************************/
void qTimeTaskWakeUp(qTask * task)
{
	qListRemove(&qTaskDelayedList, &(task->delayNode));   //将任务结点移除延时队列
	task->state &= ~QMRTOS_TASK_STATE_DELAYED;            //将任务状态标志位延时状态取消
}

/******************************************************************************
 * 函数名称：从延时队列删除任务函数
 * 函数功能：将任务从延时队列删除
 * 输入参数：qTask * task     任务指针
 * 输出参数：无  
 ******************************************************************************/
void qTimeTaskRomove(qTask * task)
{
	qListRemove(&qTaskDelayedList, &(task->delayNode));   //将任务结点移除延时队列
}

/******************************************************************************
 * 函数名称：任务SysTick定时器中断函数
 * 函数功能：发生中断时被定时器中断函数调用，使delayTicks--，并调用一次调度函数
 * 输入参数：无
 * 输出参数：无 
 ******************************************************************************/
void qTaskSystemTickHandler(void)
{
	qNode * node;
	
	uint32_t status = qTaskEnterCritical();          //对任务调度函数进行保护
	
	for(node = qTaskDelayedList.headNode.nextNode; node != &(qTaskDelayedList.headNode); node = node->nextNode)      //扫描任务延时队列
	{
		qTask * task = qNodeParent(node, qTask, delayNode);   //获取任务的地址
		if(--task->delayTicks == 0)
		{
			if(task->waitEvent)        //判读任务是否存在等待事件块队列
			{
				qEventRemoveTask(task, (void *)0, qErrorTimeout);   //将任务移除事件等待队列，并发送超时错误  
			}
			
			qTimeTaskWakeUp(task);                   //从延时队列唤醒任务
			qTaskSchedRdy(task);                     //就绪任务
		}
	}
	
	if(--currentTask->slice == 0)                    //判断时间片是否使用完毕
	{
		if(qListCount(&taskTable[currentTask->prio]) > 0) //如果使用完毕，判断当前优先级是否还存在任务
		{
			qListRemoveFirst(&taskTable[currentTask->prio]);  //如果存在任务，则移除当前任务结点
			qListAddLast(&taskTable[currentTask->prio], &(currentTask->linkNode));  //添加当前任务结点到最后一个
			
			currentTask->slice = QMRTOS_SLTIC_MAX;        //给当前任务重新配备时间片
		}
	}
	
	qTaskExitCritical(status);
	
	qTaskSched();                    //调用任务调度函数
}



qTask qTaskIdle;     //定义空闲任务
qTaskStack idleTaskEnv[QMRTOS_IDLETSSK_STACK_SIZE]; //定义空任务堆栈空间  

/******************************************************************************
 * 函数名称：空闲任务函数
 * 函数功能：空闲任务
 * 输入参数：无
 * 输出参数：无 
 ******************************************************************************/
void idleTaskEntry(void * param)
{
	for(;;)
	{

	}
}

/******************************************************************************
 * 函数名称：主函数
 * 函数功能：主函数
 * 输入参数：无
 * 输出参数：无 
 ******************************************************************************/
int main()
{
	qTaskSchedInit();          //初始化系统内核
	dprintf("TaskInit is Ok!\n");
	
	qTaskDelayedInit();        //延时列表初始化
	
//	qInitApp();                //任务初始化
	
	InspectTaskstart();        //进行功能巡检测试任务
	
	qTaskInit(&qTaskIdle, idleTaskEntry, (void *)0, QMRTOS_PRO_COUNT - 1, &idleTaskEnv[QMRTOS_IDLETSSK_STACK_SIZE]);  //初始化空闲任务
	idleTask = &qTaskIdle;

    nextTask = qTaskHighestReady();   //初始自动查找最高优先级的任务运行
	
	qTaskRunFirst();           //运行OS，开始调度第一个任务
	
	return 0;
}	

/************** (C) COPYRIGHT 2014-2018 学生开放实验室 *****END OF FILE*********/
