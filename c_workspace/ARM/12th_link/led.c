void delay()
{
	volatile int i = 0x10000;
	while (i--);
}

volatile int i = 0;

volatile int j = 0x12345678;
volatile int k = 0;
volatile int g;

int main()
{
	
	volatile unsigned long *gpmcon = (volatile unsigned long *)0x7F008820;
	volatile unsigned long *gpmdat = (volatile unsigned long *)0x7F008824;
	
	/* gpm0,1,2,3设为输出引脚 */
	*gpmcon = 0x1111;
	
	while (1)
	{
		*gpmdat = i;
		i++;
		if (i == 16)
			i = 0;
		delay();
	}
	
	return 0;
}

