#include "qmRTOS.h"
#include "api_inspect_entry.h"

qList list;
qNode node[8];

InspectStu Example_ListEntry(void)
{
	int i;
	dprintf("initial......\n");
	qListInit(&list);  //初始化链表
	
	for (i = 0; i < 8; i++)          //在链表内插入结点
    {
        qNodeInit(&node[i]);
        qListAddFirst(&list, &node[i]);
		if(qListCount(&list) != (i + 1))
		{
			dprintf("add node error\n");
			qInspectStatusSetByID(INSPECT_LIST,INSPECT_STU_ERROR);
			return INSPECT_STU_ERROR;
		}
    }

    for (i = 0; i < 8; i++)          //在链表内删除结点
    {
        qListRemoveFirst(&list);
		if(qListCount(&list) != (8 - (i + 1)))
		{
			dprintf("delete node error\n");
			qInspectStatusSetByID(INSPECT_LIST,INSPECT_STU_ERROR);
			return INSPECT_STU_ERROR;
		}
    }
	
	dprintf("List is success\n");
	qInspectStatusSetByID(INSPECT_LIST, INSPECT_STU_SUCCESS);
	return INSPECT_STU_SUCCESS;
}
