led.bin: start.o clock.o sdram.o mmu.o mmu2.o led.o
	arm-linux-ld -T mmu.lds -o led.elf $^
	arm-linux-objcopy -O binary led.elf led.bin
	arm-linux-objdump -D led.elf > led.dis

%.o : %.S
	arm-linux-gcc -o $@ $< -c

%.o : %.c
	arm-linux-gcc -o $@ $< -c
	
clean:
	rm *.o led.elf led.bin led.dis -f
