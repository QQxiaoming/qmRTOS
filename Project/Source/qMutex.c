#include "qmRTOS.h"

/******************************************************************************
 * 函数名称：互斥锁初始化函数
 * 函数功能：初始化互斥锁
 * 输入参数：qMutex * mutex        互斥锁结构指针
 * 输出参数：无
 ******************************************************************************/
void qMutexInit(qMutex * mutex)
{
	qEventInit(&mutex->event,qEventTypeMutex);      //初始化互斥锁中的事件块
	 
	mutex->lockedCount = 0;                         //初始化赋值相关内容
	mutex->owner = (qTask *)0;
	mutex->ownerOriginalPrio = QMRTOS_PRO_COUNT;
}

/******************************************************************************
 * 函数名称：互斥锁等待函数
 * 函数功能：等待互斥锁
 * 输入参数：qMutex * mutex        互斥锁结构指针
			uint32_t waitTicks    等待时间
 * 输出参数：返回结果
 ******************************************************************************/
uint32_t qMutexWait (qMutex * mutex, uint32_t waitTicks)
{
    uint32_t status = qTaskEnterCritical();    //进入临界区

    if (mutex->lockedCount <= 0)               //判断互斥锁是否上锁
    {
        mutex->owner = currentTask;            //如果没有锁定，则使用当前任务锁定
        mutex->ownerOriginalPrio = currentTask->prio;
        mutex->lockedCount++;

        qTaskExitCritical(status);             //退出临界区
        return qErrorNoError;                  //返回没有错误
    }
    else                                       //如果已经上锁
    {
        if (mutex->owner == currentTask)       //判断是否是当前任务再次占用
        {
            mutex->lockedCount++;              //如果是，计数值加一即可

            qTaskExitCritical(status);         //退出临界区
            return qErrorNoError;              //返回没有错误
        }
        else                                   //判断互斥锁拥有者之外的任务wait，则要检查下是否需要使用
        {
            if (currentTask->prio < mutex->owner->prio)  //判断当前任务优先级是否更高
            {
											  //如果当前任务的优先级比拥有者优先级更高，则使用优先级继承
                                              //提升原拥有者的优先
                qTask * owner = mutex->owner; //取出任务            
                if (owner->state == QMRTOS_TASK_STATE_RDY)  //判断任务是否就绪
                {
                   
                    qTaskSchedUnRdy(owner);    //任务处于就绪状态时，更改任务在就绪表中的位置      
                    owner->prio = currentTask->prio;        //修改优先级
                    qTaskSchedRdy(owner);      
                }
                else
                {
                    owner->prio = currentTask->prio;        //其它状态，只需要修改优先级
                }
            }


            qEventWait(&mutex->event, currentTask, (void *)0, qEventTypeMutex, waitTicks);
                                                           //当前任务进入等待队列中
			qTaskExitCritical(status);                     //退出临界区


            qTaskSched();              //执行调度， 切换至其它任务
            return currentTask->waitEventResult;          //返回结果
        }
    }
}

/******************************************************************************
 * 函数名称：互斥锁无等待函数
 * 函数功能：无等待互斥锁
 * 输入参数：qMutex * mutex        互斥锁结构指针
 * 输出参数：返回结果
 ******************************************************************************/
uint32_t qMutexNoWaitGet (qMutex * mutex)
{
    uint32_t status = qTaskEnterCritical();      //进入临界区

   if (mutex->lockedCount <= 0)                //判断互斥锁是否上锁
    {
        mutex->owner = currentTask;            //如果没有锁定，则使用当前任务锁定
        mutex->ownerOriginalPrio = currentTask->prio;
        mutex->lockedCount++;

        qTaskExitCritical(status);             //退出临界区
        return qErrorNoError;                  //返回没有错误
    }
    else                                       //互斥锁已经被锁定
    {  
        if (mutex->owner == currentTask)       //判断互斥锁的拥有者是否是当前任务
        {
            
            mutex->lockedCount++;              //如果是则简单增加计数

            qTaskExitCritical(status);             //退出临界区
			return qErrorNoError;                  //返回没有错误
        }

        qTaskExitCritical(status);                 //退出临界区
        return qErrorResourceUnavaliable;          //返回资源不可用
    }
}

