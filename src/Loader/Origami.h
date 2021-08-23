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
    CHAR8 BootloaderBrand[64];
    UINT8 OptInfo;

    UINT64 Cmdline;

    UINT64 MemmapAddress;
    UINT64 MemmapEntries;
    
    UINT64 Framebuffer;
    UINT64 Rsdp;
    UINT64 Epoch;
};

EFI_STATUS LoadOrigamiKernel(BOOT_ENTRY* Entry);

#pragma pack()
