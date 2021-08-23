#include <Uefi.h>
#include <Protocol/LoadedImage.h>
#include <Library/BaseLib.h>
#include <Library/FileHandleLib.h>
#include <Library/DevicePathLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>

#include "../util/macros.h"
#include "../util/misc.h"
#include "BootConfig.h"

static CHAR16* ConfigPaths[] =
{
    L"origami.cfg",
    L"boot\\origami.cfg"
};

BOOT_ENTRY gEntry;

static CHAR16* CopyString(CHAR16* String)
{
    return AllocateCopyPool((1 + StrLen(String)) * sizeof(CHAR16), String);
}

static EFI_STATUS LoadBootEntry(EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* FS, BOOT_ENTRY* Entry)
{
    EFI_STATUS Status = EFI_SUCCESS;
    EFI_FILE_PROTOCOL* Root = NULL;
    EFI_FILE_PROTOCOL* ConfigFile = NULL;

    ASSERT_EXPR(FS != NULL, "No Filesystem found");

    CHECK_STATUS(FS->OpenVolume(FS, &Root), "Failed to open BootFs");

    for(int i = 0; i < ARRAY_SIZE(ConfigPaths); i++)
    {
        if(!EFI_ERROR(Root->Open(Root, &ConfigFile, ConfigPaths[i], EFI_FILE_MODE_READ, 0)))
        {
            Print(L"[INFO] Config file found at %s\n", ConfigPaths[i]);
            break;
        }

        ConfigFile = NULL;
    }

    if(ConfigFile == NULL)
    {
        Print(L"[ERROR] No configuration file found at default paths.\n");
        Status = EFI_NOT_FOUND;
        goto end;
    }

    while(TRUE)
    {
        CHAR16 Line[255];
        UINTN LineSize = 255;
        BOOLEAN Ascii = TRUE;

        if(FileHandleEof(ConfigFile))
            break;

        CHECK_STATUS(FileHandleReadLine(ConfigFile, Line, &LineSize, FALSE, &Ascii), "Failed to read config file");

        if(Line[0] == L':')
        {
            Entry->Fs = FS;
            Entry->Name = CopyString(Line + 1);
            Entry->Cmdline = L"";
        }
        else
        {
            if(CHECK_OPTION(L"PATH") || CHECK_OPTION(L"KERNEL_PATH"))
            {
                Entry->Path = CopyString(StrStr(Line, L"=") + 1);
            }
            else if(CHECK_OPTION(L"CMDLINE") || CHECK_OPTION(L"KERNEL_CMDLINE"))
            {
                Entry->Cmdline = CopyString(StrStr(Line, L"=") + 1);
            }
            else if(CHECK_OPTION(L"TRACING") || CHECK_OPTION(L"BOOT_TRACING"))
            {
                if(StrnCmp(StrStr(Line, L"=") + 1, L"yes", 3) == 0)
                    Entry->Tracing = TRUE;
            }
        }
    }

end:
    if(ConfigFile != NULL)
        FileHandleClose(ConfigFile);

    if(Root != NULL)
        FileHandleClose(Root);

    return Status;
}

EFI_STATUS GetBootEntry()
{
    EFI_STATUS Status = EFI_SUCCESS;
    EFI_LOADED_IMAGE_PROTOCOL* LoadedImage = NULL;
    EFI_DEVICE_PATH_PROTOCOL* BootDevicePath = NULL;
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* BootFs = NULL;
    EFI_HANDLE FsHandle = NULL;

    CHECK_STATUS(gBS->HandleProtocol(gImageHandle, &gEfiLoadedImageProtocolGuid, (VOID**)&LoadedImage), "Couldn't open Boot Filesystem");
    CHECK_STATUS(gBS->HandleProtocol(LoadedImage->DeviceHandle, &gEfiDevicePathProtocolGuid, (VOID**)&BootDevicePath), "Couldn't open Boot Filesystem");
    CHECK_STATUS(gBS->LocateDevicePath(&gEfiSimpleFileSystemProtocolGuid, &BootDevicePath, &FsHandle), "Couldn't open Boot Filesystem");
    CHECK_STATUS(gBS->HandleProtocol(FsHandle, &gEfiSimpleFileSystemProtocolGuid, (VOID**)&BootFs), "Couldn't open Boot Filesystem");

    CHECK_STATUS(LoadBootEntry(BootFs, &gEntry), "Couldn't get Boot Entry");
    
    ASSERT_EXPR(gEntry.Path != NULL, "Boot Entry Format is not valid or it has missing fields");

    DO_IF(Print(L"Boot entry information:\n"), gEntry.Tracing);
    DO_IF(Print(L" - Name: %s\n", gEntry.Name), gEntry.Tracing);
    DO_IF(Print(L" - Cmdline: %s\n", gEntry.Cmdline), gEntry.Tracing);
    DO_IF(Print(L" - Path: %s\n", gEntry.Path), gEntry.Tracing);
    DO_IF(Print(L" - Tracing: %a\n", gEntry.Tracing ? "yes" : "no"), gEntry.Tracing);
    DO_IF(KeyWait(L""), gEntry.Tracing);

end:
    return Status;
}
