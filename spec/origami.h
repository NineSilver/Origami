#ifndef __ORIGAMI_H
#define __ORIGAMI_H

#include <stdint.h>

// Header that the kernel must contain and fill in order
// to be loaded with the required features enabled
struct origami_header
{
    uint64_t entry_point;       //  User-defined entry point (not recommended, specify it in the linker.ld file)
    uint64_t stack;             //  User-defined stack (avoids the creation of stub headers in Assembly)

    uint8_t flags;              /*  Bit 7: ALWAYS set to 0
                                 *  Bit 6: framebuffer petition
                                 *  Bit 5: reserved
                                 *  Bit 4: reserved
                                 *  Bit 3: ACPI RSDP table request
                                 *  Bit 2: UNIX Epoch
                                 *  All other bits ignored
                                 */
} __attribute__((packed));

struct e820_entry
{
    uint64_t address;
    uint64_t size;
    uint32_t type;
} __attribute__((packed));

// Self explanatory
struct origami_framebuffer
{
    uint64_t address;
    uint16_t width;
    uint16_t height;
    uint16_t pitch;
    uint16_t bpp;
} __attribute__((packed));

// Information passed to the kernel from the bootloader
struct origami_struct
{
    char bootloader_id[64];     // Must follow the format "<bootloader-name> - <version>"
    uint8_t opt_info;           /*  Bit 0: firmware (0 for Legacy/BIOS, 1 for UEFI)
                                 *
                                 *  Following bits are meant only for debugging the behavior of the hardware
                                 *  ========================================================================
                                 *  Bit 1: framebuffer status (1 found, 0 not found)
                                 *  Bit 2: whether 5-level paging is enabled or not
                                 *  Bit 3: 0 if the system doesn't support ACPI (uncommon, as UEFI requires ACPI to work;
                                 *          mantained for compatibility with *other* bootloaders), 1 otherwise
                                 *  Bits 4-7 unused
                                 */
    
    uint64_t cmdline;            // Info contained in the bootloader config file

    uint64_t mmap_address;       // Memory map address
    uint64_t mmap_entries;       // Number of memory map entries
    
    uint64_t framebuffer;        // Framebuffer pointer (0 if not requested)
    uint64_t rsdp;               // RSDP pointer (0 if not requested)
    uint64_t epoch;              // UNIX Epoch (0 if not requested)
} __attribute__((packed));

#endif /* !__ORIGAMI_H */
