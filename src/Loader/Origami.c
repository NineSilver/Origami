#include <Uefi.h>

#include <Guid/Acpi.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/FileHandleLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/RngLib.h>
#include <Library/TimeBaseLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiRuntimeLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>

#include <Protocol/GraphicsOutput.h>

#include "../Config/BootConfig.h"
#include "../util/macros.h"
#include "../util/misc.h"
#include "ElfLoader.h"
#include "Origami.h"

#define GET_FLAGS_STR(hdr, pos) (CHECK_BIT(hdr->Flags, pos) ? "yes" : "no")

static enum E820_TYPE EfiTypeToE820Type[] = {
    [EfiReservedMemoryType] = E820_TYPE_RESERVED,
    [EfiRuntimeServicesCode] = E820_TYPE_RAM,
    [EfiRuntimeServicesData] = E820_TYPE_RAM,
    [EfiMemoryMappedIO] = E820_TYPE_RESERVED,
    [EfiMemoryMappedIOPortSpace] = E820_TYPE_RESERVED_KERN,
    [EfiPalCode] = E820_TYPE_RESERVED,
    [EfiUnusableMemory] = E820_TYPE_UNUSABLE,
    [EfiACPIReclaimMemory] = E820_TYPE_ACPI,
    [EfiLoaderCode] = E820_TYPE_RAM,
    [EfiLoaderData] = E820_TYPE_RAM,
    [EfiBootServicesCode] = E820_TYPE_RAM,
    [EfiBootServicesData] = E820_TYPE_RAM,
    [EfiConventionalMemory] = E820_TYPE_RAM,
    [EfiACPIMemoryNVS] = E820_TYPE_NVS};

