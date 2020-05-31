all: start.o sdram.o init.o uart.o led.o main.o exception.o interrupt.o print.o timer.o key_eint.o
	arm-linux-ld -T sdram.lds $^ -o sdram.elf
	arm-linux-objcopy -O binary -S sdram.elf sdram.bin
	arm-linux-objdump -D sdram.elf > sdram.dis	
	rm *.o *.elf
cleanall:
	rm *.bin *.o *.elf *.dis
cleanbin:
	rm *.bin *.dis

%.o: %.c
	arm-linux-gcc -c -o $@ $< -I include 

%.o: %.S
	arm-linux-gcc -c -o $@ $<
