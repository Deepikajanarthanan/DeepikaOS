all: deepikaos.bin

boot.o: boot/boot.asm
	nasm -f elf32 boot/boot.asm -o boot.o

interrupts.o: boot/interrupts.asm
	nasm -f elf32 boot/interrupts.asm -o interrupts.o

kernel.o: kernel/kernel.c
	gcc -m32 -ffreestanding -c kernel/kernel.c -o kernel.o

deepikaos.bin: boot.o interrupts.o kernel.o linker.ld
	ld -m elf_i386 -T linker.ld -o deepikaos.bin boot.o interrupts.o kernel.o

clean:
	rm -f boot.o interrupts.o kernel.o deepikaos.bin