#pragma once

#include <Uefi.h>

EFI_STATUS KeyWait(CHAR16* String, ...);
EFI_STATUS SelectBestGopMode(UINTN* Width, UINTN* Height, UINTN* Scanline, EFI_PHYSICAL_ADDRESS* Framebuffer);
