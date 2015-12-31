#include "common.h"

int puts(char *str)
{
	int i = 0;
	while (str[i])
	{
		putc(str[i]);
		i++;
	}
}

