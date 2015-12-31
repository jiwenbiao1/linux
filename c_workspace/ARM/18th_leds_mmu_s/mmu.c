

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

extern void mmu_enable(unsigned long table);

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

	mmu_enable(0x51000000);
}

