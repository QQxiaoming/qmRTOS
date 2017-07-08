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
	*(--stack) = (unsigned long)(1 << 24);              // XPSR, 设置了Thumb模式，恢复到Thumb状态而非ARM状态运行
	*(--stack) = (unsigned long)entry;                  // PC, 程序的入口地址
    *(--stack) = (unsigned long)0x14;                   // R14(LR), 任务不会通过return xxx结束自己，所以未用
    *(--stack) = (unsigned long)0x12;                   // R12, 未用
    *(--stack) = (unsigned long)0x3;                    // R3, 未用
    *(--stack) = (unsigned long)0x2;                    // R2, 未用
    *(--stack) = (unsigned long)0x1;                    // R1, 未用
    *(--stack) = (unsigned long)param;                  // R0 = param, 传给任务的入口函数
    *(--stack) = (unsigned long)0x11;                   // R11, 未用
    *(--stack) = (unsigned long)0x10;                   // R10, 未用
    *(--stack) = (unsigned long)0x9;                    // R9, 未用
    *(--stack) = (unsigned long)0x8;                    // R8, 未用
    *(--stack) = (unsigned long)0x7;                    // R7, 未用
    *(--stack) = (unsigned long)0x6;                    // R6, 未用
    *(--stack) = (unsigned long)0x5;                    // R5, 未用
    *(--stack) = (unsigned long)0x4;                    // R4, 未用
	
	task->slice = QMRTOS_SLTIC_MAX;                     // 时间片初始化为最大值
	task->stack = stack;                                // 保存最终的值
	task->delayTicks = 0;								// 初始任务延时个数为0
	task->prio = prio;                                  // 设置任务的优先级
	task->state = QMRTOS_TASK_STATE_RDY;                // 设置任务为就绪状态
	
	qNodeInit(&(task->delayNode));                      // 初始化延时结点
	qNodeInit(&(task->linkNode));                       // 初始化链接结点
	
	qTaskSchedRdy(task);                                // 将任务加入队列并就绪
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
