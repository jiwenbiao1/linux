

#define EXE_LD_ADDR	0x57000000

#define IRQ_STACK_START	(EXE_LD_ADDR)
#define IRQ_STACK_LEN	0x10000

#define SWI_STACK_START	(IRQ_STACK_START-IRQ_STACK_LEN)
#define SWI_STACK_LEN	0x10000

#define PABT_STACK_START (SWI_STACK_START-SWI_STACK_LEN)
#define PABT_STACK_LEN	0x10000

#define DABT_STACK_START (PABT_STACK_START-PABT_STACK_LEN)
#define DABT_STACK_LEN	0x10000

#define SP_START (DABT_STACK_START-DABT_STACK_LEN)


