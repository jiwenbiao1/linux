void delay(int count)
{
	volatile int i = count;
	while (i--);
}

int xxxxx(int start)
{
	int i = start;
	
	volatile unsigned long *gpmcon = (volatile unsigned long *)0x7F008820;
	volatile unsigned long *gpmdat = (volatile unsigned long *)0x7F008824;
	
	/* gpm0,1,2,3��Ϊ������� */
	*gpmcon = (*gpmcon & ~0xffff) | 0x1111;
	
	while (1)
	{
		*gpmdat = (*gpmdat & ~0xf) | i;
		i++;
		if (i == 16)
			i = 0;
		delay(0x10000);
	}
	
	return 0;
}

