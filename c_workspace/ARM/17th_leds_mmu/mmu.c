

#define ENABLE_DCACHE_ICACHE  0
#define REGISTER_USE_CACHE    0

#define MMU_BASE  0x51000000


#define MMU_FULL_ACCESS       (3 << 10)  /* ����Ȩ�� */
#define MMU_DOMAIN            (0 << 5)   /* �����ĸ��� */
#define MMU_SPECIAL           (1 << 4)   /* bit 4������1 */
#define MMU_CACHEABLE         (1 << 3)   /* ����ʹ��cache */
#define MMU_BUFFERABLE        (1 << 2)   /* ����ʹ��write buffer */
#define MMU_SECDESC           (2)        /* ��ʾ���Ƕ������� */

#define MMU_SECDESC_WB        (MMU_FULL_ACCESS | MMU_DOMAIN | \
	                           MMU_SPECIAL | MMU_CACHEABLE | \
	                           MMU_BUFFERABLE | MMU_SECDESC)

#define MMU_SECDESC_NCNB       (MMU_FULL_ACCESS | MMU_DOMAIN | \
	                           MMU_SPECIAL | MMU_SECDESC)


void mmu_init(void)
{
	volatile unsigned long *table = (volatile unsigned long *)MMU_BASE;
	/* ����ҳ���� */
#if REGISTER_USE_CACHE	
	table[256]   = 0x7f000000 | MMU_SECDESC_WB;  /* va: 0x10000000 => pa: 0x7f000000 */
	table[0x200] = 0x7f000000 | MMU_SECDESC_WB;  /* va: 0x20000000 => pa: 0x7f000000 */
#else
	table[256]   = 0x7f000000 | MMU_SECDESC_NCNB;  /* va: 0x10000000 => pa: 0x7f000000 */
	table[0x200] = 0x7f000000 | MMU_SECDESC_NCNB;  /* va: 0x20000000 => pa: 0x7f000000 */
#endif
	/* va: 0 => pa: 0 */
	table[0]     = 0 | MMU_SECDESC_WB;   /* MMUʹ��ǰ��Ĵ������ڵĿռ�, �������ǵ������ַ���������ַ */
	
	table[0xc00] = 0x57000000 | MMU_SECDESC_WB;  /* va: 0xc0000000 => pa: 0x57000000 */

    /*
To enable the MMU:
1) Program the translation table base and domain access control registers.
2) Program level 1 and level 2 page tables as required.
3) Enable the MMU by setting bit 0 in the control register.
     */
	/* ��ҳ����Ļ���ַ����MMU */
	/* ����MMU */
	__asm__ (

		"mov    r1, #0\n"
		"mcr    p15, 0, r1, c7, c7, 0\n"    /* ʹ��ЧICaches��DCaches */

		"mcr    p15, 0, r1, c7, c10, 4\n"   /* drain write buffer on v4 */
		"mcr    p15, 0, r1, c8, c7, 0\n"    /* ʹ��Чָ�����TLB */


		"mcr p15, 0, %0, c2, c0, 0\n" /* write TTB register */
		"mrc p15, 0, r1, c3, c0, 0\n" /* read domain 15:0 access permissions */
		"orr r1, r1, #3\n"            /* domain 0, Accesses are not checked */
		"mcr p15, 0, r1, c3, c0, 0\n" /* write domain 15:0 access permissions */

		"mrc p15, 0, r1, c1, c0, 0\n" /* Read control register */

#if ENABLE_DCACHE_ICACHE
		"orr r1, r1, #(1<<2)\n"       /* Data cache enable */
		"orr r1, r1, #(1<<12)\n"      /* Instruction cache enable */
		"orr r1, r1, #(1<<14)\n"      /* Round robin replacement */
#endif		
		"orr r1, r1, #(1<<0)\n"       /* MMU enable */
		
		"mcr p15,0,r1,c1, c0,0\n"     /* write control register */
		:
		: "r" (table)
		: "r1"
	);
}

