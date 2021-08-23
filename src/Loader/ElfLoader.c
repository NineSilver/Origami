#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/FileHandleLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Protocol/SimpleFileSystem.h>

#include "../util/macros.h"
#include "elf64.h"

static EFI_STATUS FileRead(EFI_FILE_PROTOCOL* File, VOID* Buffer, UINTN ExpectedSize, UINTN Offset)
{
    EFI_STATUS Status = EFI_SUCCESS;

    ASSERT_EXPR(File != NULL, "Couldn't read from the file, as it doesn't exist");
    CHECK_STATUS(FileHandleSetPosition(File, Offset), "Couldn't read from the file");

    UINTN ReadSize = ExpectedSize;
    CHECK_STATUS(FileHandleRead(File, &ReadSize, Buffer), "Couldn't read from the file");

    ASSERT_EXPR(ExpectedSize == ReadSize, "Incorrect read buffer size");

end:
    return Status;
}

EFI_STATUS LoadSection(EFI_FILE_PROTOCOL* File, VOID** Buffer, UINTN* Size, const CHAR8* Name)
{
    EFI_STATUS Status = EFI_SUCCESS;
    CHAR8* Names = NULL;
    ASSERT_EXPR(File != NULL, "Couldn't read from the file, as it does not exist");

    Elf64_Ehdr Ehdr = {0};
    CHECK_STATUS(FileRead(File, &Ehdr, sizeof(Elf64_Ehdr), 0), "Couldn't read from the file");

    ASSERT_EXPR(IS_ELF(Ehdr), "Invalid ELF magic!");
    ASSERT_EXPR(Ehdr.e_ident[EI_DATA] == ELFDATA2LSB, "Not 2s complement little endian ELF");
    ASSERT_EXPR(Ehdr.e_ident[EI_VERSION] == EV_CURRENT, "Not the current ELF version");
    ASSERT_EXPR(Ehdr.e_machine == EM_X86_64, "Not x86_64 ELF");

    Elf64_Shdr Shstr = {0};
    CHECK_STATUS(FileRead(File, &Shstr, sizeof(Elf64_Shdr), Ehdr.e_shoff + Ehdr.e_shstrndx * Ehdr.e_shentsize), "Couldn't read from the file");
    Names = AllocatePool(Shstr.sh_size);
    ASSERT_EXPR(Names != NULL, "System ran out of resources");
    CHECK_STATUS(FileRead(File, Names, Shstr.sh_size, Shstr.sh_offset), "Couldn't read from the file");

    Elf64_Shdr Section = {0};
    VOID* NewBuffer = NULL;
    for(UINTN i = 0; i < Ehdr.e_shnum; i++)
    {
        CHECK_STATUS(FileRead(File, &Section, sizeof(Elf64_Shdr), Ehdr.e_shoff + Ehdr.e_shentsize * i), "Couldn't read from the file");
        if(AsciiStrCmp(&Names[Section.sh_name], Name) == 0)
        {
            NewBuffer = AllocatePool(Section.sh_size);
            ASSERT_EXPR(NewBuffer != NULL, "System ran out of resources");
            CHECK_STATUS(FileRead(File, NewBuffer, Section.sh_size, Section.sh_offset), "Couldn't read from the file");
            break;
        }
    }
    
    ASSERT_EXPR(NewBuffer != NULL, "Couldn't find requested section");

    *Buffer = NewBuffer;
    *Size = Section.sh_size;

end:
    if(Names != NULL)
        FreePool(Names);
    
    return Status;
}
