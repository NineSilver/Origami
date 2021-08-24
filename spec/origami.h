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

// Origami uses e820 memory map and type; no custom structs
enum e820_type
{
	E820_TYPE_RAM		= 1,
	E820_TYPE_RESERVED	= 2,
	E820_TYPE_ACPI		= 3,
	E820_TYPE_NVS		= 4,
	E820_TYPE_UNUSABLE	= 5,
	E820_TYPE_PMEM		= 7,

	E820_TYPE_PRAM		= 12,
	E820_TYPE_SOFT_RESERVED	= 0xefffffff,
	E820_TYPE_RESERVED_KERN	= 128,
};

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
    uint8_t opt_info;           /*  Bit 7: firmware (0 for Legacy/BIOS, 1 for UEFI)
                                 *
                                 *  Following bits are meant only for debugging the behavior of the hardware
                                 *  ========================================================================
                                 *  Bit 6: framebuffer status (1 found, 0 not found)
                                 *  Bit 5: reserved
                                 *  Bit 4: 0 if the system doesn't support ACPI (uncommon, as UEFI requires ACPI to work;
                                 *          mantained for compatibility with *other* bootloaders), 1 otherwise
                                 *  Bits 3-0 unused
                                 */
    
    uint64_t cmdline;            // Info contained in the bootloader config file

    struct e820_entry* memmap;   // Memory map address
    uint64_t mmap_entries;       // Number of memory map entries
    
    uint64_t framebuffer;        // Framebuffer pointer (0 if not requested)
    uint64_t rsdp;               // RSDP pointer (0 if not requested)
    uint64_t epoch;              // UNIX Epoch (0 if not requested)
} __attribute__((packed));

#endif /* !__ORIGAMI_H */
