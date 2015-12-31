
#include <s3c6410_regs.h>

#define AC97_USE_DMA

#define MUSIC_BUFF     0x52000000
static volatile unsigned long *p_music_data = (volatile unsigned long *)0x52000000;

/* �����оƬ���� */
void codec_read_irq(void)
{
}

/* ����������Ѿ�û�������� */
void pcm_out_underrun_irq(void)
{
	static int cnt = 0;
	int i;

	/* ���ж� */
	AC_GLBCTRL |= (1<<29);
	
	/* ����Ƶ���ݷŵ���������ȥ */
	for (i = 0; i < 16; i++)
	{
		AC_PCMDATA = p_music_data[cnt++];
	}
	
	if (cnt > 150000)
	{
		cnt = 0;
	}

}


/* ������������������Ѿ�������ֵ�� */
void pcm_out_threshold_irq(void)
{
}

void ac97_controller_init(void)
{
	/* ����GPIO��������ac97 */
	GPDCON &= ~(0xfffff);
	GPDCON |= 0x44444;

	/* ��ʼ��AC97���ж� */
#ifndef AC97_USE_DMA
	ac97_int_init();
#endif
	/* code reset */
	AC_GLBCTRL |= (1<<0);
	mdelay(1);
	AC_GLBCTRL &= ~(1<<0);

	/* warm reset */
	AC_GLBCTRL |= (3<<1);
	mdelay(1);
	AC_GLBCTRL &= ~(1<<1);
	mdelay(1);

	/* ����AC97������ */
	AC_GLBCTRL &= ~((3<<12) | (3<<10) | (3<<8));
#ifndef AC97_USE_DMA
	/* ������PIOģʽ */
	AC_GLBCTRL |= ((1<<12) | (1<<10) | (1<<8));
#else
	/* ������DMAģʽ */
	AC_GLBCTRL |= ((2<<12) | (2<<10) | (2<<8));
#endif
	mdelay(1);
	AC_GLBCTRL |= (1<<3);  /* Transfer data enable using AC-link */

	/* read ac97 status */
	AC_CODEC_CMD |= (1<<23);

	if ( (AC_GLBSTAT & 3 ) == 3 )
		printf("ac97 has been active ...\n");
	else
		printf("ac97 init failed ...\n");

}




void ac97_irq(void)
{
	if (AC_GLBSTAT & (1<<22))
	{
		codec_read_irq();
	}
	else if (AC_GLBSTAT & (1<<21))
	{
		/* ֻ������ж� */
		pcm_out_underrun_irq();
	}
	else if (AC_GLBSTAT & (1<<18))
	{
		pcm_out_threshold_irq();
	}

	/* ���ж� */
}


void ac97_int_init(void)
{
	VIC1VECTADDR4 = ac97_irq;	
}

void ac97_int_enable(void)
{
	//AC_GLBCTRL |= ((1<<22) | (1<<21) | (1<<18));
	/* enable fifo empty interupt, 
	 * at first, the fifo is empyt, 
	 * so generate the interrupt, and then intr driver 
	 */
	AC_GLBCTRL |=  (1<<21);  
	
	VIC1INTENABLE |= (1<<4); /* ʹ��INT_AC97 */ 
}

void ac97_int_disable(void)
{
	AC_GLBCTRL &= ~((1<<22) | (1<<21) | (1<<18));
	
	VIC1INTENCLEAR |= (1<<4); 
}

void ac97_write_cmd(unsigned char addr, unsigned short data)
{
	AC_CODEC_CMD = (addr << 16) | data;
	mdelay(1);	
}
void wm9714_init(void)
{
	// set volume, sample rate, mux, enable power output

	// left right DAC VOL control
	ac97_write_cmd(0x0c, 0);
	ac97_write_cmd(0x5c, 0);

	// enable variable rate audio control
	ac97_write_cmd(0x2a, 1);
	// set sample rate to 22050Hz
	//ac97_write_cmd(0x2c, 0x5622);
	ac97_write_cmd(0x2c, 0xBB80);  /* 48K */
	
	//  output mux select and speaker source 
	ac97_write_cmd(0x1c,(1 << 12) | (1 << 9) | (1 << 7) | (1 << 5));
	// set headphone volume
	ac97_write_cmd(0x04, (0xf << 8) | (0xf));
	// set speaker volume
	ac97_write_cmd(0x02, (0x9 << 8) | (0x9));
	ac97_write_cmd(0x6,0);
		
	// enable power
	ac97_write_cmd(0x26, 0);
	ac97_write_cmd(0x3c, 0);
	ac97_write_cmd(0x3e, 3 << 9);

	// enable jake insert
	ac97_write_cmd(0x24, (1 << 4)); 
}
	
