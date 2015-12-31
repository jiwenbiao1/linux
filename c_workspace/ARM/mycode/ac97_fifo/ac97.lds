ENTRY(_start);
OUTPUT_FORMAT( "elf32-littlearm", "elf32-bigarm", "elf32-littlearm" );
OUTPUT_ARCH(arm);

SECTIONS 
{
	. = 0x000000;	
	.text : { *(.text) }
	. = ALIGN(4);
	.data  : { *(.data) }
	. = ALIGN(4);
	.rodata  : { *(.rodata) }
	. = ALIGN(4);
	__bss_start = .;
	.bss  : { *(.bss) }
	
	. = ALIGN(4);
	__bss_end = .;
}



