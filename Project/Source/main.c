#define NVIC_INT_CTRL       0xE000ED04     //NVIC触发PendSVC寄存器
#define NVIC_PENDSVSET      0x10000000
#define NVIC_SYSPRI2        0xE000ED22
#define NVIC_PENDSV_PRT     0x000000FF

#define MEM32(addr)         *(volatile unsigned long *)(addr)  //写寄存器的宏,volatile关键字不被编译器优化
#define MEM8(addr)          *(volatile unsigned char *)(addr)

/******************************************************************************
 * 函数名称：PendSVC异常触发函数
 * 函数功能：触发PendSVC异常
 * 输入参数：无
 * 输出参数：无 
 ******************************************************************************/
void triggerPendSVC (void)
{
	MEM8(NVIC_SYSPRI2) = NVIC_PENDSV_PRT; //设置PendSVC异常优先级为最低
	MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET;//触发PendSVC异常
}

typedef struct _BlockType_t       //定义一个结构
{
	unsigned long * stackPtr;     //堆栈指针
}BlockType_t;

BlockType_t * blockPtr;    



/******************************************************************************
 * 函数名称：简单延时函数
 * 函数功能：延时
 * 输入参数：延时时间
 * 输出参数：无 
 ******************************************************************************/
void delay(int count)
{
	while (--count > 0);
}

int flag;

unsigned long stackBuffer[1024];//定义一个缓冲区
BlockType_t block;

/******************************************************************************
 * 函数名称：主函数
 * 函数功能：
 * 输入参数：无
 * 输出参数：无 
 ******************************************************************************/
int main()
{
	block.stackPtr = &stackBuffer[1024]; //结构初始化,指向最后一个单元
	blockPtr = &block;
	for(;;)
	{
		flag = 0;
		delay(100);
		flag = 1;
		delay(100);
		
		triggerPendSVC ();  //调用PendSVC异常触发函数
	}
}	

