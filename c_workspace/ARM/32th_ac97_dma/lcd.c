


#include <s3c6410_regs.h>


#define  VSPW         9
#define  VBPD          1
#define  LINEVAL     271
#define  VFPD          1


#define  HSPW         40    
#define  HBPD          1
#define  HOZVAL      479
#define  HFPD          1

#define LeftTopX     0
#define LeftTopY     0

#define RightBotX   479
#define RightBotY   271

#define FRAME_BUFFER   0x54000000


unsigned int fb_base_addr;
unsigned int bpp;
unsigned int xsize;
unsigned int ysize;

void palette_init(void)
{
	int i;
	volatile unsigned long *p = (volatile unsigned long *)WIN0_PALENTRY0;
	
	WPALCON |= (1<<9);   /* 允许CPU修改调色板 */


	WPALCON &= ~(0x7);   /*  */
	WPALCON |= 1;            /* 调色板的数据格式: 24-bit ( 8:8:8 ) */

	p[0] = 0x000000;
	p[1] = 0x00ff00;
	p[2] = 0xff0000;
	p[3] = 0x0000ff;
	p[4] = 0xffffff;

	for (i = 0; i <256; i++)
	{
		//p[i] = ;
	}
	

	WPALCON &= ~(1<<9);  /* 禁止CPU访问调色板 */
	
}

void clean_screem(void)
{
	int x;
	int y;
	int cnt = 0;
	
	volatile unsigned char *p = (volatile unsigned char *)fb_base_addr;
	for (x = 0; x <=HOZVAL; x++)
		for (y = 0; y <= LINEVAL; y++)
			p[cnt++] = 0;
}

void lcd_init(void)
{
	/* 1. 设置相关GPIO引脚用于LCD */
	GPICON = 0xaaaaaaaa;  /* gpi0~gpi15用作lcd_vd[0~15] */
	GPJCON = 0xaaaaaaa;   /* gpj0~gpi11用作lcd */
	//GPFCON &= ~(0x3<<28);
	//GPFCON |=  (1<<28);    /* GPF14用作背光使能信号 */
	
	GPECON &= ~(0xf);
	GPECON |= (0x1);          /* GPE0用作LCD的on/off信号 */
	
	/* 2. 初始化6410的display controller 
	 * 2.1 hsync,vsync,vclk,vden的极性和时间参数
	 * 2.2 行数、列数(分辨率),象素颜色的格式
	 * 2.3 分配显存(frame buffer),写入display controller
	 */

	MIFPCON &= ~(1<<3);   /* Normal mode */

	SPCON    &= ~(0x3);
	SPCON    |= 0x1;            /* RGB I/F style */

#if 0
	VIDCON0 &= ~((3<<26) | (3<<17) | (0xff<<6));     /* RGB I/F, RGB Parallel format,  */
	VIDCON0 |= ((2<<6) | (1<<4) | (0x3<<2));      /* vclk== 27MHz Ext Clock input / (CLKVAL+1) = 27/3 = 9MHz */
#else
	VIDCON0 &= ~((3<<26) | (3<<17) | (0xff<<6)  | (3<<2));     /* RGB I/F, RGB Parallel format,  */
	VIDCON0 |= ((14<<6) | (1<<4) );      /* vclk== HCLK / (CLKVAL+1) = 133/15 = 9MHz */
#endif

	VIDCON1 &= ~(1<<7);   /* 在vclk的下降沿获取数据 */
	VIDCON1 |= ((1<<6) | (1<<5));  /* HSYNC高电平有效, VSYNC高电平有效, */

	VIDTCON0 = (VBPD << 16) | (VFPD << 8) | (VSPW << 0);
	VIDTCON1 = (HBPD << 16) | (HFPD << 8) | (HSPW << 0);
	VIDTCON2 = (LINEVAL << 11) | (HOZVAL << 0);

	WINCON0 &= ~(0xf << 2);
	WINCON0 |= (0x3<<2) | (1<<17);    /* 8 BPP (palletized), byte swap */

	VIDOSD0A = (LeftTopX<<11) | (LeftTopY << 0);
	VIDOSD0B = (RightBotX<<11) | (RightBotY << 0);
	VIDOSD0C = (LINEVAL + 1) * (HOZVAL + 1) / 4;

	VIDW00ADD0B0 = FRAME_BUFFER;
	VIDW00ADD1B0 =  (((HOZVAL + 1)*1 + 0) * (LINEVAL + 1)) & (0xffffff);
	                                /* VBASEL = VBASEU + (LINEWIDTH+OFFSIZE) x (LINEVAL+1) 
                          *        = 0 + (480*1 + 0) * 272
                          *        = 
	                     */
	//VIDW00ADD2 =  HOZVAL + 1;


	/* 设置调色板 */
	palette_init();	

	pwm_set(2);
									
	fb_base_addr = FRAME_BUFFER;
	xsize = HOZVAL + 1;
	ysize = LINEVAL + 1;
	bpp   = 8;

	clean_screem();
}


