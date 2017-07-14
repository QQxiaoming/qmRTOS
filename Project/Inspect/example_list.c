#include "qmRTOS.h"
#include "api_inspect_entry.h"

InspectStu Example_ListEntry(void)
{
	qInspectStatusSetByID(INSPECT_LIST, INSPECT_STU_SUCCESS);
	return INSPECT_STU_SUCCESS;
}
