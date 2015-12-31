
.globl _start
.globl delay
_start:

/* 硬件相关的设置 */
    /* Peri port setup */
    ldr r0, =0x70000000
    orr r0, r0, #0x13
    mcr p15,0,r0,c15,c2,4       @ 256M(0x70000000-0x7fffffff)
    
/* 关看门狗 */
/* 往WTCON(0x7E004000)写0 */
	
	ldr r0, =0x7E004000
	mov r1, #0
	str r1, [r0]

	/* 设置栈 */
	ldr sp, =8*1024
	mov r0, #9
	bl xxxxx
halt:
	b halt	

delay:
delay_loop:		
	cmp r0, #0
	sub r0, r0, #1
	bne delay_loop
	mov pc, lr
	