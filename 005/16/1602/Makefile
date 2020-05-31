all: start.o sdram.o init.o interrupt.o nand_flash.o nor_flash.o led.o uart.o main.o exception.o timer.o my_printf.o string_utils.o lib1funcs.o  print.o key_eint.o
	arm-linux-ld -T sdram.lds $^ -o sdram.elf
	arm-linux-objcopy -O binary -S sdram.elf sdram.bin
	arm-linux-objdump -D sdram.elf > sdram.dis	
	rm *.o *.elf
cleanall:
	rm *.bin *.o *.elf *.dis
cleanbin:
	rm *.bin *.dis

%.o: %.c
	arm-linux-gcc -march=armv4 -c -o $@ $< -I include 

%.o: %.S
	arm-linux-gcc -march=armv4 -c -o $@ $<
