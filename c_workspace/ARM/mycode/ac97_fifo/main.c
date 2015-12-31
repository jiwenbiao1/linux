#include <stdio.h>
#include <common.h>

#define AC_GLBCTRL    0x7F001000
#define AC_GLBSTAT    0x7F001004
#define AC_CODEC_CMD  0x7F001008
#define AC_CODEC_STAT 0x7F00100C
#define AC_PCMADDR    0x7F001010
#define AC_PCMDATA    0x7F001018

#define GPDCON        0x7F008060
#define GPECON        0x7F008080
#define VIC1INTENABLE 0x71300010
#define VIC1SOFTINT   0x71300018
#define VIC1IRQSTATUS 0x71300000

#define MUSIC_ADDR     0x54000000 // the start address of wav data

volatile unsigned int read_ready = 0;
unsigned int times = 0;

void ac97_init(void);
int do_ac97_int( void );
int enable_irq( void );
void write_cmd(unsigned char, unsigned short);
unsigned short read_cmd(unsigned char);
void wm9714_init(void);
void udelay(int num);


#define INT_AC97	36

int main( void )
{
	printf("start.\n");	

	// config pin
	set_val( GPDCON, 0x44444 );

	// install int ac97 intterupt handler
	install_int_handler( INT_AC97, do_ac97_int );
	enable_irq();

	ac97_init();	
	wm9714_init();	
	
	// enable fifo empty interupt, at first, the fifo is empyt, so generate the interrupt, and then intr driver
	set_one( AC_GLBCTRL, 21 );

	printf("end.\n");
	return 0;
}

int do_ac97_int( void )
{
	// codec ready interrupt
	if ( get_bit( AC_GLBSTAT, 22) )
	{
		read_ready = 1;
		// codec ready interrupt clear
		set_one( AC_GLBCTRL, 30 );
	}

	// pcm out channel underrun interupt
	if ( get_bit( AC_GLBSTAT, 21 ) )
	{
		// clear pcm out chancel underrun interrupt
		set_one( AC_GLBCTRL, 29 );
		// write 4 bytes wav data into PCMDATA reg
		set_val( AC_PCMDATA, *(volatile unsigned int *)(MUSIC_ADDR + (4 * times++)) );
		if (times > 300000 )
		{
			printf("restart ..\n");
			times = 0;
		} 
	}
}

void ac97_init(void)
{
	//cold reset ......
	set_one( AC_GLBCTRL, 0 );
	udelay(1000);
	set_zero( AC_GLBCTRL, 0 );
	udelay(1000);

	//warm reset ......
	set_2bit( AC_GLBCTRL, 1, 0x3 );
	udelay(1000);
	set_zero( AC_GLBCTRL, 1 );
	udelay(1000);

	set_one( AC_GLBCTRL, 12 );	// set pio mode, not dma
	udelay(1000);
	set_one( AC_GLBCTRL, 3 );	// transfer data enable using AC-link

	set_one( AC_CODEC_CMD, 23 );	// read ac97 status
	udelay(1000);

	if ( ( read_val( AC_GLBSTAT ) & 3 ) == 3 )
		printf("ac97 has been active ...\n");
	else
		printf("ac97 init failed ...\n");
}

void wm9714_init(void)
{

	// set volume, sample rate, mux, enable power output

	// left right DAC VOL control
	write_cmd(0x0c, 0);
	write_cmd(0x5c, 0);

	// enable variable rate audio control
	write_cmd(0x2a, 1);
	// set sample rate to 44100Hz
	write_cmd(0x2c, 0xac44);
	
	//  output mux select and speaker source 
	write_cmd(0x1c,(1 << 12) | (1 << 9) | (1 << 7) | (1 << 5));
	// set headphone volume
	write_cmd(0x04, (0xf << 8) | (0xf));
	// set speaker volume
	write_cmd(0x02, (0x9 << 8) | (0x9));
	write_cmd(0x6,0);
		
	// enable power
	write_cmd(0x26, 0);
	write_cmd(0x3c, 0);
	write_cmd(0x3e, 3 << 9);

	// enable jake insert
	write_cmd(0x24, (1 << 4)); 
}

void write_cmd(unsigned char addr, unsigned short data)
{
	set_val( AC_CODEC_CMD , (addr << 16) | data );
	udelay(1000);	
}

unsigned short read_cmd(unsigned char addr)
{
	set_val( AC_CODEC_CMD , ((1 << 23) | (addr << 16) | 0) );
	udelay(1000);

	set_one( AC_GLBCTRL, 22 );
	while (!read_ready)	
		;
	read_ready = 0;	

	return read_val( AC_CODEC_STAT ) & 0xffff;
}

void udelay(int num)
{
	__asm__ __volatile__ (
	"mov	r0, %[num] \n\t"
	"loop:	\n\t"
	"sub	r0, r0, #1\n\t"
	"cmp	r0, #0 \n\t"
	"bne	loop \n\t"
	:
	:[num] "r"(66 * num)
	:"r0"
	);
}

int enable_irq( void )
{
       __asm__ __volatile__ (
        "mrs    r0, cpsr \n"
        "bic    r0, r0, #0x80 \n"
        "msr    cpsr, r0 \n"
        :::"r0"
        );
}
