kasm: kernel.asm
	nasm -f elf32 kernel.asm -o kasm.o

kc: kernel.c
	gcc -I/usr/include/x86_64-linux-gnu -fno-stack-protector -m32 -c kernel.c -o kc.o

link: kasm.o kc.o link.ld
	ld -m elf_i386 -T link.ld -o kernel kasm.o kc.o

run: kernel
	qemu-system-i386 -kernel kernel