extern VOID EFIAPI JumpToKernel(struct ORIGAMI_STRUCT* Strct, UINT64 Stack, VOID* Entry);

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

    Print(L"[INFO] Loading Origami-compliant kernel...");
    DO_IF(KeyWait(L""), Entry->Tracing);

    ELF_INFO Info = {0};
    CHECK_STATUS(LoadElf(Kernel, &Info, EfiMemoryMappedIOPortSpace), "Couldn't load the kernel image into memory");
    if(Header->EntryPoint != 0)
        Info.EntryPoint = Header->EntryPoint;

    struct ORIGAMI_STRUCT* Struct = AllocateZeroPool(sizeof(struct ORIGAMI_STRUCT));
    CHECK_STATUS(Struct != NULL, "System ran out of resources");

    Print(L"[INFO] Setting up the struct...\n");

    AsciiStrnCpy(Struct->BootloaderID, "Origami - UEFI", sizeof(Struct->BootloaderID));
    Struct->OptInfo = 0b10010000;
    if(Entry->Cmdline != NULL)
        Struct->Cmdline = (UINT64)Entry->Cmdline;

    Print(L"[INFO] Finding framebuffer...\n");
    DO_IF(KeyWait(L""), Entry->Tracing);

    UINTN Width = 0;
    UINTN Height = 0;
    UINTN Scanline = Width;
    EFI_PHYSICAL_ADDRESS Framebuffer = 0;
    CHECK_STATUS(SelectBestGopMode(&Width, &Height, &Scanline, &Framebuffer), "Failed to get graphics framebuffer");

    if(CHECK_BIT(Header->Flags, 6))
    {
        struct ORIGAMI_FRAMEBUFFER* FB = AllocateZeroPool(sizeof(struct ORIGAMI_FRAMEBUFFER));

        FB->FramebufferBpp = 32;
        FB->FramebufferWidth = Width;
        FB->FramebufferHeight = Height;
        FB->FramebufferPitch = Scanline;
        FB->FramebufferAddress = Framebuffer;

        Struct->Framebuffer = (UINT64)FB;
        Struct->OptInfo |= (1 << 6);
    }

    if(CHECK_BIT(Header->Flags, 3))
    {
        VOID* AcpiTable = NULL;

        if(!EFI_ERROR(EfiGetSystemConfigurationTable(&gEfiAcpi20TableGuid, &AcpiTable)))
        {
            EFI_ACPI_2_0_ROOT_SYSTEM_DESCRIPTION_POINTER* ActualRsdp = AcpiTable;
            Struct->Rsdp = (UINT64)AllocateReservedCopyPool(ActualRsdp->Length, ActualRsdp);
            Print(L"[INFO] Got ACPI 2.0 at %p", Struct->Rsdp);
            Struct->OptInfo |= (1 << 4);
        }
        else if(!EFI_ERROR(EfiGetSystemConfigurationTable(&gEfiAcpi10TableGuid, &AcpiTable)))
        {
            Struct->Rsdp = (UINT64)AllocateReservedCopyPool(sizeof(EFI_ACPI_1_0_ROOT_SYSTEM_DESCRIPTION_POINTER), AcpiTable);
            Print(L"[INFO] Got ACPI 1.0 at %p", Struct->Rsdp);
            Struct->OptInfo |= (1 << 4);
        }
        else
        {
            Print(L"[ERROR] No ACPI Table found, setting to 0 instead...");
        }

        DO_IF(KeyWait(L""), Entry->Tracing);
    }

    if(CHECK_BIT(Header->Flags, 2))
    {
        EFI_TIME CurrentTime = {0};
        if(!EFI_ERROR(EfiGetTime(&CurrentTime, NULL)))
        {
            Struct->Epoch = EfiTimeToEpoch(&CurrentTime);
            Print(L"[INFO] Got epoch %d", Struct->Epoch);
        }
        else
        {
            Print(L"[ERROR] Failed to get system time, setting to 0 instead...\n");
        }

        DO_IF(KeyWait(L""), Entry->Tracing);
    }

    Print(L"[INFO] Preparing Higher Half\n");
    DO_IF(KeyWait(L""), Entry->Tracing);

    IA32_CR0 Cr0 = {.UintN = AsmReadCr0()};
    Cr0.Bits.WP = 0;
    AsmWriteCr0(Cr0.UintN);

    UINT64* Pml4 = (UINT64*)AsmReadCr3();
    Pml4[256] = Pml4[0];

    UINT64* Pml3High = AllocateReservedPages(1);
    SetMem(Pml3High, EFI_PAGE_SIZE, 0);
    Pml4[511] = ((UINT64)Pml3High) | 0x3u;

    UINT64* Pml3Low = (UINT64*)(Pml4[0] & 0x7ffffffffffff000u);
    Pml3High[510] = Pml3Low[0];
    Pml3High[511] = Pml3Low[1];

    Print(L"[INFO] Getting Memory Map...\n");
    DO_IF(KeyWait(L""), Entry->Tracing);

    gST->ConOut->ClearScreen(gST->ConOut);

    UINTN DescriptorSize = 0;
    UINT32 DescriptorVersion = 0;
    UINTN MemoryMapSize = 0;
    UINTN MapKey = 0;
    VOID* Descriptors = NULL;
    Status = (gBS->GetMemoryMap(&MemoryMapSize, Descriptors, &MapKey, &DescriptorSize, &DescriptorVersion));
    ASSERT_EXPR(Status == EFI_BUFFER_TOO_SMALL, "Memory map is too small");
    ASSERT_EXPR(DescriptorVersion == EFI_MEMORY_DESCRIPTOR_VERSION, "Invalid UEFI descriptor version");
    Status = EFI_SUCCESS;
    MemoryMapSize += DescriptorSize * 10;
    Descriptors = AllocatePool(MemoryMapSize);
    CHECK_STATUS(Descriptors != NULL, EFI_OUT_OF_RESOURCES);
    CHECK_STATUS(gBS->GetMemoryMap(&MemoryMapSize, Descriptors, &MapKey, &DescriptorSize, &DescriptorVersion), "Couldn't get memory map");

    CHECK_STATUS(gBS->ExitBootServices(gImageHandle, MapKey), "Failed to exit Boot Services");

    DisableInterrupts();

    Struct->Memmap = Descriptors;
    struct E820_ENTRY* Current = Descriptors;

    for(UINTN Index = 0; Index < MemoryMapSize / DescriptorSize; Index++)
    {
        EFI_MEMORY_DESCRIPTOR* Descriptor = (EFI_MEMORY_DESCRIPTOR*)((UINT64)Descriptors + DescriptorSize * Index);

        UINT32 Type = E820_TYPE_RESERVED;
        if(Descriptor->Type < EfiMaxMemoryType)
            Type = EfiTypeToE820Type[Descriptor->Type];

        if(
            Index != 0 &&
            Current->type == Type &&
            Current->address + Current->size == Descriptor->PhysicalStart)
        {
            Current->size += EFI_PAGES_TO_SIZE(Descriptor->NumberOfPages);
        }
        else
        {
            if(Index != 0)
                Current++;

            Current->address = Descriptor->PhysicalStart;
            Current->size = EFI_PAGES_TO_SIZE(Descriptor->NumberOfPages);
            Current->type = Type;

            Struct->MemmapEntries++;
        }
    }

    JumpToKernel(Struct, Header->Stack, (VOID*)Info.EntryPoint);

    Print(L"[ERROR] The Kernel returned to the bootloader. This should not happen.");
    CpuDeadLoop();

end:
    return Status;
}
