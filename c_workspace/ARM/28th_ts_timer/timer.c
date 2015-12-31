#include <s3c6410_regs.h>

static volatile int ts_timer_started = 0;
static volatile int cursor_timer_started = 0;

void ts_timer_start(void)
{
	ts_timer_started = 1;
}

void ts_timer_stop(void)
{
	ts_timer_started = 0;
}

void cursor_timer_start(void)
{
	cursor_timer_started = 1;
}

void cursor_timer_stop(void)
{
	cursor_timer_started = 0;
}

void timer2_irq(void)
{
	//printf("timer2_irq\n\r");
	static unsigned int cnt = 0;
	cnt++;
	
	if (ts_timer_started)
	{
		/* 再次启动测量x坐标 */
		ts_timer_stop();
		start_ts_measure();
	}

	if (cursor_timer_started)
	{
		if (cnt % 4 == 0)
		{
			show_hide_cursor();
		}
	}
	
	/* 清中断 */
	TINT_CSTAT |= (1<<7);
}

void timer2_init(void)
{
	/* 设置时钟源 */
	TCFG0 &= ~(0xff<<8);
	TCFG0 |= (1<<8);
	TCFG1  &= ~(0xf<<8);  
	                             /* Timer2 input clock Frequency = PCLK / ( {prescaler value + 1} ) / {divider value} 
	                    *      = 66500000 / (1+1) / 1
	                    *      = 33250000
	                    */

	/* 设置TCMPB2, TCNTB2 */
	TCNTB2 = 33250 * 50;   /* 倒数到0时,耗时50ms */							

	TCON |= (1<<15);    /* auto-reload */

	VIC0VECTADDR25 = timer2_irq;
	VIC0INTENABLE    |= (1<<25);

	timer2_start();
}

void timer2_start(void)
{
	TCON |= (1<<13);   /* set manual update */
	TCON |= (1<<12);   /* start timer 2 */
	TCON &= ~(1<<13); /* clean manual update */

	TINT_CSTAT |= (1<<2); /* Timer 2 Interrupt Enable */
}

void timer2_stop(void)
{
	TCON &= ~(1<<12);   /* stop timer 2 */
	TINT_CSTAT &= ~(1<<2); /* Timer 2 Interrupt disable */
}


