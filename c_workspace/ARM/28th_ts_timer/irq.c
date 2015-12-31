
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
	
	EINT0PEND   = 0x3f;  /* ���ж� */

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

	EINT0PEND   = 0x3f;  /* ���ж� */
}

void irq_init(void)
{
	key_irq_init();
	uart_irq_init();
}





void key_irq_init(void)
{
	/* ����GPIO����Ϊ�ж����� */
	/* GPN0~5 ��Ϊ�ж����� */
	GPNCON &= ~(0xfff);
	GPNCON |= 0xaaa;

	/* �����жϴ�����ʽΪ: ˫���ش��� */
	EINT0CON0 &= ~(0xfff);
	EINT0CON0 |= 0x777;

	/* ʹ���ж� */
	EINT0MASK &= ~(0x3f);

	/* ���жϿ�������ʹ����Щ�ж� */
	VIC0INTENABLE |= (0x3); /* bit0: eint0~3, bit1: eint4~11 */ 

	VIC0VECTADDR0 = eint0_3_irq;
	VIC0VECTADDR1 = eint4_11_irq;

	/* �������ȼ� */
}


void do_irq(void)
{
	int i = 0;

	void (*the_isr)(void);

	if (VIC0IRQSTATUS)
	{
		the_isr = VIC0ADDRESS;
			
		/* 2.1 �ֱ����ĸ��ж� */
		/* 2.2 �������Ĵ����� */	
		/* 2.3 ���ж� */	

		the_isr();
		
		VIC0ADDRESS = 0;
	}
	else if (VIC1IRQSTATUS)
	{
		the_isr = VIC1ADDRESS;
			
		/* 2.1 �ֱ����ĸ��ж� */
		/* 2.2 �������Ĵ����� */	
		/* 2.3 ���ж� */	

		the_isr();
		VIC1ADDRESS = 0;
	}
}


