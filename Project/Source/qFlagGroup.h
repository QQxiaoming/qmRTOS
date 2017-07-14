#ifndef QFLAGGROUP_H
#define QFLAGGROUP_H

#include "qEvent.h"

typedef struct _qFlagGroup
{
	qEvent event;       //事件控制块
	uint32_t flags;      //32位的标志组
}qFlagGroup;

typedef struct _qFlagGroupInfo
{
	uint32_t flags;      //当前的事件标志
    uint32_t taskCount;  //当前等待的任务计数
}qFlagGroupInfo;

#define QFLAGGROUP_CLEAR      (0x0 << 0)   //清除
#define QFLAGGROUP_SET		  (0x1 << 0)   //设置
#define QFLAGGROUP_ANY		  (0x0 << 1)   //任意
#define QFLAGGROUP_ALL		  (0x0 << 1)   //所有

#define QFLAGGROUP_STT_ALL    (QFLAGGROUP_SET | QFLAGGROUP_ALL)    //所有设置
#define QFLAGGROUP_SET_ANY    (QFLAGGROUP_SET | QFLAGGROUP_ANY)    //任意设置
#define QFLAGGROUP_CLEAR_ALL  (QFLAGGROUP_CLEAR | QFLAGGROUP_ALL)  //所有清除
#define QFLAGGROUP_CLEAR_ANY  (QFLAGGROUP_CLEAR | QFLAGGROUP_ANY)  //任意清除

#define QFLAGGROUP_CONSUME    (1 << 7)  

void qFlagGroupInit(qFlagGroup * flagGroup, uint32_t flags);
uint32_t qFlagGroupWait(qFlagGroup * flagGroup, uint32_t waitType, uint32_t requestFlag, uint32_t * resultFlag, uint32_t waitTicks);
uint32_t qFlagGroupNoWaitGet(qFlagGroup * flagGroup, uint32_t waitType, uint32_t requestFlag, uint32_t * resultFlag);
void qFlagGroupNotify(qFlagGroup * flagGroup, uint8_t isSet, uint32_t flags);
uint32_t qFlagGroupDestroy (qFlagGroup * flagGroup);
void qFlagGroupGetInfo (qFlagGroup * flagGroup, qFlagGroupInfo * info);
#endif /*QFLAGGROUP_H*/
