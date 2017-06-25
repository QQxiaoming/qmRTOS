#define NVIC_INT_CTRL       0xE000ED04     //NVIC����PendSVC�Ĵ���
#define NVIC_PENDSVSET      0x10000000
#define NVIC_SYSPRI2        0xE000ED22
#define NVIC_PENDSV_PRT     0x000000FF

#define MEM32(addr)         *(volatile unsigned long *)(addr)  //д�Ĵ����ĺ�,volatile�ؼ��ֲ����������Ż�
#define MEM8(addr)          *(volatile unsigned char *)(addr)

/******************************************************************************
 * �������ƣ�PendSVC�쳣��������
 * �������ܣ�����PendSVC�쳣
 * �����������
 * ����������� 
 ******************************************************************************/
void triggerPendSVC (void)
{
	MEM8(NVIC_SYSPRI2) = NVIC_PENDSV_PRT; //����PendSVC�쳣���ȼ�Ϊ���
	MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET;//����PendSVC�쳣
}

typedef struct _BlockType_t       //����һ���ṹ
{
	unsigned long * stackPtr;     //��ջָ��
}BlockType_t;

BlockType_t * blockPtr;    



/******************************************************************************
 * �������ƣ�����ʱ����
 * �������ܣ���ʱ
 * �����������ʱʱ��
 * ����������� 
 ******************************************************************************/
void delay(int count)
{
	while (--count > 0);
}

int flag;

unsigned long stackBuffer[1024];//����һ��������
BlockType_t block;

/******************************************************************************
 * �������ƣ�������
 * �������ܣ�
 * �����������
 * ����������� 
 ******************************************************************************/
int main()
{
	block.stackPtr = &stackBuffer[1024]; //�ṹ��ʼ��,ָ�����һ����Ԫ
	blockPtr = &block;
	for(;;)
	{
		flag = 0;
		delay(100);
		flag = 1;
		delay(100);
		
		triggerPendSVC ();  //����PendSVC�쳣��������
	}
}	

