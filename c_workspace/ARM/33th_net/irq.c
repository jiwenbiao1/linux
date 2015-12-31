
#include <s3c6410_regs.h>


void eint0_3_irq(void)
{
	int i;
	
	printf("eint0_3_irq\n\r");  /* K1~K4 */
	for (i = 0; i < 4; i ++)
	{
		if (EINT0PEND & (1<<i))
		{
			if (GPNDAT & (1<<i))
			{
				printf("K%d released\n\r", i+1);
			}
			else
			{
				printf("K%d pressed\n\r", i+1);
			}
		}
	}
	
	EINT0PEND   = 0x3f;  /* 清中断 */

}

void eint4_11_irq(void)
{
	int i;
	printf("eint4_11_irq\n\r"); /* K5~K6 */
	for (i = 4; i < 6; i ++)
	{
		if (EINT0PEND & (1<<i))
		{
			if (GPNDAT & (1<<i))
			{
				printf("K%d released\n\r", i+1);
			}
			else
			{
				printf("K%d pressed\n\r", i+1);
			}
		}
	}

	EINT0PEND   = 0x3f;  /* 清中断 */
}

void irq_init(void)
{
	key_irq_init();
	uart_irq_init();
}





void key_irq_init(void)
{
	/* 配置GPIO引脚为中断引脚 */
	/* GPN0~5 设为中断引脚 */
	GPNCON &= ~(0xfff);
	GPNCON |= 0xaaa;

	/* 设置中断触发方式为: 双边沿触发 */
	EINT0CON0 &= ~(0xfff);
	EINT0CON0 |= 0x777;

	/* 使能中断 */
	EINT0MASK &= ~(0x3f);

	/* 在中断控制器里使能这些中断 */
	VIC0INTENABLE |= (0x3); /* bit0: eint0~3, bit1: eint4~11 */ 

	VIC0VECTADDR0 = eint0_3_irq;
	VIC0VECTADDR1 = eint4_11_irq;

	/* 设置优先级 */
}


void do_irq(void)
{
	int i = 0;

	void (*the_isr)(void);

	if (VIC0IRQSTATUS)
	{
		the_isr = VIC0ADDRESS;
			
		/* 2.1 分辨是哪个中断 */
		/* 2.2 调用它的处理函数 */	
		/* 2.3 清中断 */	

		the_isr();
		
		VIC0ADDRESS = 0;
	}
	else if (VIC1IRQSTATUS)
	{
		the_isr = VIC1ADDRESS;
			
		/* 2.1 分辨是哪个中断 */
		/* 2.2 调用它的处理函数 */	
		/* 2.3 清中断 */	

		the_isr();
		VIC1ADDRESS = 0;
	}
}


