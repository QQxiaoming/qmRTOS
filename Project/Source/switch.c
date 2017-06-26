/******************************************************************************
 * 函数名称：PendSV异常处理函数
 * 函数功能：PendSV异常处理
 * 输入参数：无
 * 输出参数：无 
 ******************************************************************************/
__asm void PendSV_Handler (void)		//保存内核寄存器必须使用汇编代码，采用内嵌汇编，函数以__asm开头
{										//注意函数名称必须是PendSV_Handler
	IMPORT blockPtr       //导入变量
	
	LDR R0, =blockPtr	  //加载blockPtr指针变量地址到R0
	LDR R0, [R0]          //加载指针变量地址的值，也就是加载BlockType_t结构的起始地址
	LDR R0, [R0]		  //加载BlockType_t结构起始处4个字节的内容也就是stackPtr的值，其指向stackBuffer[1024]。

	STMDB R0!, {R4-R11}   //STMDB是批量往内存地址写寄存器，D表示递减从高地址往低地址，B表示在写寄存器之前先把R0寄存器递减再往里写R4到R11
						  //!变量表示最后一个单元地址保存在R0里面
	
	LDR R1, =blockPtr     //重新加载blockPtr
	LDR R1, [R0]
	STR R0, [R1]		  //将最后的地址写入到blockPtr中
	
    ADD R4, R4, #1		  // 修改部分寄存器，用于测试，ADD加法指令
    ADD R5, R5, #1
    
    LDMIA R0!, {R4-R11}    // 恢复R4-R11寄存器
    
    BX      LR            // 退出异常
	NOP					  //对齐，消除警告
}
