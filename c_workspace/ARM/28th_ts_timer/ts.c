#include <s3c6410_regs.h>


static int x_min, x_max, x_mid;
static int y_min, y_max, y_mid;

static volatile int x, y;
static volatile unsigned char has_get_xy = 0;

void wait_for_pen_down(void)
{
	has_get_xy = 0;
	ADCTSC = 0xd3;
//	ADCTSC  &= ~(1<<8); /* Detect Stylus Down Interrupt Signal. */	
}

void wait_for_pen_up(void)
{
	ADCTSC = 0x1d3;
//	ADCTSC  |= (1<<8); /* Detect Stylus Up Interrupt Signal. */
}

void enter_to_measure_x(void)
{
	/* (AUTO_PST=0, XY_PST: 2b01 */
	ADCTSC = 1;
}

void start_adc(void)
{
	ADCCON |= (1<<0);
}

void enter_to_measure_y(void)
{
	/* (AUTO_PST=0, XY_PST: 2b10 */
	ADCTSC = 2;
}

void start_ts_measure(void)
{
	enter_to_measure_x();
	start_adc();	
}

void irq_pen_down_up(void)
{
	if (ADCDAT0 & (1<<15))
	{
		//printf("pen up\n\r");
		//ADCTSC  &= ~(1<<8); /* Detect Stylus Down Interrupt Signal. */	
		ts_timer_stop();
		wait_for_pen_down();
	}
	else
	{
		//printf("pen down\n\r");
		//wait_for_pen_up();
		ts_timer_stop();
		enter_to_measure_x();
		start_adc();
	}

	ADCUPDN = 0;
	ADCCLRINTPNDNUP = 0;
}

void irq_adc(void)
{
	if ((ADCDAT0 & (1<<15)) == 0) /* 触摸笔仍然处于按下状态 */
	{
		/* 如果是x坐标, 记录下来, 启动Y坐标的ADC */
		if ((ADCTSC & 0x3) == 1)
		{
			x = ADCDAT0 & 0xfff;
			/* 清中断 */
			ADCCLRINT = 0;
			enter_to_measure_y();
			start_adc();
		}
		else if ((ADCTSC & 0x3) == 2)
		{
			/* 如果是Y坐标, 记录下来, wait_for_pen_up */
			y = ADCDAT1 & 0xfff;
			/* 清中断 */
			ADCCLRINT = 0;
			wait_for_pen_up();
			has_get_xy = 1;
			ts_timer_start();
			//printf("[x,y] = [%d, %d]\n\r", x, y);
		}
	}
	else
	{
		/* 在ADC期间, 触摸笔已经松开了 */
		ts_timer_stop();
		wait_for_pen_down();
	}
}


void ts_irq_init(void)
{
	VIC1VECTADDR30 = irq_pen_down_up;
	VIC1VECTADDR31 = irq_adc;
	VIC1INTENABLE |= ((1<<30) | (1<<31)); /* 使能INT_PENDNUP, INT_ADC */ 
}

void ts_init(void)
{
	/* 一般的设置:比如时钟 */
	ADCCON &= ~((0xff << 6) | (1<<2));

	/* 12-bit A/D conversion, bit16
      * A/D converter prescaler enable, bit14 
      * A/D converter prescaler value = 13, ADC CLK = 66.5MHz/(255+1)=4.75MHz, bit[13:6]
      */
	ADCCON |= (1<<16) | (1<<14) | (255<<6);  

	ADCDLY = 0xffff;

	timer2_init();
		
	/* 中断设置 */
	ts_irq_init();

	wait_for_pen_down();
}

int abs(int a, int b)
{
	if (a > b)
		return a - b;
	else
		return b - a;
}

void ts_calibrate(void)
{
	int x_pre;
	int y_pre;
	
	/* 1. 在左上角(10,10)的地方显示"+" 
	 */
	has_get_xy = 0;
	show_cross(10, 10);
	while (!has_get_xy);
	has_get_xy = 0;
	x_min = x_pre =  x;
	y_min = y_pre = y;
	hide_cross(10, 10);

	/* 2. 在右上角(470,10)的地方显示"+" 
	 */
	show_cross(470, 10);
	while (!has_get_xy || \
		    ((abs(x, x_pre) < 300) && (abs(y, y_pre) < 300)));
	has_get_xy = 0;
	x_pre = x;
	y_pre = y;
	x_max += x;
	y_min += y;
	hide_cross(470, 10);

	/* 3. 在右下角(470,262)的地方显示"+" 
	 */
	show_cross(470, 262);
	while (!has_get_xy || \
		    ((abs(x, x_pre) < 300) && (abs(y, y_pre) < 300)));
	has_get_xy = 0;
	x_pre = x;
	y_pre = y;
	x_max += x;
	y_max += y;
	hide_cross(470, 262);

	/* 4. 在左下角(10,262)的地方显示"+" 
	 */
	show_cross(10, 262);
	while (!has_get_xy || \
		    ((abs(x, x_pre) < 300) && (abs(y, y_pre) < 300)));
	has_get_xy = 0;
	x_pre = x;
	y_pre = y;
	x_min += x;
	y_max += y;
	hide_cross(10, 262);

	x_min /= 2;
	y_min /= 2;
	x_max /= 2;
	y_max /= 2;

	printf("x_min = %d, x_max = %d\n\r", x_min, x_max);
	printf("y_min = %d, y_max = %d\n\r", y_min, y_max);
}

void ts_test(void)
{
	unsigned char c = 0;
	int lcd_x = 10, lcd_y = 10;
	
	printf("press q to exit\n\r");
	
	while (1)
	{
		while (!has_get_xy)
		{
			getc_nowait(&c);
			if (c == 'q' || c == 'Q')
			{
				return ;
			}
		}
		
		hide_cross(lcd_x, lcd_y);
		has_get_xy = 0;
		lcd_x = (470 - 10)*(x - x_min) / (x_max - x_min) + 10;
		lcd_y = (262 - 10)*(y - y_min) / (y_max - y_min) + 10;
		if (lcd_x < 0 || lcd_x > 470 || lcd_y < 0 || lcd_y > 271)
		{
			printf("ts error!\n\r");
		}
		else
		{
			show_cross(lcd_x, lcd_y);
		}
	}
	
}
	
