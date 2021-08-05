#include <Uefi.h>
#include <Protocol/LoadedImage.h>
#include <Library/BaseLib.h>
#include <Library/FileHandleLib.h>
#include <Library/DevicePathLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/MemoryAllocationLib.h>

#include "BootConfig.h"
#include "../util/macros.h"

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

static EFI_STATUS LoadBootEntry(EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* FS, BOOT_ENTRY* Cfg)
{
    EFI_STATUS Status = EFI_SUCCESS;
    EFI_FILE_PROTOCOL* Root = NULL;
    EFI_FILE_PROTOCOL* ConfigFile = NULL;

    ASSERT_EXPR(FS != NULL, L"No Filesystem found");

    CHECK_STATUS(FS->OpenVolume(FS, &Root), L"Failed to open BootFs");

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
        //CHAR16* Line = AllocatePool(255);
        CHAR16 Line[255];
        UINTN LineSize = 255;
        BOOLEAN Ascii = TRUE;

        if(FileHandleEof(ConfigFile))
            break;

        CHECK_STATUS(FileHandleReadLine(ConfigFile, Line, &LineSize, FALSE, &Ascii), L"Failed to read config file");

        if(Line[0] == L':')
        {
            Cfg->Fs = FS;
            Cfg->Name = CopyString(Line + 1);
            Cfg->Cmdline = L"";
        }
        else
        {
            if(CHECK_OPTION(L"PATH") || CHECK_OPTION(L"KERNEL_PATH"))
                Cfg->Path = CopyString(StrStr(Line, L"=") + 1);
            else if(CHECK_OPTION(L"CMDLINE") || CHECK_OPTION(L"KERNEL_CMDLINE"))
                Cfg->Cmdline = CopyString(StrStr(Line, L"=") + 1);
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

    CHECK_STATUS(gBS->HandleProtocol(gImageHandle, &gEfiLoadedImageProtocolGuid, (VOID**)&LoadedImage), L"Couldn't open Boot Filesystem");
    CHECK_STATUS(gBS->HandleProtocol(LoadedImage->DeviceHandle, &gEfiDevicePathProtocolGuid, (VOID**)&BootDevicePath), L"Couldn't open Boot Filesystem");
    CHECK_STATUS(gBS->LocateDevicePath(&gEfiSimpleFileSystemProtocolGuid, &BootDevicePath, &FsHandle), L"Couldn't open Boot Filesystem");
    CHECK_STATUS(gBS->HandleProtocol(FsHandle, &gEfiSimpleFileSystemProtocolGuid, (VOID**)&BootFs), L"Couldn't open Boot Filesystem");

    return LoadBootEntry(BootFs, &gEntry);

end:
    return Status;
}
