#include "qmRTOS.h"
#include <ARMCM3.h>

#define NVIC_INT_CTRL       0xE000ED04     //NVIC触发PendSVC寄存器
#define NVIC_PENDSVSET      0x10000000
#define NVIC_SYSPRI2        0xE000ED22
#define NVIC_PENDSV_PRT     0x000000FF

#define MEM32(addr)         *(volatile unsigned long *)(addr)  //写寄存器的宏,volatile关键字不被编译器优化
#define MEM8(addr)          *(volatile unsigned char *)(addr)

/******************************************************************************
 * 函数名称：PendSV异常处理函数
 * 函数功能：PendSV异常处理
 * 输入参数：无
 * 输出参数：无 
 ******************************************************************************/
__asm void PendSV_Handler (void)		//保存内核寄存器必须使用汇编代码，采用内嵌汇编，函数以__asm开头
{										//注意函数名称必须是PendSV_Handler
	IMPORT  currentTask               // 使用import导入C文件中声明的全局变量
    IMPORT  nextTask                  // 类似于在C文文件中使用extern int variable
    
    MRS     R0, PSP                   // 获取当前任务的堆栈指针
    CBZ     R0, PendSVHandler_nosave  // if 这是由tTaskSwitch触发的(此时，PSP肯定不会是0了，0的话必定是tTaskRunFirst)触发
                                      // 不清楚的话，可以先看tTaskRunFirst和tTaskSwitch的实现
    STMDB   R0!, {R4-R11}             //     那么，我们需要将除异常自动保存的寄存器这外的其它寄存器自动保存起来{R4, R11}
                                      //     保存的地址是当前任务的PSP堆栈中，这样就完整的保存了必要的CPU寄存器,便于下次恢复
    LDR     R1, =currentTask          //     保存好后，将最后的堆栈顶位置，保存到currentTask->stack处    
    LDR     R1, [R1]                  //     由于stack处在结构体stack处的开始位置处，显然currentTask和stack在内存中的起始
    STR     R0, [R1]                  //     地址是一样的，这么做不会有任何问题

PendSVHandler_nosave                  // 无论是tTaskSwitch和tTaskSwitch触发的，最后都要从下一个要运行的任务的堆栈中恢复
                                      // CPU寄存器，然后切换至该任务中运行
    LDR     R0, =currentTask          // 好了，准备切换了
    LDR     R1, =nextTask             
    LDR     R2, [R1]  
    STR     R2, [R0]                  // 先将currentTask设置为nextTask，也就是下一任务变成了当前任务
 
    LDR     R0, [R2]                  // 然后，从currentTask中加载stack，这样好知道从哪个位置取出CPU寄存器恢复运行
    LDMIA   R0!, {R4-R11}             // 恢复{R4, R11}。为什么只恢复了这么点，因为其余在退出PendSV时，硬件自动恢复

    MSR     PSP, R0                   // 最后，恢复真正的堆栈指针到PSP  
    ORR     LR, LR, #0x04             // 标记下返回标记，指明在退出LR时，切换到PSP堆栈中(PendSV使用的是MSP) 
    BX      LR                        // 最后返回，此时任务就会从堆栈中取出LR值，恢复到上次运行的位置
}

/******************************************************************************
 * 函数名称：初始任务运行函数
 * 函数功能：运行初始任务
 * 输入参数：无
 * 输出参数：无 
 ******************************************************************************/
void qTaskRunFirst()
{
	__set_PSP(0);      //PSP赋值为0，在ARMCM3.h实现
	
	MEM8(NVIC_SYSPRI2) = NVIC_PENDSV_PRT; //设置PendSVC异常优先级为最低
	MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET;//触发PendSVC异常
}

/******************************************************************************
 * 函数名称：触发PendSVC异常函数
 * 函数功能：触发PendSVC异常
 * 输入参数：无
 * 输出参数：无 
 ******************************************************************************/
void qTaskSwitch()
{
	MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET;//触发PendSVC异常
}
