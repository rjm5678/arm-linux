all:
	arm-linux-gcc -c -o nLED1.o nLED1.S
	arm-linux-ld -Ttext 0 nLED1.o -o nLED1.elf
	arm-linux-objcopy -O binary -S nLED1.elf nLED1.bin

clean:
	rm *.bin *.o *.elf
