SECTIONS
{
	. = 0x50000000;
	.text : {
		start.o
		* (.text)
	}
    		
	.data : {
		* (.data)
	}
	
	bss_start = .;
	.bss : {
		* (.bss)
	}
	bss_end  = .;	
}
