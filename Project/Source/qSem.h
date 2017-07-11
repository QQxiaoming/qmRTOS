#ifndef QSEM_H
#define QSEM_H

#include "qEvent.h"

typedef struct _qSem
{
	qEvent event;
	uint32_t count;
	uint32_t maxCount;
}qSem;

typedef struct _qSemInfo
{
	uint32_t count;
	uint32_t maxCount;
	uint32_t taskCount;     //等待的任务个数
}qSemInfo;

void qSemInit(qSem * sem, uint32_t startCount, uint32_t maxCount);
uint32_t qSemWait(qSem * sem, uint32_t waitTicks);
uint32_t qSemNoWaitGet(qSem * sem);
void qSemNotify(qSem * sem);
uint32_t qSemDestroy(qSem * sem);
void qSemGetInfo(qSem * sem, qSemInfo * info);
	
#endif /*QSEM_H*/
