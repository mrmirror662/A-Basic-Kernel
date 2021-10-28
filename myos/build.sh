cd boot;
nasm -f elf64 multiboot.s &&
nasm -f elf64 boot.s &&
cd .. &&
ld -n- o kernel.bin -T boot/linker.ld boot/multiboot.o boot/boot.o Kernel.o &&
cp boot/kernel.bin isofiles/boot &&
grub-mkrescue -o os.iso isofiles 



