#ifndef QMRTOS_H
#define QMRTOS_H

#include <stdint.h>

#include "qConfig.h"
#include "qLib.h"
#include "qTask.h"
#include "qEvent.h"
#include "qSem.h"

#include "Debug/Debug.h"

typedef enum _qError{          //错误码
	qErrorNoError,
	qErrorTimeout,
	qErrorResourceUnavaliable,
	qErrorDel,
}qError;

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


void qSetSysTickPeriod(uint32_t ms);
void qInitApp(void);

#endif /*QMRTOS_H*/
