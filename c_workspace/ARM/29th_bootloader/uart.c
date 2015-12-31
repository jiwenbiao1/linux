#include <s3c6410_regs.h>

//#define ENABLE_FIFO

void init_uart(void)
{
	GPACON &= ~0xff;
	GPACON |= 0x22;
	
	/* ULCON0 */
	ULCON0 = 0x3;  /* 数据位:8, 无较验, 停止位: 1, 8n1 */
	UCON0  = 0x5 | (1<<9);  /* 使能UART发送、接收, tx interrupt request type = level */
#ifdef ENABLE_FIFO
	UFCON0 = 0x07 | (1<<6); /* FIFO enable, tx fifo trigger level = 16 bytes */	
#else
	UFCON0 = 0x00; /* FIFO disable */
#endif
	UMCON0 = 0;
	
	/* 波特率 */
	/* DIV_VAL = (PCLK / (bps x 16 ) ) - 1 
	 * bps = 57600
	 * DIV_VAL = (66500000 / (115200 x 16 ) ) - 1 
	 *         = 35.08
	 */
	UBRDIV0   = 35;

	/* x/16 = 0.08
	 * x = 1
	 */
	UDIVSLOT0 = 0x1;

}

unsigned char getc(void)
{
#ifdef ENABLE_FIFO
	while ((UFSTAT0 & (1<<6)) == 0 && (UFSTAT0 & 0x3f) == 0);
#else	
	while ((UTRSTAT0 & (1<<0)) == 0);
#endif
	
	return URXH0;
}

