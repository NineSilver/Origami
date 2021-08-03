#
# Define come directories for a better build system
#

BINDIR := ./bin
BUILDDIR := ./build

#
# Compiler and linker to be used
#

CC = clang
FUSE_LD = lld-link

#
# Compilation flags
#

CFLAGS := -target x86_64-unknown-windows -ffreestanding
CFLAGS += -fshort-wchar -Wall -Werror -Os -flto -g
CFLAGS += -nostdlib -nostdinc -std=c11

CFLAGS += -Ilibedk2/Include
CFLAGS += -Ilibedk2/Include/X64

CFLAGS += -DgEfiCallerBaseName=\"OrigamiBoot\"

EDK2_OPTS_BOOL := PcdVerifyNodeInList=FALSE
EDK2_OPTS_BOOL += PcdComponentNameDisable=FALSE
EDK2_OPTS_BOOL += PcdDriverDiagnostics2Disable=FALSE
EDK2_OPTS_BOOL += PcdComponentName2Disable=FALSE
EDK2_OPTS_BOOL += PcdDriverDiagnosticsDisable=FALSE
EDK2_OPTS_BOOL += PcdUgaConsumeSupport=TRUE

EDK2_OPTS_UINT32 := PcdMaximumLinkedListLength=1000000
EDK2_OPTS_UINT32 += PcdMaximumUnicodeStringLength=1000000
EDK2_OPTS_UINT32 += PcdMaximumAsciiStringLength=1000000
EDK2_OPTS_UINT32 += PcdSpinLockTimeout=10000000
EDK2_OPTS_UINT32 += PcdFixedDebugPrintErrorLevel=0xFFFFFFFF
EDK2_OPTS_UINT32 += PcdDebugPrintErrorLevel=0xFFFFFFFF
EDK2_OPTS_UINT32 += PcdUefiLibMaxPrintBufferSize=320
EDK2_OPTS_UINT32 += PcdMaximumDevicePathNodeCount=0
EDK2_OPTS_UINT32 += PcdCpuLocalApicBaseAddress=0xfee00000
EDK2_OPTS_UINT32 += PcdCpuInitIpiDelayInMicroSeconds=10000

EDK2_OPTS_UINT16 := PcdUefiFileHandleLibPrintBufferSize=1536

EDK2_OPTS_UINT8 := PcdSpeculationBarrierType=0x1
EDK2_OPTS_UINT8 += PcdDebugPropertyMask=DEBUG_PROPERTY_ASSERT_BREAKPOINT_ENABLED
EDK2_OPTS_UINT8 += PcdDebugClearMemoryValue=0xAF

CFLAGS += $(EDK2_OPTS_UINT32:%=-D_PCD_GET_MODE_32_%)
CFLAGS += $(EDK2_OPTS_UINT16:%=-D_PCD_GET_MODE_16_%)
CFLAGS += $(EDK2_OPTS_UINT8:%=-D_PCD_GET_MODE_8_%)
CFLAGS += $(EDK2_OPTS_BOOL:%=-D_PCD_GET_MODE_BOOL_%)

LDFLAGS := -target x86_64-unknown-windows
LDFLAGS += -nostdlib
LDFLAGS += -Wl,-entry:EfiMain
LDFLAGS += -Wl,-subsystem:efi_application
LDFLAGS += -fuse-ld=$(FUSE_LD)

ASFLAGS := -g -f win64

#
# Sources and object files
#

SRCS := $(shell find src/ -name '*.c')
SRCS += $(shell find src/ -name '*.asm')

SRCS += libedk2/Library/guids.c

SRCS += $(shell find libedk2/Library/ -name '*.c')
SRCS += $(shell find libedk2/Library/ -name '*.asm')

OBJS := $(SRCS:%=$(BUILDDIR)/%.obj)

#
# Actual targets
#

.PHONY: default
default: all

.PHONY: all
all: $(BINDIR)/BOOTX64.EFI

$(BINDIR)/BOOTX64.EFI: $(OBJS)
	@echo " [LD] $@"
	@mkdir -p $(@D)
	@${CC} $(LDFLAGS) $^ -o $@

$(BUILDDIR)/%.c.obj: %.c
	@echo " [CC] $@"
	@mkdir -p $(@D)
	@${CC} $(CFLAGS) -c $< -o $@

$(BUILDDIR)/%.asm.obj: %.asm
	@echo " [AS] $@"
	@mkdir -p $(@D)
	@nasm $(ASFLAGS) $< -o $@

# Clean target: remove the build and bin directories
.PHONY: clean
clean:
	rm -rf $(BUILDDIR) $(BINDIR) image

# Flags for testing with qemu
QEMU_ARGS += -m 4G -smp 4
QEMU_ARGS += -machine q35
QEMU_ARGS += -debugcon stdio
QEMU_ARGS += -monitor telnet:localhost:4321,server,nowait
QEMU_ARGS += --no-shutdown
QEMU_ARGS += --no-reboot

# Workaround for WSL
ifeq ($(shell uname -r | sed -n 's/.*\( *Microsoft *\).*/\1/p'), Microsoft)
    QEMU := qemu-system-x86_64.exe
else
    QEMU := qemu-system-x86_64
endif

# Use an emulated drive as hard disk for qemu
image: $(BINDIR)/BOOTX64.EFI
	mkdir -p ./$@/EFI/BOOT/
	cp $< ./$@/EFI/BOOT/BOOTX64.EFI

# Test it!
.PHONY: test
test: image misc/OVMF.fd
	${QEMU} $(QEMU_ARGS) -bios misc/OVMF.fd -hda fat:rw:image

misc/OVMF.fd:
	rm -rf misc
	mkdir -p $(@D)
	wget -P $(@D) https://efi.akeo.ie/OVMF/OVMF-X64.zip
	cd $(@D) && unzip OVMF-X64.zip
	rm -f OVMF-X64.zip readme.txt
