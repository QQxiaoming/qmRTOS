#include "qmRTOS.h"

/******************************************************************************
 * 函数名称：邮箱初始化函数
 * 函数功能：初始化邮箱
 * 输入参数：qMbox * mbox               消息结构指针
			void **msgBuffer           数据缓冲区指针
			uint32_t maxCount          最大消息数量
 * 输出参数：无
 ******************************************************************************/
void qMboxInit(qMbox * mbox, void **msgBuffer, uint32_t maxCount)
{
	qEventInit(&mbox->event, qEventTypeMbox);
	mbox->msgBuffer = msgBuffer;
	mbox->maxCount = maxCount;
	mbox->read = 0;
	mbox->write = 0;
	mbox->count = 0;
}

/******************************************************************************
 * 函数名称：消息等待函数
 * 函数功能：任务需要消息进入等待
 * 输入参数：qMbox * mbox               邮箱结构指针
			void **msg                 消息地址
			uint32_t waitTicks         等待时间
 * 输出参数：返回错误代码
 ******************************************************************************/
uint32_t qMboxWait(qMbox * mbox, void **msg, uint32_t waitTicks)
{
	uint32_t status = qTaskEnterCritical();          //进入临界区
	
	if(mbox->count > 0)    //判断邮箱是否存在消息
	{
		--mbox->count;     //消息数减一
		*msg = mbox->msgBuffer[mbox->read++];  //消息写入，读指针后移
		if(mbox->read >= mbox->maxCount)       //判断读指针是否到末尾
		{
			mbox->read = 0;//如果是，则将读指针移向第一个
		}
		qTaskExitCritical(status);                   //退出临界区
		return qErrorNoError;      //返回没有错误
	}
	else
	{
		qEventWait(&mbox->event, currentTask, (void *)0, qEventTypeMbox, waitTicks);//否则邮箱没有消息，当前任务开始等待消息
		qTaskExitCritical(status);                       //退出临界区

		qTaskSched();     //进行调度
		
		*msg = currentTask->eventMsg;          //任务取出消息
		return currentTask->waitEventResult;  //返回等待结果
	}
}

/******************************************************************************
 * 函数名称：任务获取消息无等待函数
 * 函数功能：任务需要消息无等待
 * 输入参数：qMbox * mbox               邮箱结构指针
			void **msg                 消息地址
 * 输出参数：返回错误代码
 ******************************************************************************/
uint32_t qMboxNoWaitGet(qMbox * mbox, void **msg)
{
	uint32_t status = qTaskEnterCritical();          //进入临界区

	if(mbox->count > 0)    //判断邮箱是否存在消息
	{
		--mbox->count;     //消息数减一
		*msg = mbox->msgBuffer[mbox->read++];  //消息写入，读指针后移
		if(mbox->read >= mbox->maxCount)       //判断读指针是否到末尾
		{
			mbox->read = 0;//如果是，则将读指针移向第一个
		}
		qTaskExitCritical(status);                   //退出临界区
		return qErrorNoError;      //返回没有错误
	}
	else
	{
		qTaskExitCritical(status);                   //退出临界区
		return qErrorResourceUnavaliable;            //返回没有资源
	}
}

/******************************************************************************
 * 函数名称：发送消息函数
 * 函数功能：发送消息到邮箱
 * 输入参数：qMbox * mbox               邮箱结构指针
			void * msg                 数据指针
			uint32_t notifyOption      模式
 * 输出参数：错误代码
 ******************************************************************************/
uint32_t qMboxNotify(qMbox * mbox, void * msg, uint32_t notifyOption)
{
	uint32_t status = qTaskEnterCritical();          //进入临界区

	if(qEventWaitCount(&mbox->event) > 0)         //判断是否有任务在等待
	{
		qTask * task = qEventWakeUp(&mbox->event, (void *)msg, qErrorNoError);  //将消息传给任务
	}
	else
	{
		if(mbox->count >= mbox->maxCount)     //如果没有任务等待，则判断消息是否已经满了
		{
			qTaskExitCritical(status);                   //退出临界区
			return qErrorResourceFull;     //如果已经满了则返回资源已满错误
		}
		if(notifyOption & qMBoxSendFront)        //如果没满，则插入消息,判断以什么方式插入
		{
			//高优先级插入方式：
			if(mbox->read <= 0)          //判断读索引是否小于0
			{
				mbox->read = mbox->maxCount - 1;  //将读索引执行最后，再写入消息前移动
			}
			else
			{
				--mbox->read;//否则读索引直接前移
			}
			mbox->msgBuffer[mbox->read] = msg;   //将消息写入
		}
		else
		{
			//普通插入方式：
			mbox->msgBuffer[mbox->write++] = msg;   //将消息写入
			if(mbox->write >= mbox->maxCount)    //判断写索引是否到最后了
			{
				mbox->write = 0;//如果是则将写索引指向0
			}
		}
		mbox->count++;    //消息数自增
	}
	
	qTaskExitCritical(status);                   //退出临界区
	return qErrorNoError;           //返回没有错误
}

/******************************************************************************
 * 函数名称：清空邮箱函数
 * 函数功能：清空邮箱
 * 输入参数：qMbox * mbox               邮箱结构指针
 * 输出参数：无
 ******************************************************************************/
void qMboxFlush(qMbox * mbox)
{
	uint32_t status = qTaskEnterCritical();          //进入临界区

	if(qEventWaitCount(&mbox->event) == 0)      //判读邮箱中是否有任务在等待
	{
		mbox->read = 0;	//如果有则将其清空
		mbox->write = 0;
		mbox->count = 0;
	}
	
	qTaskExitCritical(status);                   //退出临界区
}

/******************************************************************************
 * 函数名称：删除邮箱函数
 * 函数功能：删除邮箱
 * 输入参数：qMbox * mbox               邮箱结构指针
 * 输出参数：删除的任务数
 ******************************************************************************/
uint32_t qMboxDestroy(qMbox * mbox)
{
	uint32_t status = qTaskEnterCritical();          //进入临界区

	uint32_t count = qEventRemoveAll(&mbox->event, (void *)0, qErrorDel);  //清空事件块所有的任务
	
	qTaskExitCritical(status);                   //退出临界区
	
	if(count > 0)              //判断是否删除了任务
	{
		qTaskSched();          //如果删除了等待任务则进行调度
	} 
	return count;              //返回删除的个数
}

/******************************************************************************
 * 函数名称：查询邮箱状态信息函数
 * 函数功能：查询邮箱状态信息
 * 输入参数：qMbox * mbox               邮箱结构指针
            qMboxInfo * info           邮箱状态查询结构指针
 * 输出参数：删除的任务数
 ******************************************************************************/
void tMboxGetInfo (qMbox * mbox, qMboxInfo * info)
{
    uint32_t status = qTaskEnterCritical();

    // 拷贝需要的信息
    info->count = mbox->count;
    info->maxCount = mbox->maxCount;
    info->taskCount = qEventWaitCount(&mbox->event);

    qTaskExitCritical(status);
}
