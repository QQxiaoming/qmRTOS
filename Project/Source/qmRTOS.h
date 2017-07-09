#ifndef QMRTOS_H
#define QMRTOS_H

#include <stdint.h>

#include "qConfig.h"
#include "qLib.h"

#include "Debug/Debug.h"

#define QMRTOS_TASK_STATE_RDY        0           //就绪状态
#define QMRTOS_TASK_STATE_DESTORYED  (1 << 0)    //删除状态
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
	
	void (*clean)(void * param);   //任务清理函数入口
	void * cleanParam;                  //清理函数参数
	uint8_t requestDeleteFlag;       //任务删除标记
}qTask;

extern qTask * currentTask;   
extern qTask * nextTask;     

void qTaskRunFirst (void);
void qTaskSwitch (void);

uint32_t qTaskEnterCritical(void);
void qTaskExitCritical(uint32_t status);

void qTaskSchedInit(void);
void qTaskSchedDisable(void);
void qTaskSchedEnable(void);
void qTaskSchedRdy(qTask * task);
void qTaskSchedUnRdy(qTask * task);
void qTaskSchedRemove(qTask * task);
void qTaskSched(void);
void qTimeTaskWait(qTask * task, uint32_t ticks);
void qTimeTaskWakeUp(qTask * task);
void qTimeTaskRomove(qTask * task);
void qTaskSystemTickHandler(void);
void qTaskDelay(uint32_t delay);

typedef struct _qTaskInfo{
	uint32_t delayTicks;
	uint32_t prio;
	uint32_t state;
	uint32_t slice;
	uint32_t suspendCount;
}qTaskInfo;

void qTaskInit(qTask * task , void (*entry) (void *), void *param ,uint32_t prio, qTaskStack * stack );
void qTaskSuspend(qTask * task);
void qTaskWakeUp(qTask * task);
void qTaskSetCleanCallFunc(qTask * task, void (*clean)(void * param), void * param);
void qTaskForceDelete(qTask * task);
void qTaskRequestDelete(qTask * task);
uint8_t qTaskIsRequestedDeleted(void);
void qTaskDeleteSelf(void);
void qTaskGetInfo(qTask * task, qTaskInfo * Info);

void qSetSysTickPeriod(uint32_t ms);
void qInitApp(void);

#endif /*QMRTOS_H*/
