#include <stdint.h>

#include "../spec/origami.h"

static uint8_t stack[4096];

__attribute__((section(".origamihdr"), used))
struct origami_header header = {
    .entry_point = 0,
    .stack = (uintptr_t)&stack + sizeof(stack),
    .flags = 0b01001100
};

void kmain(struct origami_struct* boot_info)
{
    for(;;);
}
