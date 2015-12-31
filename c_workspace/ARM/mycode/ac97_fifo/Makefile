
CC      = arm-linux-gcc
LD      = arm-linux-ld
AR      = arm-linux-ar
OBJCOPY = arm-linux-objcopy
OBJDUMP = arm-linux-objdump

INCLUDEDIR 	:= $(shell pwd)/include
CFLAGS 		:= -Wall -O2
CPPFLAGS   	:= -nostdinc -I$(INCLUDEDIR)

export 	CC LD OBJCOPY OBJDUMP INCLUDEDIR CFLAGS CPPFLAGS

objs := start.o clock.o sdram.o uart.o common.o  irq.o  main.o  print.o  lib/libc.a

ac97.bin: $(objs)
	${LD} -Tac97.lds -o ac97.elf $^
	${OBJCOPY} -O binary -S ac97.elf $@
	${OBJDUMP} -D -m arm ac97.elf > ac97.dis

.PHONY : lib/libc.a
lib/libc.a:
	cd lib; make; cd ..
	
%.o:%.c
	${CC} $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

%.o:%.S
	${CC} $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

clean:
	make  clean -C lib
	rm -f ac97.bin ac97.elf ac97.dis *.o
	
