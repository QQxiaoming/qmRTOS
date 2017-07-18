/*
******************************************************************************
* Copyright (c) 2017,学生开放实验室
* All rights reserved.
******************************************************************************
* 文件名称： qTask.h
* 文件标识： OS
* 摘 要： 任务管理函数声明
******************************************************************************
* 当前版本： 1.0
* 作 者： Quard
* 完成日期： 2017年7月18日
******************************************************************************
*/
#ifndef QTASK_H
#define QTASK_H

#define QMRTOS_TASK_STATE_RDY        0              //就绪状态
#define QMRTOS_TASK_STATE_DESTORYED  (1 << 0)       //删除状态
#define QMRTOS_TASK_STATE_DELAYED    (1 << 1)       //延时状态
#define QMRTOS_TASK_STATE_SUSPEND    (1 << 2)       //挂起状态  

#define QMRTOS_TASK_WAIT_WASK        (0xFF << 16)   //等待状态标志区

struct _qEvent;   //c语言，前向引用

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
	void * cleanParam;             //清理函数参数
	uint8_t requestDeleteFlag;     //任务删除标记
	
	struct _qEvent * waitEvent;    //任务等待的事件块
	void * eventMsg;               //等待的数据存放位置
	uint32_t waitEventResult;      //等待的结果
	
	uint32_t waitFlagsType;        //等待请求类型
	uint32_t eventFlags;           //请求标志
}qTask;

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


#endif /*QTASK_H*/

/************** (C) COPYRIGHT 2014-2018 学生开放实验室 *****END OF FILE*********/
