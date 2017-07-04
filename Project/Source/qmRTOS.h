#ifndef QMRTOS_H
#define QMRTOS_H

#include <stdint.h>

#include "qConfig.h"
#include "qLib.h"

#include "Debug/Debug.h"

typedef uint32_t qTaskStack;       //堆栈单元类型

typedef struct _qTask {            //任务结构
	qTaskStack * stack;            //任务堆栈指针
	uint32_t delayTicks;           //任务延时个数
	uint32_t prio;                 //任务优先级
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
void wTaskSchedInit(void);

#endif /*QMRTOS_H*/
