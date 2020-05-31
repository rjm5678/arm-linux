all:
	arm-linux-gcc -c -o led.o led.c
	arm-linux-gcc -c -o uart.o uart.c
	arm-linux-gcc -c -o init.o init.c
	arm-linux-gcc -c -o sdram.o sdram.c
	arm-linux-gcc -c -o main.o main.c
	arm-linux-gcc -c -o start.o start.S
	arm-linux-ld -T sdram.lds start.o led.o uart.o init.o sdram.o main.o -o sdram.elf
	arm-linux-objcopy -O binary -S sdram.elf sdram.bin
	arm-linux-objdump -D sdram.elf > sdram.dis	
clean:
	rm *.bin *.o *.elf *.dis