void ac97_init()
{
	ac97_controller_init();
	wm9714_init();
}

void ac97_clear(void)
{
	int i;
	for (i = 0; i < 150000; i++)
	{
		p_music_data[i] = 0;
	}
}


void DMA1_int_disable(void)
{
	DMAC1C0Configuration &= ~(1<<15);
	VIC1INTENCLEAR          |= (1<<10);
}


void ac97_dma_init(void)
{
	/* Դ/Ŀ��/���� */
	SDMA_SEL = 0xffffffff; /* ʹ��һ���DMA */

	 /* �������Ĺ���ʱ����DMA��������ʱ�Ӳ���ͬ, Ҫʹ��"ͬ���߼�" */
	DMAC1Sync = 0x0;  

	/* DMAԴ��ַ */
	DMAC1C0SrcAddr = 0x52000000; 

	/* DMAĿ�ĵ�ַ */
	DMAC1C0DestAddr = 0x7F001018;  /* AC_PCMDATA */

	/* ��ʹ�� linked list */
	DMAC1C0LLI = 0;

	/*  */
	// һ�δ���ʱ,��Դ��ַȡ4�ֽ�����, �ŵ�Ŀ�ĵ�ַȥ
        DMAC1C0Control0 =  (2 << 18)|(2 << 21)|(1 << 25)|(1 << 26)|(1 << 28) | ( 1 << 31) ;

	// ����Ĵ���
       DMAC1C0Control1 = 150000;

	/* ʹ��DMA������1��ͨ��0, DestPeripheral = 6 (ac97_pcm_out) 
	 * FlowCntrl = 1 (Memory to peripheral)
	 */	
       DMAC1C0Configuration = (1 | (6 << 6) | (1 << 11) | (1 << 14) | (1 << 15) );

	DMA1_int_init();
}

void dma1_irq(void)
{
	/* ��������dam������1��ͨ��0 */
	ac97_dma_init();

	/* ���ж� */
	DMAC1IntTCClear = 1;
}

void DMA1_int_init(void)
{
	VIC1VECTADDR10 = dma1_irq;
}

void DMA1_int_enable(void)
{
	DMAC1C0Configuration |= (1<<15);
	VIC1INTENABLE            |= (1<<10);
}


void ac97_dma_controller_enable(void)
{
	// enable dmac1, start to response dma transfer request	
	DMAC1Configuration = 0x1 ;	
}


void ac97_test(void)
{
	unsigned char *buf = (unsigned char *)0x52000000;
	unsigned long len = 0;
	int have_begin = 0;
	int nodata_time = 0;
	unsigned char c;

	ac97_init();

	ac97_clear();
	
	/* �Ӵ��ڷ���wav�ļ���0x52000000 */
	printf("\n\ruse gtkterm to send wav file\n\r", len);
	while (1)
	{
		if (getc_nowait(&buf[len]) == 0)
		{
			have_begin = 1;
			nodata_time = 0;
			len++;
		}
		else
		{
			if (have_begin)
			{
				nodata_time++;
			}			
		}

		if (nodata_time == 1000)
		{
			break;
		}
	}
	printf("have get %d bytes data\n\r", len);

#ifndef AC97_USE_DMA
	/* ����: ʹ���ж� */
	printf("use PIO mode(irq) to test AC97\n\r", len);
	ac97_int_enable();
#else
	printf("use DMA mode to test AC97\n\r", len);
	ac97_dma_init();
	ac97_dma_controller_enable();
	DMA1_int_enable();
#endif
	printf("press q to stop and exit\n\r");

	while (1)
	{
		getc_nowait(&c);
		if (c == 'q' || c == 'Q')
		{
#ifndef AC97_USE_DMA
			ac97_int_disable();
#else
			DMA1_int_disable();
#endif
			ac97_write_cmd(0x26, (1<<12));
			return ;
		}
	}
	
}

