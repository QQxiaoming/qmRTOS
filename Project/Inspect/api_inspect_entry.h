#ifndef API_INSPECT_ENTRY_H
#define API_INSPECT_ENTRY_H

typedef enum {
    INSPECT_STU_START   =  0xFF,  //开始检查
    INSPECT_STU_ERROR   =  0x01,  //错误
    INSPECT_STU_SUCCESS =  0x00,  //成功
}InspectStu;                      //检查状态

typedef enum {
    INSPECT_TASK = 0,      //任务
	INSPECT_LIST,          //双向链表
//  INSPECT_EVENT,         //事件控制块
//	INSPECT_SEM,           //计数信号量
//	INSPECT_MBOX,          //邮箱
//	INSPECT_MEMBLOCK,      //存储块
//	INSPECT_FLAGGROUP,     //事件标志组
//	INSPECT_MUTEX,         //互斥锁
	INSPECT_NUM
}InspectID;                //检查项目

typedef struct _Inspect_def {
    InspectID   inspectID;                //检查项目ID
    InspectStu  status;                   //检查状态
    InspectStu   (*Inspectfunc) (void);   //检查项目入口
    char    name[6];                      //检查项目名称
}Inspect_def;                             //巡检结构

void InspectTaskstart(void);

InspectStu qInspectStatusSetByID(InspectID inspectid,InspectStu inspectstu);
InspectStu qInspectByID(InspectID inspectid);
#endif /*API_INSPECT_ENTRY_H*/
