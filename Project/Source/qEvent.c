#include "qmRTOS.h"

/******************************************************************************
 * 函数名称：事件块初始化函数
 * 函数功能：初始化事件块
 * 输入参数：qEvent * event       事件块结构指针
			qEventType type      事件块类型
 * 输出参数：无 
 ******************************************************************************/
void qEventInit(qEvent * event, qEventType type)
{
	event->type = qEventTypeUnknow;
	qListInit(&event->waitList);
}

/******************************************************************************
 * 函数名称：等待事件块函数
 * 函数功能：让任务进入等待事件块队列中
 * 输入参数：qEvent * event       要进入的事件块结构指针
			qTask * task         要进入事件块的任务
			void * msg           存放消息的位置 
			uint32_t state       等待状态
			uint32_t timeout     超时时间
 * 输出参数：无 
 ******************************************************************************/
void qEventWait(qEvent * event, qTask * task, void * msg,  uint32_t state, uint32_t timeout)
{
	uint32_t statue = qTaskEnterCritical();   //进入临界区

	task->state |= state << 16;      //设定任务状态标志
	task->waitEvent = event;   //设置进入的事件块
	task->eventMsg = msg;      //设置存放消息
	task->waitEventResult = qErrorNoError; //设置等待结果无
	
	qTaskSchedUnRdy(task);     //将任务移除就绪队列
	
	qListAddLast(&event->waitList, &task->linkNode);   //将任务插入事件块等待队列
	
	if(timeout)
	{
		qTimeTaskWait(task, timeout);    //检查是否设置超时，将其加入延时队列
	}
	
	qTaskExitCritical(statue);
}

/******************************************************************************
 * 函数名称：唤醒事件块函数
 * 函数功能：让任务从等待事件块队列中唤醒
 * 输入参数：qEvent * event       要进入的事件块结构指针
			void * msg           存放消息的位置 
			uint32_t result      唤醒结果
 * 输出参数：被唤醒的任务结构指针 
 ******************************************************************************/
qTask * qEventWakeUp(qEvent * event, void * msg, uint32_t result)
{
	qNode * node;
	qTask * task = (qTask *)0;
	uint32_t statue = qTaskEnterCritical();   //进入临界区

	if((node = qListRemoveFirst(&event->waitList)) != (qNode *)0)   //从事件块队列移除一个任务并判断
	{
		task = (qTask *)qNodeParent(node, qTask, linkNode);   //如果这个任务存在，则获取其任务指针
		task->waitEvent = (qEvent *)0;         //对其事件块清除
		task->eventMsg = msg;                  //设置消息
		task->waitEventResult = result;        //设置唤醒结果
		task->state &= ~QMRTOS_TASK_WAIT_WASK; //清除状态位
		
		if(task->delayTicks != 0)
		{
			qTimeTaskWakeUp(task);           //判断任务是否有延时，如果有则将其唤醒
		}
		
		qTaskSchedRdy(task);        //将任务插入到就绪队列
	}
	
	qTaskExitCritical(statue);
	
	return task;
}

/******************************************************************************
 * 函数名称：唤醒事件块制定任务函数
 * 函数功能：让指定任务从等待事件块队列中唤醒
 * 输入参数：qEvent * event       要进入的事件块结构指针
			qTask * task         指定任务
			void * msg           存放消息的位置 
			uint32_t result      唤醒结果
 * 输出参数：被唤醒的任务结构指针 
 ******************************************************************************/
void qEventWakeUpTask (qEvent * event, qTask * task, void * msg, uint32_t result)
{
    uint32_t status = qTaskEnterCritical();                 // 进入临界区

    qListRemove(&event->waitList, &task->linkNode);         //将任务移除事件块

    task->waitEvent = (qEvent *)0;      // 设置收到的消息、结构，清除相应的等待标志位
    task->eventMsg = msg;
    task->waitEventResult = result;
    task->state &= ~QMRTOS_TASK_WAIT_WASK;

    if (task->delayTicks != 0)          // 任务申请了超时等待，这里检查下，将其从延时队列中移除
    {
        qTimeTaskWakeUp(task);
    }
    qTaskSchedRdy(task);                // 将任务加入就绪队列

    qTaskExitCritical(status);          // 退出临界区
}

/******************************************************************************
 * 函数名称：移除事件块函数
 * 函数功能：将任务从等待事件块队列中强制移除
 * 输入参数：qTask * task         要移除的任务结构指针
			void * msg           存放消息的位置 
			uint32_t result      唤醒结果
 * 输出参数：无 
 ******************************************************************************/
void qEventRemoveTask(qTask * task, void * msg, uint32_t result)
{
	uint32_t statue = qTaskEnterCritical();   //进入临界区

	qListRemove(&task->waitEvent->waitList, &task->linkNode);   //将指定任务移除
	task->waitEvent = (qEvent *)0;         //对其事件块清除
	task->eventMsg = msg;                  //设置消息
	task->waitEventResult = result;        //设置唤醒结果
	task->state &= QMRTOS_TASK_WAIT_WASK;  //清除状态位
	
	qTaskExitCritical(statue);
}

/******************************************************************************
 * 函数名称：清空事件块函数
 * 函数功能：将全部任务从等待事件块队列中移除
 * 输入参数：qEvent * event       要清空的事件块结构指针
			void * msg           存放消息的位置 
			uint32_t result      唤醒结果
 * 输出参数：事件块中的任务数量
 ******************************************************************************/
uint32_t qEventRemoveAll(qEvent * event, void * msg, uint32_t result)
{
	qNode * node;
	uint32_t count = 0;
	
	uint32_t statue = qTaskEnterCritical();   //进入临界区

	count = qListCount(&event->waitList);   //获取队列中的任务数量
	
	while((node = qListRemoveFirst(&event->waitList)) != (qNode *)0)   //依次移除队列头部的任务
	{
		qTask * task = (qTask *)qNodeParent(node, qTask, linkNode);   //获取任务指针
		task->waitEvent = (qEvent *)0;         //对其事件块清除
		task->eventMsg = msg;                  //设置消息
		task->waitEventResult = result;        //设置唤醒结果
		task->state &= QMRTOS_TASK_WAIT_WASK;  //清除状态位
		
		if(task->delayTicks != 0)        //判断任务是否有延时   
		{
			qTimeTaskWakeUp(task);      //唤醒任务，从延时队列移除
		}
		
		qTaskSchedRdy(task);       //将任务就绪
	}
	
	qTaskExitCritical(statue);
	
	return count;
}

/******************************************************************************
 * 函数名称：获取事件块任务数量函数
 * 函数功能：获取事件块任务数量
 * 输入参数：qEvent * event       要获取数量的事件块结构指针
 * 输出参数：事件块中的任务数量
 ******************************************************************************/
uint32_t qEventWaitCount(qEvent * event)
{
	uint32_t count = 0;
	
	uint32_t statue = qTaskEnterCritical();   //进入临界区

	count = qListCount(&event->waitList);   //获取队列中的任务数量
	
	qTaskExitCritical(statue);
	
	return count;
}
