# Origami Bootloader

Origami is a x86_64 UEFI bootloader and boot protocol/specification meant for hobbist kernels, providing modern features like ACPI and KASLR while keeping things ultra-simple. It is inspired in [Stivale](https://github.com/stivale/stivale) boot protocol and in the now archived [TomatBoot](https://github.com/TomatOrg/TomatBoot).

## Building

You will need clang and lld; both of them use the LLVM architecture to cross compile apps, which will allow us to link the objects and transform the executable into a EFI app (see the [Makefile](./Makefile) for further information).

Run the command `make all` to generate a bootable UEFI app inside the `bin` folder. To test using QEMU run `make test`. You can speed up the build process (of building all the EDK2 library) by passing the flags `-j <nproc + 1> -l <nproc>` to the previous command.
