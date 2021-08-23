CC = clang
AS = nasm

SRCS := kernel.c
OBJS := $(SRCS:.c=.o)
KERNEL = test.elf

CFLAGS = -Wall -Wextra -g -O2
CHARDFLAGS := $(CFLAGS) \
			-target x86_64-unknown-none \
			-nostdlib \
			-ffreestanding \
			-fno-stack-protector \
			-fno-pic \
			-mcmodel=kernel \
			-mno-red-zone \
			-mno-sse \
			-mno-sse2

LDHARDFLAGS := -target x86_64-unknown-none \
			-O2 \
			-nostdlib \
			-static \
			-fno-pic \
			-z max-page-size=0x1000 \
			-T linker.ld \

.PHONY: all
all: $(KERNEL)

$(KERNEL): $(OBJS)
	${CC} $(LDHARDFLAGS) -o $@ $^

./build/%.o: %.c
	${CC} $(CHARDFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -rf $(OBJS) $(KERNEL)