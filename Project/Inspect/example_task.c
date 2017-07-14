#include "qmRTOS.h"
#include "api_inspect_entry.h"

/*任务测试入口函数，在里面创建优先级不一样的两个任务*/
InspectStu Example_TaskEntry(void)
{
	qInspectStatusSetByID(INSPECT_TASK, INSPECT_STU_SUCCESS);
	return INSPECT_STU_SUCCESS;
}
