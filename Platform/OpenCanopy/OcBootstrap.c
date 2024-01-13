/** @file
  This file is part of OpenCanopy, OpenCore GUI.

  Copyright (c) 2018-2019, Download-Fritz. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-3-Clause
**/

#include <Library/OcMainLib.h>
#include <Uefi.h>

#include <Protocol/DevicePath.h>
#include <Protocol/LoadedImage.h>
#include <Protocol/OcBootstrap.h>
#include <Protocol/OcInterface.h>

#include <Library/UefiLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/OcBootManagementLib.h>
#include <Library/OcConsoleLib.h>
#include <Library/OcDebugLogLib.h>
#include <Library/DevicePathLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/OcDevicePathLib.h>
#include <Library/OcFileLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiApplicationEntryPoint.h>

#include "OpenCanopy.h"
#include "BmfLib.h"
#include "GuiApp.h"

//
// Add slight x offset of cursor by its position within its icon file,
// in order to match look of Apple picker initial position.
//
#define DEFAULT_CURSOR_OFFSET_X  BOOT_CURSOR_OFFSET
#define DEFAULT_CURSOR_OFFSET_Y  112U

extern BOOT_PICKER_GUI_CONTEXT  mGuiContext;

STATIC GUI_DRAWING_CONTEXT              mDrawContext;
STATIC EFI_CONSOLE_CONTROL_SCREEN_MODE  mPreviousMode;

