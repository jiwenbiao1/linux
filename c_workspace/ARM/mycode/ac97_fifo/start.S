#include <config.h>
#define WTCON	0x7e004000

#define GPMCON	0x7f008820
#define GPMDAT	0x7f008824

	.text
	.global _start
_start:
	b reset
	ldr pc, _undefined 			
	ldr pc, _soft_int
	ldr pc, _prefetch
	ldr pc, _dabt
	ldr pc, _reserved
	ldr pc, _irq
	ldr pc, _fiq


_undefined:
	.word undefined	
_soft_int:
	.word soft_int	
_prefetch:
	.word prefetch	
_dabt:
	.word dabt
_reserved:
	.word reserved
_irq:
	.word irq
_fiq:
	.word fiq
	
undefined:
	b undefined	

soft_int:
	b soft_int

prefetch:
	b prefetch

dabt:
	b dabt

reserved:
	b reserved

irq:
	ldr sp, =IRQ_STACK_START
	sub lr, lr, #4
	stmfd sp!, {r0-r12, lr}

	bl do_irq

	ldmfd sp!, {r0-r12, pc}^

fiq:
	b fiq


reset:
    /* Peri port setup */
    ldr r0, =0x70000000
    orr r0, r0, #0x13
    mcr p15,0,r0,c15,c2,4       @ 256M(0x70000000-0x7fffffff)

       ldr r0, =0x7E004000  
        mov r1, #0
        str r1, [r0]

        bl clock_init

        ldr sp, =8*1024
        bl sdram_init

        bl init_uart

	mov r0, r0
	mov r0, r0

	ldr sp, =SP_START
	mov fp, #0
	
	mov r0, #0
	ldr r1, =EXE_LD_ADDR
	mov r2, #(8*1024)
	@bl memcpy

	bl clean_bss


	bl main

die_loop:
	b die_loop	

clean_bss:
        ldr r0, =__bss_start
        ldr r1, =__bss_end
        mov r3, #0
        cmp r0, r1
        moveq pc, lr
clean_loop:
        str r3, [r0], #4
        cmp r0, r1
        bne clean_loop
        mov pc, lr


_bss_start:
	.word __bss_start

_bss_end:
	.word __bss_end