void backlight_enable(void)
{
	//GPFDAT |= (1<<14);
}

void backlight_disable(void)
{
	//GPFDAT &= ~(1<<14);
}


void lcd_on(void)
{
	GPEDAT |= (1<<0);
	/* 等待10 frame */
}

void lcd_off(void)
{
	GPEDAT &= ~(1<<0);
}

void displaycon_on(void)
{
	VIDCON0 |= 0x3;
	WINCON0 |= 1;
}

void displaycon_off(void)
{
	VIDCON0 &= ~0x3;
	WINCON0 &= ~1;
}

void lcd_enable(void)
{
	/* 使能LCD本身 */
	lcd_on();
	
	/* 打开背光 */
	//backlight_enable();
	pwm_start();
	
	/* 使能display controller */
	displaycon_on();
}

void lcd_disable(void)
{
	/* 关闭背光 */
	/* 关闭LCD本身 */
	/* 关闭display controller */
}

void draw_line(void)
{
	DrawLine(0,0, 0,271, 0);
	DrawLine(0,0, 479,0, 1);
	DrawLine(0,0, 479,271, 2);
	DrawLine(0,271, 479,0, 3);
	DrawLine(0,271, 479,271, 1);
	DrawLine(479,271, 479,0, 2);
	DrawLine(0,136, 479,136, 3);
	DrawLine(240,0, 240,271, 1);
}

void display_red(void)
{
	volatile unsigned char *p = (volatile unsigned char *)FRAME_BUFFER;
	int x, y;
	int cnt = 0;
	unsigned char colors[] = {0, 1, 2, 3};
	static int color_idx = 0;
	
	for (y = 0; y <= LINEVAL; y++)
	{
		for (x = 0; x <= HOZVAL; x++)
		{
			p[cnt++] =colors[color_idx] ;  /* red */
		}
	}

	color_idx++;
	if (color_idx == 5)
		color_idx = 0;
}

void lcd_test(void)
{
	unsigned char c;
	static int lcdon = 0;
	static int blon = 0;
	static int dispon = 0;

	lcd_init();
	
	while (1)
	{
		printf("********LCD TEST MENU********\n\r");
		printf("[L] enable/disable LCD\n\r");
		printf("[B] enable/disable back light\n\r");
		printf("[C] enable/disable s3c6410 display controller\n\r");
		printf("[D] display color\n\r");
		printf("[I]  draw line\n\r");
		printf("[Q] quit\n\r");

		do {
			c = getc();
			if (c == '\n' || c == '\r')
			{
				printf("\n\r");
			}
			else
			{
				putc(c);
			}
		} while (c == '\n' || c == '\r');

		switch (c) {
			case 'l':
			case 'L':
			{
				if (lcdon)
				{
					lcd_off();
					printf("LCD off\n\r");
				}
				else
				{
					lcd_on();
					printf("LCD on\n\r");
				}
				lcdon = !lcdon;
				break;
			}


			case 'b':
			case 'B':
			{
				pwm_menu();
				break;
			}


			case 'c':
			case 'C':
			{
				if (dispon)
				{
					displaycon_off();
					printf("Display controller off\n\r");
				}
				else
				{
					displaycon_on();
					printf("Display controller on\n\r");
				}
				blon = !blon;
				break;
			}

			case 'd':
			case 'D':
			{
				display_red();
				break;
			}

			case 'i':
			case 'I':
			{
				draw_line();
				break;
			}

			case 'q':
			case 'Q':
			{
				return ;
				break;
			}

		}

	}
}

