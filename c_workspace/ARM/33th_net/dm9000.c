#include <stdio.h>
#include <common.h>
#include <s3c6410_regs.h>

#define IOADDR	0x18000000
#define IODATA	0x18000004

#define outb( val, addr )	*(u8*)(addr) = (val)
#define inb( addr )			(*(u8*)(addr))
#define outw( val, addr )	*(u16*)(addr) = (val)
#define inw( addr )			(*(u16*)(addr))

#define GPR	0x1f
#define NCR	0x00
#define IMR	0xff
#define NSR 	0x01
#define ISR	0xfe
#define RCR	0x05
#define MRCMDX	0xf0
#define MRCMD	0xf2
#define MWCMD	0xf8
#define MDRAH	0xfd
#define MDRAL	0xfc
#define TCR		0x02

void iow( u8 reg, u8 data )
{
	outb( reg, IOADDR ); 
	outb( data, IODATA );
}

u8 ior( u8 reg )
{
	outb( reg, IOADDR );
	return inb( IODATA );			
}

#define Tacs  0 //0xf
#define Tcos  0
#define Tacc  2
#define Tcoh  0
#define Tcah 0
#define Tacp 0

/* (Tacc + Tacp) >= 10ns
 */

int dm9000_init( void )
{

	/* 设置 SROM控制器 
	 * 设置BANK1(cs1): 位宽/是否使用等待信号/时间参数
	 */
	 SROM_BW |= (1<<4);  /* 16bit, no nWait */
	 SROM_BC1 = (Tacs << 28) | (Tcos << 24) | \
	 	               (Tacc << 16) | (Tcoh << 12) | \
	 	               (Tcah << 8)  | (Tacp << 4);
	 
	/* 设置DM9000 
	 * 例子:
	 * 怎么访问DM900内部地址为addr的寄存器,写入数据data?
	 * 分两部: 先把"addr"写到dm9000的"index port", dm9000的cmd为0,6410的addr2为0,用0x18000000
	 *         再把"data"写到dm9000的"data port", dm9000的cmd为1,6410的addr2为1,用0x18000004
	 */

	// power on the dm9000		
	iow( GPR, 0x00 );

	// software reset 
	iow( NCR, 0x01 );		
	mdelay(1);
	iow( NCR, 0x00 );

	iow( IMR, ( 1 << 7 ) );

	ior( NSR );
	ior( ISR );
	iow( IMR, ( 0x1 | ( 1 << 7 ) ) );		
	iow( RCR, ( 0x1 | ( 1 << 1 ) | ( 1 << 4 ) | ( 1 << 5 ) ) );
}

u32 dm_recv( void * buf )
{
	u8 status = ior( ISR );		
	if( status & 0x01 ) {
			iow( ISR, 0x01 );	
	} else {
			return -1;
	}

	status = ior( MRCMDX );
	status = inb( IODATA );

	if( ( status & 0x2 ) != 0 ) {
			return -2;		
	}

	u8 io_mode = ior( ISR );
	io_mode >>= 7;

	#define DM9000_BYTE_MODE	1
	#define DM9000_WORD_MODE	0

	u32 rx_length = 0;

	outb( MRCMD, IOADDR );	

	if( io_mode == DM9000_WORD_MODE ) {
			status = inw( IODATA );	
			rx_length = inw( IODATA );	
	} else {
			return -3;
	}

	u32 tmp_length = ( rx_length + 1 ) >> 1;		
	int i;
	for( i=0; i<tmp_length; i++ ) {
			(( u16*)buf)[i] = inw( IODATA );
	}	

	return rx_length;
}

int dm_send( void * buf, int len )
{
	outb( MWCMD, IOADDR );

	int tmp_length = ( len + 1 ) >> 1 ;
	int i;

	for( i=0; i<tmp_length; i++ ) 
		outw( ((u16*)buf)[i], IODATA );						printf("%s line %d\n\r", __FUNCTION__, __LINE__);

	printf("%s line %d\n\r", __FUNCTION__, __LINE__);

	iow( MDRAH, ( len >> 8 ) & 0xff ); 
	printf("%s line %d\n\r", __FUNCTION__, __LINE__);
	iow( MDRAL, len & 0xff );
	printf("%s line %d\n\r", __FUNCTION__, __LINE__);

	iow( TCR, 0x1 );	
	printf("%s line %d\n\r", __FUNCTION__, __LINE__);
	
	int status;
	
	while( 1 ) {
		status = ior( NSR );				
		printf("%s line %d, status = 0x%x\n\r", __FUNCTION__, __LINE__, status);
		if( status & ( 4 | 8 ) )
		{
			printf("%s line %d, status = 0x%x\n\r", __FUNCTION__, __LINE__, status);
			break;
		}
	}
	return 0;
} 


void dm9000_test(void)
{
	char buf[100] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66 };
	char c;

	printf("DM9000 test :\n\r");
	printf("press q to exit\n\r");

	dm9000_init();

	while( 1 ) {
		
		getc_nowait(&c);
		if (c == 'q' || c == 'Q')
		{
			printf("%s line %d\n\r", __FUNCTION__, __LINE__);
			return ;
		}

		printf("%s line %d\n\r", __FUNCTION__, __LINE__);
		dm_send( buf, sizeof(buf) );	
		printf("%s line %d\n\r", __FUNCTION__, __LINE__);
	}	
}

