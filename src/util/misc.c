#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>

#include "macros.h"

EFI_STATUS KeyWait(CHAR16* String, ...)
{
    EFI_STATUS Status;
    EFI_INPUT_KEY Key;

    VA_LIST Args;
    VA_START(Args, String);

    Print(String, Args);
    Print(L"\n");

    CHECK_STATUS(gST->ConOut->OutputString(gST->ConOut, L"Press any key to continue..."), "Couldn't print to gST Console Output");

    // Clear keystroke buffer
    CHECK_STATUS(gST->ConIn->Reset(gST->ConIn, FALSE), "Couldn't reset gST Console Input");

    // Poll for key
    while((Status = (gST->ConIn->ReadKeyStroke(gST->ConIn, &Key))) == EFI_NOT_READY);

    // Clear keystroke buffer (this is just a pause)
    CHECK_STATUS(gST->ConIn->Reset(gST->ConIn, FALSE), "Couldn't reset gST Console Input");

    Print(L"\n\n");

end:
    VA_END(Args);
    return Status;
}
