#ifndef QMUTEX_H
#define QMUTEX_H

#include "qEvent.h"

typedef struct _qMutex
{
	qEvent event;               //事件控制块
	uint32_t lockedCount;       //锁计数器
	qTask * owner;              //当前占有任务
	uint32_t ownerOriginalPrio; //占有任务的原始优先级
}qMutex;

typedef struct  _qMutexInfo
{
    uint32_t taskCount;     //等待的任务数量
    uint32_t ownerPrio;     //拥有者任务的优先级
    uint32_t inheritedPrio; // 继承优先级
    qTask * owner;          // 当前信号量的拥有者
    uint32_t lockedCount;   // 锁定次数
}qMutexInfo;

void qMutexInit(qMutex * mutex);
uint32_t qMutexWait(qMutex * mutex, uint32_t waitTicks);
uint32_t qMutexNoWaitGet(qMutex * mutex);
uint32_t qMutexNotify(qMutex * mutex);
uint32_t qMutexDestroy(qMutex * mutex);
void qMutexGetInfo(qMutex * mutex, qMutexInfo * info);
#endif /*QMUTEX_H*/
