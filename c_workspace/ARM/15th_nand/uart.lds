SECTIONS {
    . = 0x51000000;
    
	.text : {
			start.o
			* (.text)
	}

	.rodata : {
			* (.rodata)
	}

	.data : {
			* (.data)
	}

    __bss_start = .;
    .bss ALIGN(4)  : { *(.bss)  *(COMMON) }
    __bss_end = .;
}
