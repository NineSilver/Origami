#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>

#include "Config/BootConfig.h"
#include "Loader/Origami.h"
#include "util/macros.h"

EFI_STATUS EFIAPI UefiBootServicesTableLibConstructor(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable);
EFI_STATUS EFIAPI UefiRuntimeServicesTableLibConstructor(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable);
EFI_STATUS EFIAPI RuntimeDriverLibConstruct(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable);
EFI_STATUS EFIAPI DxeDebugLibConstructor(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable);
EFI_STATUS EFIAPI BaseRngLibConstructor();

EFI_STATUS EfiMain(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable)
{
    EFI_STATUS Status = EFI_SUCCESS;

    CHECK_STATUS(UefiBootServicesTableLibConstructor(ImageHandle, SystemTable), "An error has been invoked during the execution of the program");
    CHECK_STATUS(UefiRuntimeServicesTableLibConstructor(ImageHandle, SystemTable), "An error has been invoked during the execution of the program");
    CHECK_STATUS(RuntimeDriverLibConstruct(ImageHandle, SystemTable), "An error has been invoked during the execution of the program");
    CHECK_STATUS(DxeDebugLibConstructor(ImageHandle, SystemTable), "An error has been invoked during the execution of the program");
    CHECK_STATUS(BaseRngLibConstructor(), "An error has been invoked during the execution of the program");

    CHECK(SystemTable->ConOut->ClearScreen(SystemTable->ConOut), "An error has been invoked during the execution of the program");

    Print(L"Origami Bootloader - Hello, World!\n");
    Print(L"==================================\n\n");

    CHECK_STATUS(GetBootEntry(), "An error has been invoked during the execution of the program");
    CHECK_STATUS(LoadOrigamiKernel(&gEntry), "Error trying to load the kernel");

end:
    CpuDeadLoop();
    return Status;
}
