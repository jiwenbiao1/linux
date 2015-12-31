
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
	
	/* 设置栈 */
	ldr sp, =8*1024

	/* 设置时钟 */
	bl clock_init
	bl ddr_init
	bl init_uart
	
	/* 清BSS */
/* 把BSS段对应的内存清零 */
clean_bss:
	ldr r0, =bss_start
	ldr r1, =bss_end
	mov r3, #0
	cmp r0, r1
	b copy_kernel
clean_loop:
	str r3, [r0], #4
	cmp r0, r1	
	bne clean_loop		

copy_kernel:
	mov r0, #0x100000
	ldr r1, =(0x50008000 - 64)
	mov r2, #0x200000
	bl copykernel2ddr	
	cmp r0, #0
	bne halt

	bl set_params

	mov r0, #0
	ldr r1, =1626
	ldr r2, =0x50000100
	ldr pc, =0x50008000
		
halt:
	b halt	


