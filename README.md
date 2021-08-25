# Origami Bootloader

Origami is a x86_64 UEFI bootloader and boot protocol/specification meant for hobbist kernels, providing modern features like ACPI and KASLR while keeping things ultra-simple. It is inspired in [Stivale](https://github.com/stivale/stivale) boot protocol and in the now archived [TomatBoot](https://github.com/TomatOrg/TomatBoot).

## Features

- [X] Complete ELF load
- [x] Higher half mapping
- [x] Custom protocol
- [x] Runs on baremetal
- [X] Boot Menu
- [ ] KASLR and relocatable kernels support
- [ ] Level 5 paging
- [ ] Variable number of boot entries

## Building

You will need clang and lld; both of them use the LLVM architecture to cross compile apps, which will allow us to link the objects and transform the executable into a EFI app (see the [Makefile](./Makefile) for further information).

Run the command `make all` to generate a bootable UEFI app inside the `bin` folder. To test using QEMU run `make test`. You can speed up the build process (of building all the EDK2 library) by passing the flags `-j <nproc + 1> -l <nproc>` to the previous command.

## About the protocol

Origami has been designed to provide hobbist OS developers an easy boot protocol to rely on. This means it is kept simple (following KISS philosophy) yet powerful, giving ACPI, UNIX Epoch and a frambuffer as the very basics to build great kernels, no SMP nor TTY support. Origami is not meant to replace Stivale or Multiboot; in fact, it is a project I did to learn.

### Origami Barebones

Firstly copy the [Spec file](./spec/origami.h) to the directory from where your kernel will be developed. Then create the kernel main file containing the entry point, a stack and the Origami header with the appropriate flags that suit your needs. You can take a look at the [Test kernel](./tests). Modify the kernel entry point to do whatvere you want. Finally, create a `linker.ld` fileand fill it with the executable sections.

To build and test the kernel, I assume you understand what is a freestanding toolchain and a cross compiler and the args you have to pass to the compiler. The minimal command would look like `gcc -nostdlib -ffreestanding -fno-stack-protector -fno-pic -mcmodel=kernel -mno-red-zone -mno-sse -mno-sse2 -c kernel.c -o kernel .o`, and it will link with `-nostdlib -static -fno-pic -z max-page-size=0x1000 -T linker.ld kernel.o -o kernel.elf`.

#### Test on QEMU

Create a directory containing the following structure:
```
./
 |__ EFI
      |__ BOOT
```
Place the compiled `BOOTX64.EFI` file into the `BOOT` folder and the `kernel.elf` at the root. Then create `origami.cfg` at the root and fill it following the [Spec]().
Finally, issue `qemu-system-x86_64 -machine q35 -m 4G -smp 4 -debugcon stdio -hda fat:rw:<directory name>`.

#### Test on baremetal

Format an USB drive with FAT32 and follow steps above. Then shutdown your computer. disable Secure Boot and use the USB as boot drive.

### Technical information

When the kernel is booted, all the registers are set to 0 except `rdi`, which contains the boot parameters according to SysV x86_64 ABI, and `rsp`, which is set to the value of `origami_header.stack`. `gBS->ExitBootServices()` is called just before jumping into the kernel. The kernel must load its own GDT immediately.

## About the configuration files

The format used by Origami is very similar to [Limine](https://github.com/limine-bootloader/limine/blob/trunk/test/limine.cfg) to keep the parser simple, using `:Name` to denote the boot entry name, `PATH` or `KERNEL_PATH` to indicate the kernel image file, `CMDLINE` to pass parameters to the kernel and `TRACING` to enabel bootloader debug messages.
An example can be found at [config/example.cfg](config/example.cfg).

## Acknowledgements

This project is hugely inspired by [TomatBoot](https://github.com/TomatOrg/TomatBoot), using its library and build system; and also by the [Stivale](https://github.com/stivale/stivale) boot protocol.
