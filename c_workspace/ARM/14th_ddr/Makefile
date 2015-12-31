led.bin : start.o clock.o sdram.o led.o
	arm-linux-ld  -T led.lds -o led.elf $^
	arm-linux-objcopy -O binary led.elf led.bin
	arm-linux-objdump -D led.elf > led.dis

%.o : %.S
	arm-linux-gcc -g -c -O2 -o $@ $^

%.o : %.c
	arm-linux-gcc -g -c -O2 -o $@ $^

clean:
	rm -f *.o *.bin *.elf *.dis	
