#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Protocol/GraphicsOutput.h>

#include "macros.h"
#include "misc.h"

EFI_STATUS KeyWait(CHAR16* String, ...)
{
    EFI_STATUS Status;
    EFI_INPUT_KEY Key;

    VA_LIST Args;
    VA_START(Args, String);

    Print(String, Args);
    Print(L"\n");

    gST->ConOut->OutputString(gST->ConOut, L"Press any key to continue...");

    // Clear keystroke buffer
    gST->ConIn->Reset(gST->ConIn, FALSE);

    // Poll for key
    while((Status = (gST->ConIn->ReadKeyStroke(gST->ConIn, &Key))) == EFI_NOT_READY);

    // Clear keystroke buffer (this is just a pause)
    gST->ConIn->Reset(gST->ConIn, FALSE);

    Print(L"\n\n");

end:
    VA_END(Args);
    return Status;
}

EFI_STATUS SelectBestGopMode(UINTN* Width, UINTN* Height, UINTN* Scanline, EFI_PHYSICAL_ADDRESS* Framebuffer)
{
    EFI_STATUS Status = EFI_SUCCESS;

    // get the protocol
    EFI_GRAPHICS_OUTPUT_PROTOCOL* GOP = NULL;
    CHECK_STATUS(gBS->LocateProtocol(&gEfiGraphicsOutputProtocolGuid, NULL, (VOID**)&GOP), "Failed to get Graphics Protocol");

    if(*Width == 0 || Width == NULL)
        *Width = MAX_UINTN;

    if(*Height == 0 || Height == NULL)
        *Height = MAX_UINTN;

    UINT32 GoodOption = GOP->Mode->Mode;
    INT32 BestWidth = 0;
    INT32 BestHeight = 0;
    for(int i = 0; i < GOP->Mode->MaxMode; i++)
    {
        EFI_GRAPHICS_OUTPUT_MODE_INFORMATION* Info = NULL;
        UINTN SizeOfInfo = sizeof(EFI_GRAPHICS_OUTPUT_MODE_INFORMATION);
        CHECK_STATUS(GOP->QueryMode(GOP, i, &SizeOfInfo, &Info), "Failed to get information about the graphics mode");

        if(Info->PixelFormat != PixelBlueGreenRedReserved8BitPerColor)
            continue;

        if(Info->VerticalResolution == *Height && Info->HorizontalResolution == *Width)
        {
            GoodOption = i;
            break;
        }

        if(
            (Info->VerticalResolution > *Height && Info->HorizontalResolution > *Width) ||
            (Info->VerticalResolution < BestHeight && Info->HorizontalResolution < BestWidth)
        )
            continue;

        BestWidth = Info->HorizontalResolution;
        BestHeight = Info->VerticalResolution;
        GoodOption = i;
    }

    CHECK_STATUS(GOP->SetMode(GOP, GoodOption), "Failed to set the graphics mode");
    *Width = GOP->Mode->Info->VerticalResolution;
    *Height = GOP->Mode->Info->HorizontalResolution;
    *Scanline = GOP->Mode->Info->PixelsPerScanLine;
    *Framebuffer = GOP->Mode->FrameBufferBase;

end:
    return Status;
}
