#pragma once

#include <Uefi.h>
#include <Protocol/SimpleFileSystem.h>

#define CHECK_OPTION(x) (StrnCmp(Line, x L"=", ARRAY_SIZE(x)) == 0)

typedef struct _ORIGAMI_BOOT_CONFIG
{
    CHAR16* Name;
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* Fs;
    CHAR16* Path;
    CHAR16* Cmdline;
    BOOLEAN Tracing;
} BOOT_ENTRY;

extern BOOT_ENTRY gEntry;

EFI_STATUS GetBootEntry();
