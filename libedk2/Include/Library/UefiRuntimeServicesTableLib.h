/** @file
  Provides a service to retrieve a pointer to the EFI Runtime Services Table.

  This library does not contain any functions or macros.  It simply exports the
  global variable gRT that is a pointer to the EFI Runtime Services Table as defined
  in the UEFI Specification.  The global variable gRT must be preinitialized to NULL.
  The library constructor must set gRT to point at the EFI Runtime Services Table so
  it is available at the module's entry point. Since there is overhead in initializing
  this global variable, only those modules that actually require access to the EFI
  Runtime Services Table should use this library.
  Only available to DXE and UEFI module types.

Copyright (c) 2006 - 2008, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __UEFI_RUNTIME_SERVICES_TABLE_LIB_H__
#define __UEFI_RUNTIME_SERVICES_TABLE_LIB_H__

///
/// Cached copy of the EFI Runtime Services Table
///
extern EFI_RUNTIME_SERVICES  *gRT;

#endif