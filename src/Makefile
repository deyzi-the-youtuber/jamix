# stuff
objout = Image
iso_objout = cdrom.iso
iso_dir = ../out
c_flags = -I include -ffreestanding -mcmodel=large -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -g
asm_flags = -f elf64 -g
ld_flags = -T link.lds

# compilers / assemblers
export cc = x86_64-elf-gcc
export as = nasm
export ld = x86_64-elf-ld

# source files
c_src := $(shell find . -type f -name "*.c")
asm_src := $(shell find . -type f -name "*.S")
# object files
c_obj := $(patsubst %.c, %.o, $(c_src))
asm_obj := $(patsubst %.S, %.o, $(asm_src))

all: clean $(objout)

$(objout): $(asm_obj) $(c_obj)
	@echo "LD   $(objout)"
	@$(ld) $(ld_flags) $^ -n -o $(objout)
#	@strip --strip-unneeded $(objout)
	@echo "$(objout) is ready"

%.o: %.S
	@echo "AS   $<"
	@$(as) $(asm_flags) $< -o $@

%.o: %.c
	@echo "CC   $<"
	@$(cc) -c $< -o $@ $(c_flags)

clean:
	@rm -f $(c_obj) $(asm_obj) $(iso_objout) $(objout)

iso:
	@cp $(objout) $(iso_dir)/boot/
	@grub-mkrescue $(iso_dir) -o $(iso_objout)
#	@qemu-img convert -f raw -O vpc $(iso_objout) disk.vhd

qemu:
	@qemu-system-x86_64 \
	-cdrom $(iso_objout) \
	-m 2G \
	-no-reboot \
	-serial stdio
