#include <stddef.h>
#include <stdint.h>

#include "../spec/origami.h"

// Define a stack for kernel use
static uint8_t stack[4096];

// Set up the header
__attribute__((section(".origamihdr"), used))
struct origami_header header = {
    .entry_point = 0,                           // No custom entry point
    .stack = (uintptr_t)&stack + sizeof(stack), // Use the stack above
    .flags = 0b01001100                         // Framebuffer, ACPI and Epoch
};

// Entry point
void kmain(struct origami_struct* boot_info)
{
    // Output an 'a' to QEMU debugcon
    asm volatile(
        ".intel_syntax noprefix\n"
        "out dx, al\n\t"
        :
        : "d"(0xe9), "a"('a'));

    asm volatile(
        ".intel_syntax noprefix\n"
        "out dx, al\n\t"
        :
        : "d"(0xe9), "a"('\n'));

    // Print a white pixel at the top of the screen
    struct origami_framebuffer* fb = (struct origami_framebuffer*)boot_info->framebuffer;
    uint32_t* framebuffer = (uint32_t*)fb->address;
    size_t index = 10 + (fb->pitch / sizeof(uint32_t)) * 10;

    framebuffer[index] = 0xffffff;

    // Hang forever
    for(;;);
}
