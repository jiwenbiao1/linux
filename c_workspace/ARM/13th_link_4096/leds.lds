SECTIONS
{
	. = 0x1000;
	.text : {
		start.o
		* (.text)
	}
    		
	.data : {
		* (.data)
	}
	
	bss_start = .;  /* 0x1150 */
	.bss : {
		* (.bss)
	}
	bss_end  = .;	
}
