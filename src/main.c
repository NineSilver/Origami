#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>

#include "Config/BootConfig.h"
#include "util/macros.h"

EFI_STATUS EFIAPI UefiBootServicesTableLibConstructor(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable);
EFI_STATUS EFIAPI UefiRuntimeServicesTableLibConstructor(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable);
EFI_STATUS EFIAPI RuntimeDriverLibConstruct(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable);
EFI_STATUS EFIAPI DxeDebugLibConstructor(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable);
EFI_STATUS EFIAPI BaseRngLibConstructor();

EFI_STATUS EfiMain(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable)
{
    EFI_STATUS Status = EFI_SUCCESS;

    CHECK_STATUS(UefiBootServicesTableLibConstructor(ImageHandle, SystemTable), L"An error has been invoked during the execution of the program");
    CHECK_STATUS(UefiRuntimeServicesTableLibConstructor(ImageHandle, SystemTable), L"An error has been invoked during the execution of the program");
    CHECK_STATUS(RuntimeDriverLibConstruct(ImageHandle, SystemTable), L"An error has been invoked during the execution of the program");
    CHECK_STATUS(DxeDebugLibConstructor(ImageHandle, SystemTable), L"An error has been invoked during the execution of the program");
    CHECK_STATUS(BaseRngLibConstructor(), L"An error has been invoked during the execution of the program");

    CHECK(SystemTable->ConOut->ClearScreen(SystemTable->ConOut), L"An error has been invoked during the execution of the program");

    Print(L"Origami Bootloader - Hello, World!\n");
    Print(L"==================================\n\n");

    CHECK_STATUS(GetBootEntry(), L"An error has been invoked during the execution of the program");

    Print(L"Boot entry information:\n");
    Print(L" - Name: %s\n", gEntry.Name);
    Print(L" - Cmdline: %s\n", gEntry.Cmdline);
    Print(L" - Path: %s\n", gEntry.Path);
    
end:
    while(TRUE);
}