STATIC
EFI_STATUS
OcShowMenuByOcEnter (
  IN BOOT_PICKER_GUI_CONTEXT  *GuiContext
  )
{
  EFI_STATUS  Status;

  Status = GuiLibConstruct (
             GuiContext,
             mGuiContext.CursorOffsetX,
             mGuiContext.CursorOffsetY
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  mPreviousMode = OcConsoleControlSetMode (EfiConsoleControlScreenGraphics);

  return EFI_SUCCESS;
}

STATIC
VOID
OcShowMenuByOcLeave (
  VOID
  )
{
  GuiLibDestruct ();
  OcConsoleControlSetMode (mPreviousMode);
}

STATIC
VOID
OcSetInitialCursorOffset (
  VOID
  )
{
  //
  // The cursor position is updated on GUI exit, so don't overwrite it.
  //
  if (mGuiContext.PickerContext == NULL) {
    mGuiContext.CursorOffsetX = DEFAULT_CURSOR_OFFSET_X * mGuiContext.Scale;
    mGuiContext.CursorOffsetY = DEFAULT_CURSOR_OFFSET_Y * mGuiContext.Scale;
  }
}

EFI_STATUS
EFIAPI
OcShowMenuByOc (
  IN     OC_BOOT_CONTEXT  *BootContext,
  IN     OC_BOOT_ENTRY    **BootEntries,
  OUT    OC_BOOT_ENTRY    **ChosenBootEntry
  )
{
  EFI_STATUS  Status;
  UINTN       Index;

  *ChosenBootEntry = NULL;
  OcSetInitialCursorOffset ();
  mGuiContext.BootEntry   = NULL;
  mGuiContext.ReadyToBoot = FALSE;

  //
  // When enabled, re-run intro animation on each entry into menu, to avoid
  // stuck animation which happens otherwise, if a menu item which returns to
  // the menu is selected before the animation ends.
  // Do not play intro animation for visually impaired users.
  //
  mGuiContext.UseMenuEaseIn = !BootContext->PickerContext->PickerAudioAssist
                              && ((BootContext->PickerContext->PickerAttributes & OC_ATTR_REDUCE_MOTION) == 0);

  mGuiContext.HideAuxiliary        = BootContext->PickerContext->HideAuxiliary;
  mGuiContext.Refresh              = FALSE;
  mGuiContext.PickerContext        = BootContext->PickerContext;
  mGuiContext.AudioPlaybackTimeout = -1;

  Status = OcShowMenuByOcEnter (&mGuiContext);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  mDrawContext.TimeOutSeconds = BootContext->PickerContext->TimeoutSeconds;

  Status = BootPickerViewInitialize (
             &mDrawContext,
             &mGuiContext,
             InternalGetCursorImage,
             (UINT8)BootContext->BootEntryCount
             );
  if (EFI_ERROR (Status)) {
    OcShowMenuByOcLeave ();
    return Status;
  }

  for (Index = 0; Index < BootContext->BootEntryCount; ++Index) {
    Status = BootPickerEntriesSet (
               BootContext->PickerContext,
               &mGuiContext,
               BootEntries[Index],
               (UINT8)Index
               );
    if (EFI_ERROR (Status)) {
      OcShowMenuByOcLeave ();
      return Status;
    }
  }

  BootPickerViewLateInitialize (
    &mDrawContext,
    &mGuiContext,
    (UINT8)BootContext->DefaultEntry->EntryIndex - 1
    );

  GuiRedrawAndFlushScreen (&mDrawContext);

  if (BootContext->PickerContext->PickerAudioAssist) {
    BootContext->PickerContext->PlayAudioFile (
                                  BootContext->PickerContext,
                                  OC_VOICE_OVER_AUDIO_FILE_CHOOSE_OS,
                                  OC_VOICE_OVER_AUDIO_BASE_TYPE_OPEN_CORE,
                                  FALSE
                                  );
    for (Index = 0; Index < BootContext->BootEntryCount; ++Index) {
      BootContext->PickerContext->PlayAudioEntry (
                                    BootContext->PickerContext,
                                    BootEntries[Index]
                                    );
      if ((BootContext->PickerContext->TimeoutSeconds > 0) && (BootContext->DefaultEntry->EntryIndex - 1 == Index)) {
        BootContext->PickerContext->PlayAudioFile (
                                      BootContext->PickerContext,
                                      OC_VOICE_OVER_AUDIO_FILE_DEFAULT,
                                      OC_VOICE_OVER_AUDIO_BASE_TYPE_OPEN_CORE,
                                      FALSE
                                      );
      }
    }

    BootContext->PickerContext->PlayAudioBeep (
                                  BootContext->PickerContext,
                                  OC_VOICE_OVER_SIGNALS_NORMAL,
                                  OC_VOICE_OVER_SIGNAL_NORMAL_MS,
                                  OC_VOICE_OVER_SILENCE_NORMAL_MS
                                  );
  }

  GuiDrawLoop (&mDrawContext);
  ASSERT (mGuiContext.BootEntry != NULL || mGuiContext.Refresh);

  //
  // There are reasons to prefer not to perform this screen clear:
  //  - If starting macOS, boot.efi performs the same screen clear to the same UI theme colour
  //    immediately afterwards anyway
  //  - The native Apple picker does not clear its graphics before exit (i.e. does not do this)
  //  - Most OS booters in most circumstance perform their own screen clear
  //  - Each screen clear on a slow GOP (such as direct GOP rendering) is a noticeable slowdown
  // However:
  //  - Windows without ACPI->Quirks->ResetLogoStatus does not clear any pre-existing graphics
  //    - Ref: https://github.com/acidanthera/bugtracker/issues/2231
  //  - Peforming this screen clear gives a sense of progress (i.e. something happens immediately
  //    rather than nothing) if the selected entry will be very slow to start (e.g. Recovery, in
  ///   some circumstances)
  //
  if (!mGuiContext.Refresh) {
    //
    // Clear the screen only when we exit.
    //
    GuiClearScreen (&mDrawContext, &mGuiContext.BackgroundColor.Pixel);
  }

  //
  // Note, it is important to destruct GUI here, as we must ensure
  // that keyboard/mouse polling does not conflict with FV2 ui.
  //
  BootPickerViewDeinitialize (&mDrawContext, &mGuiContext);
  OcShowMenuByOcLeave ();

  *ChosenBootEntry                          = mGuiContext.BootEntry;
  BootContext->PickerContext->HideAuxiliary = mGuiContext.HideAuxiliary;
  if (mGuiContext.Refresh) {
    return EFI_ABORTED;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
OcShowPasswordByOc (
  IN OC_PICKER_CONTEXT   *Context,
  IN OC_PRIVILEGE_LEVEL  Level
  )
{
  EFI_STATUS  Status;

  OcSetInitialCursorOffset ();
  mGuiContext.BootEntry            = NULL;
  mGuiContext.ReadyToBoot          = FALSE;
  mGuiContext.HideAuxiliary        = TRUE;
  mGuiContext.Refresh              = FALSE;
  mGuiContext.PickerContext        = Context;
  mGuiContext.AudioPlaybackTimeout = -1;

  Status = OcShowMenuByOcEnter (&mGuiContext);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  mDrawContext.TimeOutSeconds = 0;

  mGuiContext.UsePasswordEaseIn = !Context->PickerAudioAssist
                                  && ((Context->PickerAttributes & OC_ATTR_REDUCE_MOTION) == 0);

  Status = PasswordViewInitialize (
             &mDrawContext,
             &mGuiContext
             );
  if (EFI_ERROR (Status)) {
    OcShowMenuByOcLeave ();
    return Status;
  }

  GuiRedrawAndFlushScreen (&mDrawContext);

  GuiDrawLoop (&mDrawContext);

  //
  // This screen clear is useful, even though the boot entry started will in general
  // perform its own screen clear eventually anyway, since this gives a better sense
  // of progress between (intentionally slow - computationally intensive) password
  // verification and (can be slow) start of Recovery.
  //
  if (Context->PickerCommand != OcPickerShowPicker) {
    //
    // Clear the screen only if we will not show BootPicker afterwards.
    //
    GuiClearScreen (&mDrawContext, &mGuiContext.BackgroundColor.Pixel);
  }

  //
  // Note, it is important to destruct GUI here, as we must ensure
  // that keyboard/mouse polling does not conflict with FV2 ui.
  //
  PasswordViewDeinitialize (&mDrawContext, &mGuiContext);
  OcShowMenuByOcLeave ();

  return EFI_SUCCESS;
}

EFI_STATUS
InternalContextConstruct (
  OUT BOOT_PICKER_GUI_CONTEXT  *Context,
  IN  OC_STORAGE_CONTEXT       *Storage,
  IN  OC_PICKER_CONTEXT        *Picker
  );

STATIC
EFI_STATUS
EFIAPI
GuiOcInterfacePopulate (
  IN OC_INTERFACE_PROTOCOL  *This,
  IN OC_STORAGE_CONTEXT     *Storage,
  IN OC_PICKER_CONTEXT      *Context
  )
{
  EFI_STATUS  Status;

  Status = InternalContextConstruct (&mGuiContext, Storage, Context);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Context->ShowMenu         = OcShowMenuByOc;
  Context->RequestPrivilege = OcShowPasswordByOc;

  return EFI_SUCCESS;
}

STATIC OC_INTERFACE_PROTOCOL  mOcInterface = {
  OC_INTERFACE_REVISION,
  GuiOcInterfacePopulate
};

EFI_STATUS
EFIAPI
UefiMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;
  VOID        *PrevInterface;
  EFI_HANDLE  NewHandle;

  //
  // Check for previous GUI protocols.
  //
  Status = gBS->LocateProtocol (
                  &gOcInterfaceProtocolGuid,
                  NULL,
                  &PrevInterface
                  );

  if (!EFI_ERROR (Status)) {
    DEBUG ((DEBUG_WARN, "OCUI: Another GUI is already present\n"));
    return EFI_ALREADY_STARTED;
  }

  //
  // Install new GUI protocol
  //
  NewHandle = NULL;
  Status    = gBS->InstallMultipleProtocolInterfaces (
                     &NewHandle,
                     &gOcInterfaceProtocolGuid,
                     &mOcInterface,
                     NULL
                     );

  if (!EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "OCUI: Registered custom GUI protocol\n"));
  } else {
    DEBUG ((DEBUG_WARN, "OCUI: Failed to install GUI protocol - %r\n", Status));
  }

  return Status;
}
