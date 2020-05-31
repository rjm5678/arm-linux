all:
	arm-linux-gcc -c -o led.o led.c
	arm-linux-gcc -c -o uart.o uart.c
	arm-linux-gcc -c -o init.o init.c
	arm-linux-gcc -c -o main.o main.c
	arm-linux-gcc -c -o start.o start.S
	arm-linux-ld -Ttext 0 start.o led.o uart.o init.o main.o -o tacc_set.elf
	arm-linux-objcopy -O binary -S tacc_set.elf tacc_set.bin
	arm-linux-objdump -D tacc_set.elf > tacc_set.dis	
clean:
	rm *.bin *.o *.elf *.dis

