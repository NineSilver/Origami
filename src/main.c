#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>

#include "util/macros.h"

EFI_STATUS EFIAPI UefiBootServicesTableLibConstructor(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable);
EFI_STATUS EFIAPI UefiRuntimeServicesTableLibConstructor(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable);
EFI_STATUS EFIAPI RuntimeDriverLibConstruct(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable);
EFI_STATUS EFIAPI DxeDebugLibConstructor(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable);
EFI_STATUS EFIAPI BaseRngLibConstructor();

EFI_STATUS EfiMain(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable)
{
    EFI_STATUS Status = EFI_SUCCESS;

    CHECK_STATUS(UefiBootServicesTableLibConstructor(ImageHandle, SystemTable), L"[ERROR] An error has been invoked during the execution of the program. Halt.\n");
    CHECK_STATUS(UefiRuntimeServicesTableLibConstructor(ImageHandle, SystemTable), L"[ERROR] An error has been invoked during the execution of the program. Halt.\n");
    CHECK_STATUS(RuntimeDriverLibConstruct(ImageHandle, SystemTable), L"[ERROR] An error has been invoked during the execution of the program. Halt.\n");
    CHECK_STATUS(DxeDebugLibConstructor(ImageHandle, SystemTable), L"[ERROR] An error has been invoked during the execution of the program. Halt.\n");
    CHECK_STATUS(BaseRngLibConstructor(), L"[ERROR] An error has been invoked during the execution of the program. Halt.\n");

    SystemTable->ConOut->ClearScreen(SystemTable->ConOut);

    Print(L"Origami Bootloader - Hello, World!\n");
    Print(L"==================================\n\n");

end:
    while(TRUE);
}