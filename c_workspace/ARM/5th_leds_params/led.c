void delay(int a, int b, int c, int d, int e, int f, int g, int h)
{
	volatile int i = a+b+c+d+e+f+g+h;
	while (i--);
}

int xxxxx(int start, int end, int a, int b, int c, int d)
{
	int i = start + a + b + c + d;
	
	volatile unsigned long *gpmcon = (volatile unsigned long *)0x7F008820;
	volatile unsigned long *gpmdat = (volatile unsigned long *)0x7F008824;
	
	/* gpm0,1,2,3��Ϊ������� */
	*gpmcon = 0x1111;
	
	while (1)
	{
		*gpmdat = i;
		i++;
		if (i == end)
			i = start;
		delay(0, 1, 2, 3, 4, 5, 6, 0x10000);
	}
	
	return 0;
}

