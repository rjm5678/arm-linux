objs = start.o sdram.o nand_flash.o init.o interrupt.o  nor_flash.o led.o uart.o main.o exception.o timer.o my_printf.o string_utils.o lib1funcs.o  print.o key_eint.o

objs += ./adc_ts/touchscreen.o


all: $(objs)
	arm-linux-ld -T sdram.lds $^ -o sdram.elf
	arm-linux-objcopy -O binary -S sdram.elf sdram.bin
	arm-linux-objdump -D sdram.elf > sdram.dis	
	rm *.o *.elf ./adc_ts/touchscreen.o
cleanall:
	rm *.bin *.o *.elf *.dis ./adc_ts/touchscreen.o
cleanbin:
	rm *.bin *.dis 

%.o: %.c
	arm-linux-gcc -march=armv4 -c -o $@ $< -I include 

%.o: %.S
	arm-linux-gcc -march=armv4 -c -o $@ $<
