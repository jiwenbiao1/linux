
#include <stdio.h>


extern void a_fun(void);

int main(void)
{
	printf("hello, it is main\n\r");
	a_fun();
	return 0;
}