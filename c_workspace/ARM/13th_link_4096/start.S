
.globl _start
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

/* 重定位 */
	adr r0, _start  /* 伪指令,取_start的当前地址 */
	ldr r1, =_start /* 链接地址 */
	ldr r2, =bss_start
	cmp r0, r1
	beq clean_bss
copy_loop:
	ldr r3, [r0], #4
	str r3, [r1], #4
	cmp r1, r2
	bne copy_loop
		
/* 清bss段 */
clean_bss:
	ldr r0, =bss_start
	ldr r1, =bss_end
	mov r2, #0
clean_loop:
	str r2, [r0], #4
	cmp r0, r1
	bne clean_loop	

	/* 设置栈 */
	ldr sp, =8*1024
	ldr pc, =main
	bl main
halt:
	b halt	