/******************************************************************************
 * 函数名称：互斥锁通知函数
 * 函数功能：通知互斥锁可用
 * 输入参数：qMutex * mutex        互斥锁结构指针
 * 输出参数：返回结果
 ******************************************************************************/
 uint32_t qMutexNotify (qMutex * mutex)
{
    uint32_t status = qTaskEnterCritical();         //进入临界区

    if (mutex->lockedCount <= 0)          //判断互斥锁是否上锁
    {
       
        qTaskExitCritical(status);        //锁定计数为0，未被锁定，直接退出
        return qErrorNoError;
    }

    if (mutex->owner != currentTask)      //判断是否当前任务解锁
    {
        qTaskExitCritical(status);        //不是拥有者释放，认为是非法
        return qErrorOwner;               //返回错误
    }

    if (--mutex->lockedCount > 0)         //互斥锁减一，判断是否释放
    {
      
        qTaskExitCritical(status);        //减1后计数仍不为0, 直接退出临界区，不需要唤醒等待的任务
        return qErrorNoError;             //返回没有错误
    }

    if (mutex->ownerOriginalPrio != mutex->owner->prio)      //判断是否有发生优先级继承
    {
        if (mutex->owner->state == QMRTOS_TASK_STATE_RDY)    //有发生优先级继承，恢复拥有者的优先级，判断是否在运行
        {
            qTaskSchedUnRdy(mutex->owner);     //任务处于就绪状态时，更改任务在就绪表中的位置
            currentTask->prio = mutex->ownerOriginalPrio;    //修改优先级
            qTaskSchedRdy(mutex->owner);
        }
        else
        {
            currentTask->prio = mutex->ownerOriginalPrio;    //其它状态，只需要修改优先级
        }
    }


    if (qEventWaitCount(&mutex->event) > 0)        //检查是否有任务等待
    {

        qTask * task = qEventWakeUp(&mutex->event, (void *)0, qErrorNoError);
												  //如果有的话，则直接唤醒位于队列首部（最先等待）的任务
        mutex->owner = task;                      //赋值占用的任务的参数
        mutex->ownerOriginalPrio = task->prio;
        mutex->lockedCount++;

        if (task->prio < currentTask->prio)       //如果这个任务的优先级更高
        {
            qTaskSched();                        //执行调度
        }
    }
    qTaskExitCritical(status);              //退出临界区
    return qErrorNoError;                   //返回没有错误
}

 /******************************************************************************
 * 函数名称：互斥锁删除函数
 * 函数功能：删除互斥锁
 * 输入参数：qMutex * mutex        互斥锁结构指针
 * 输出参数：删除的个数
 ******************************************************************************/
uint32_t qMutexDestroy(qMutex * mutex)
{
	uint32_t count = 0;
    uint32_t status = qTaskEnterCritical();         //进入临界区

    if (mutex->lockedCount > 0)       //判断信号量是否已经被锁定，未锁定时没有任务等待，不必处理
    {
       
        if (mutex->ownerOriginalPrio != mutex->owner->prio)   //判断是否有发生优先级继承
        {
            if (mutex->owner->state == QMRTOS_TASK_STATE_RDY)   //有发生优先级继承，恢复拥有者的优先级，判断是否在运行
            {
                qTaskSchedUnRdy(mutex->owner);            //任务处于就绪状态时，更改任务在就绪表中的位置
                mutex->owner->prio = mutex->ownerOriginalPrio;  //修改优先级
                qTaskSchedRdy(mutex->owner);
            }
            else
            {
                mutex->owner->prio = mutex->ownerOriginalPrio;  //修改优先级
            }
        }

        count = qEventRemoveAll(&mutex->event, (void *)0, qErrorDel);  //清空事件控制块中的任务
  
        if (count > 0)      //判断有任务删除
        {
            qTaskSched();   //调度
        }
    }

    qTaskExitCritical(status);           //退出临界区
    return count;                        //返回删除的个数
}

 /******************************************************************************
 * 函数名称：互斥锁状态查询函数
 * 函数功能：互斥锁状态查询
 * 输入参数：qMutex * mutex         互斥锁结构指针
			qMutexInfo * info      互斥锁状态查询结构
 * 输出参数：无
 ******************************************************************************/
void qMutexGetInfo(qMutex * mutex, qMutexInfo * info)
{
	uint32_t status = qTaskEnterCritical();        //进入临界区

    //拷贝需要的信息
    info->taskCount = qEventWaitCount(&mutex->event);     
    info->ownerPrio = mutex->ownerOriginalPrio;
    if (mutex->owner != (qTask *)0)
    {
        info->inheritedPrio = mutex->owner->prio;
    }
    else
    {
        info->inheritedPrio = QMRTOS_PRO_COUNT;
    }
    info->owner = mutex->owner;
    info->lockedCount = mutex->lockedCount;

    qTaskExitCritical(status);                     //退出临界区
}
