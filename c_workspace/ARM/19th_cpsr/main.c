
#include "uart.h"

char *str = "hello uplooking\n\r";

void hello(void)
{
	printf("%s", str);
}

int print_cpsr(unsigned int cpsr, char *why)
{	
	printf("print_cpsr %s cpsr = 0x%x\n\r", why, cpsr);

}
int main(unsigned int cpsr, char *why)
{
	int a, b;
	
//	printf("main %s cpsr = 0x%x\n\r", why, cpsr);

	while (1)
	{
		printf("please enter two number: \n\r");
		scanf("%d %d", &a, &b);
		printf("\n\r");
		printf("the sum of %d + %d is: %d\n\r", a, b, a+b);
	}
	
	return 0;
}
