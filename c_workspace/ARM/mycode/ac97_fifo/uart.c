#define GPACON    (*((volatile unsigned long *)0x7F008000))

#define ULCON0    (*((volatile unsigned long *)0x7F005000))
#define UCON0     (*((volatile unsigned long *)0x7F005004))
#define UFCON0    (*((volatile unsigned long *)0x7F005008))
#define UMCON0    (*((volatile unsigned long *)0x7F00500C))
#define UFSTAT0   (*((volatile unsigned long *)0x7F005018))
#define UTXH0     (*((volatile unsigned char *)0x7F005020))
#define URXH0     (*((volatile unsigned char *)0x7F005024))
#define UBRDIV0   (*((volatile unsigned short *)0x7F005028))
#define UDIVSLOT0 (*((volatile unsigned short *)0x7F00502C))


void init_uart(void)
{
	/* ����GPA0,GPA1��ΪRxD0, TxD0 */
	GPACON &= ~0xff;
	GPACON |= 0x22;
	
	ULCON0 = 0x3;  /* 8������λ */
	UCON0  = 0x5;  /* ʹ��UART, ʱ��ԴPCLK */
	UFCON0 = 0x1;  /* FIFO enable */
	UMCON0 = 0;    /* ������ */

	/* ���ò����� */
	/* DIV_VAL = (PCLK / (bps x 16 ) ) - 1 
	 * bps  = 115200
	 * PCLK = 66500000 Hz
	 * DIV_VAL = 66500000 / (115200 x 16) - 1 = 35.08
	 * x/16 = 0.08
	 *    x = 1
	 * 
	 */
	UBRDIV0   = 35; /* ʵ�ʲ����� = 115451 */
	UDIVSLOT0 = 0x1;
}

char getc(void)
{
	/* �ж��������� */
	/* �����ݵ�״̬:
	 * 1. UFCON0 & (1<<6) == 1
	 * ��
	 * 2. UFCON0 & (1<<6) == 0, &&, (UFCON0 & 0x3f) > 0
	 * ������: UFCON0 & (1<<6) == 0, && (UFCON0 & 0x3f) == 0
	 */

	while ((UFSTAT0 & 0x7f) == 0);
	
	return URXH0;
}

void putc(char c)
{
	/* �ж�״̬ */
	while (UFSTAT0 & (1<<14));
	UTXH0 = c;
}

