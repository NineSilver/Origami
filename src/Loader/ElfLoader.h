#pragma once

#include <Uefi.h>
#include <Protocol/SimpleFileSystem.h>

typedef struct _ELF_INFO {
    // the entry point of the elf
    UINT64 EntryPoint;

    // the top of the loaded file
    EFI_PHYSICAL_ADDRESS Top;
} ELF_INFO;

EFI_STATUS LoadSection(EFI_FILE_PROTOCOL* File, VOID** Buffer, UINTN* Size, const CHAR8* Name);
EFI_STATUS LoadElf(EFI_FILE_PROTOCOL* File, ELF_INFO* Info, EFI_MEMORY_TYPE Type);
