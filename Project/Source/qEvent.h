#ifndef QEVENT_H
#define QEVENT_H

#include "qLib.h"
#include "qTask.h"

typedef enum _qEventType{
	qEventTypeUnknow,
	qEventTypeSem,
	qEventTypeMbox,
}qEventType;

typedef struct _qEvent{
	qEventType type;
	qList waitList;
}qEvent;

void qEventInit(qEvent * event, qEventType type);
void qEventWait(qEvent * event, qTask * task, void * msg,  uint32_t state, uint32_t timeout);
qTask * qEventWakeUp(qEvent * event, void * msg, uint32_t result);
void qEventRemoveTask(qTask * task, void * msg, uint32_t result);

uint32_t qEventRemoveAll(qEvent * event, void * msg, uint32_t result);
uint32_t qEventWaitCount(qEvent * event);
#endif /*QEVENT_H*/
