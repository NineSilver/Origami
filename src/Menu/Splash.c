#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Uefi.h>

#include "../Config/BootConfig.h"
#include "../Loader/Origami.h"
#include "../util/macros.h"
#include "Splash.h"

STATIC CHAR8* BootLogo[] = {
    "        _____                   /|",
    "        |   \\      ____        / |",
    "  __    |    \\    /\\   |      /  ;",
    " /\\  \\  |     \\  /  \\  |     /  ;",
    "/,'\\  \\ |      \\/  : \\ |    /   ;",
    "~  ;   \\|      /   :  \\|   /   ;",
    "   |    \\     /   :'  |   /    ;",
    "   |     \\   /    :   |  /    ;                Press Enter to boot",
    "   |      \\ /    :'   | /     ;                Press ESC to reset the computer",
    "   |       /     :    |/     ;",
    "   |      /     :'    |      ;",
    "    \\    /      :     |     ;",
    "     \\  /      :'     |     ;",
    "      \\       :'      |    ;",
    "       \\______:_______|___;\n",
};

EFI_STATUS SplashScreen()
{
    EFI_STATUS Status = EFI_SUCCESS;

    // No need to check the Status returned from these functions
    gST->ConOut->ClearScreen(gST->ConOut);

    gST->ConOut->SetAttribute(gST->ConOut, EFI_RED);
    Print(L"Origami Bootloader\n\n");

    gST->ConOut->SetAttribute(gST->ConOut, EFI_WHITE);
    for(int i = 0; i < ARRAY_SIZE(BootLogo); i++)
        Print(L"%a\n", BootLogo[i]);

    gST->ConIn->Reset(gST->ConIn, FALSE);
    EFI_INPUT_KEY Key;
    gST->ConIn->ReadKeyStroke(gST->ConIn, &Key);

    while(Key.ScanCode != SCAN_ESC)
    {
        if(Key.UnicodeChar == CHAR_CARRIAGE_RETURN)
        {
            gST->ConOut->ClearScreen(gST->ConOut);
            CHECK_STATUS(GetBootEntry(), "Failed to get Boot Entry");
            CHECK_STATUS(LoadOrigamiKernel(&gEntry), "Failed to load the kernel");
        }

        gST->ConIn->ReadKeyStroke(gST->ConIn, &Key);
    }

    gRT->ResetSystem(EfiResetCold, EFI_SUCCESS, 0, NULL);

end:
    return Status;
}
