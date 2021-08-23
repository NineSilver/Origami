#include <Uefi.h>

#include <Library/RngLib.h>
#include <Library/BaseLib.h>
#include <Library/TimeBaseLib.h>
#include <Library/UefiLib.h>
#include <Library/FileHandleLib.h>
#include <Library/UefiRuntimeLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>

#include <Protocol/GraphicsOutput.h>

#include "../Config/BootConfig.h"
#include "../util/macros.h"
#include "../util/misc.h"
#include "ElfLoader.h"
#include "Origami.h"

#define GET_FLAGS_STR(hdr, pos) (CHECK_BIT(hdr->Flags, pos) ? "yes" : "no")

/* static EFI_STATUS CheckBootEntry(BOOT_ENTRY* Entry)
{
    if((Entry->Name == NULL) || (Entry->Path == NULL) || (Entry->Fs = NULL))
        return EFI_INVALID_PARAMETER;

    return EFI_SUCCESS;
} */

EFI_STATUS LoadOrigamiKernel(BOOT_ENTRY* Entry)
{
    EFI_STATUS Status = EFI_SUCCESS;

    EFI_FILE_PROTOCOL* Root = NULL;
    CHECK_STATUS(Entry->Fs->OpenVolume(Entry->Fs, (EFI_FILE_PROTOCOL**)&Root), L"Couldn't open BootFS");

    EFI_FILE_PROTOCOL* Kernel = NULL;
    CHECK_STATUS(Root->Open(Root, (EFI_FILE_PROTOCOL**)&Kernel, Entry->Path, EFI_FILE_MODE_READ, 0), L"Couldn't open Kernel image");

    struct ORIGAMI_HEADER* Header = NULL;
    UINTN HeaderSize;
    CHECK_STATUS(LoadSection(Kernel, (VOID**)&Header, &HeaderSize, ".origamihdr"), "Cannot retrieve Origami Header");
    ASSERT_EXPR(HeaderSize == sizeof(struct ORIGAMI_HEADER), "Incorrect size of Origami Header");

    Print(L"[INFO] Found Origami Header in kernel %s\n", Entry->Name);

    DO_IF(Print(L"Origami Header information:\n"), Entry->Tracing);
    DO_IF(Print(L" - Entry Point: %p\n", Header->EntryPoint), Entry->Tracing);
    DO_IF(Print(L" - Stack: %p\n", Header->Stack), Entry->Tracing);
    DO_IF(Print(L" - Framebuffer requested: %a\n", GET_FLAGS_STR(Header, 6)), Entry->Tracing);
    DO_IF(Print(L" - ACPI RSDP requested: %a\n", GET_FLAGS_STR(Header, 3)), Entry->Tracing);
    DO_IF(Print(L" - UNIX Epoch requested: %a\n", GET_FLAGS_STR(Header, 2)), Entry->Tracing);
    DO_IF(KeyWait(L""), Entry->Tracing);

end:
    return Status;
}
