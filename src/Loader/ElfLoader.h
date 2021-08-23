#pragma once

#include <Uefi.h>
#include <Protocol/SimpleFileSystem.h>

EFI_STATUS LoadSection(EFI_FILE_PROTOCOL* File, VOID** Buffer, UINTN* Size, const CHAR8* Name);
