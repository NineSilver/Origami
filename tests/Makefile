CC = clang
AS = nasm

SRCS := kernel.c
OBJS := $(SRCS:.c=.o)
KERNEL = test.elf

CFLAGS = -Wall -Wextra -g -O2
KERNELCFLAGS := $(CFLAGS) \
			-target x86_64-unknown-none \
			-nostdlib \
			-ffreestanding \
			-fno-stack-protector \
			-fno-pic \
			-mcmodel=kernel \
			-mno-red-zone \
			-mno-sse \
			-mno-sse2

KERNELLDFLAGS := -target x86_64-unknown-none \
			-O2 \
			-nostdlib \
			-static \
			-fno-pic \
			-z max-page-size=0x1000 \
			-T linker.ld \

.PHONY: all
all: $(KERNEL)

$(KERNEL): $(OBJS)
	${CC} $(KERNELLDFLAGS) -o $@ $^

%.o: %.c
	${CC} $(KERNELCFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -rf $(OBJS) $(KERNEL)