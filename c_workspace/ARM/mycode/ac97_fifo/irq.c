
#include <stdio.h>
#include <common.h>

typedef int ( * PFUNC )( void );

#define VIC0ADDRESS	0x71200f00
#define VIC1ADDRESS	0x71300f00
#define VIC0INTENABLE	0x71200010	
#define VIC1INTENABLE	0x71300010

int do_irq( void )
{
	PFUNC func;

	if( ( func = ( PFUNC )(read_val( VIC0ADDRESS ))) != 0x0 ) {
	} else if( (func=(PFUNC)(read_val( VIC1ADDRESS ))) != 0X0 ) {
	} else {
		return -1;			
	}

	set_val( VIC1ADDRESS, 0x0 );
	set_val( VIC0ADDRESS, 0x0 );

	func();
}

int install_int_handler( int num, PFUNC p )
{
	u32 * pbase;
	if( num < 32) {
		pbase = ( u32 * )( 0x71200000 + 0x100 );
		set_one( VIC0INTENABLE, num );
	} else {
		pbase = ( u32 * )( 0x71300000 + 0x100 );
		num -= 32;
		set_one( VIC1INTENABLE, num );
	}

	pbase[num] = ( u32 )p;

	return 0;
}
