#ifndef QMRTOS_H
#define QMRTOS_H

#include <stdint.h>

#include "qConfig.h"
#include "qLib.h"

#include "Debug/Debug.h"

#define QMRTOS_TASK_STATE_RDY        0           //就绪状态
#define QMRTOS_TASK_STATE_DELAYED    (1 << 1)    //延时状态
#define QMRTOS_TASK_STATE_SUSPEND    (1 << 2)    //挂起状态  

typedef uint32_t qTaskStack;       //堆栈单元类型

typedef struct _qTask {            //任务结构
	qTaskStack * stack;            //任务堆栈指针
	qNode linkNode;                //连接结点
	uint32_t delayTicks;           //任务延时个数
	qNode delayNode;               //任务延时结点
	uint32_t prio;                 //任务优先级
	uint32_t state;                //任务状态
	uint32_t slice;                //时间片计数器
	uint32_t suspendCount;         //挂起计数器
}qTask;

extern qTask * currentTask;   
extern qTask * nextTask;     

void qTaskRunFirst (void);
void qTaskSwitch (void);

uint32_t qTaskEnterCritical(void);
void qTaskExitCritical(uint32_t status);

void qTaskSched(void);
void qTaskSchedEnable(void);
void qTaskSchedDisable(void);	
void qTaskSchedRdy(qTask * task);
void qTaskSchedUnRdy(qTask * task);
void qTaskSchedInit(void);
void qTimeTaskWait(qTask * task, uint32_t ticks);
void qTimeTaskWakeUp(qTask * task);
void qTaskSystemTickHandler(void);
void qTaskDelay(uint32_t delay);
void qTaskInit(qTask * task , void (*entry) (void *), void *param ,uint32_t prio, qTaskStack * stack );
void qSetSysTickPeriod(uint32_t ms);
void qInitApp(void);
void qTaskSuspend(qTask * task);
void qTaskWakeUp(qTask * task);

#endif /*QMRTOS_H*/
