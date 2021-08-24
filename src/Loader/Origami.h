#pragma once

// Implementation of spec/origami.h which follows UEFI naming convention

#include <ProcessorBind.h>

#pragma pack(1)

struct ORIGAMI_HEADER
{
    UINT64 EntryPoint;
    UINT64 Stack;

    UINT8 Flags;
};

enum E820_TYPE
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

struct E820_ENTRY
{
    UINT64 address;
    UINT64 size;
    UINT32 type;
};

struct ORIGAMI_FRAMEBUFFER
{
    UINT64 FramebufferAddress;
    UINT16 FramebufferWidth;
    UINT16 FramebufferHeight;
    UINT16 FramebufferPitch;
    UINT16 FramebufferBpp;
};

struct ORIGAMI_STRUCT
{
    CHAR8 BootloaderID[64];
    UINT8 OptInfo;

    UINT64 Cmdline;

    struct E820_ENTRY* Memmap;
    UINT64 MemmapEntries;
    
    UINT64 Framebuffer;
    UINT64 Rsdp;
    UINT64 Epoch;
};

EFI_STATUS LoadOrigamiKernel(BOOT_ENTRY* Entry);

#pragma pack()
