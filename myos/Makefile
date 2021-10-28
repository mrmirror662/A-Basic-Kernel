CXX = x86_64-elf-g++
CC = x86_64-elf-gcc

KERNEL    := kernel.elf
ASM = nasm
ASMFLAGS = -f elf64
CXXFLAGS =  -ffreestanding -Wall -Wextra 
ASMDIR = boot
SOURCEDIR = Kernel

asmsrc := $(shell find $(ASMDIR) -name '*.asm')
cxxsrc := $(shell find $(SOURCEDIR) -name '*.cpp')

OBJDIR = obj

asmobj    := $(patsubst %, $(OBJDIR)/%.o, $(asmsrc))
cxxobj    := $(patsubst %, $(OBJDIR)/%.o, $(cxxsrc))
obj       := $(asmobj) $(cxxobj)
.PHONY: all
all: $(KERNEL)

$(KERNEL):	$(obj)
	ld  -n -T boot/linker.ld $(obj) -o $@
	cp kernel.elf isofiles/boot
	grub-mkrescue -o os.iso isofiles

$(OBJDIR)/%.asm.o: %.asm
	mkdir -p $(dir $@)
	$(ASM) $(ASMFLAGS) -o $@ $<

$(OBJDIR)/%.cpp.o: %.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -o $@ -c $<

	
clean:
	rm -rf $(OBJDIR) $(KERNEL)
