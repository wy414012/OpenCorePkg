/** @file
  This library implements audio interaction.

  Copyright (c) 2020, vit9696. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause
**/

#ifndef OC_AUDIO_LIB_H
#define OC_AUDIO_LIB_H

#include <Uefi.h>
#include <Library/OcFileLib.h>
#include <Protocol/OcAudio.h>

#define OC_AUDIO_DEFAULT_VOLUME_LEVEL  70

/**
  Install audio support protocols.

  @param[in] Reinstall           Overwrite installed protocols.
  @param[in] DisconnectHda       Attempt to disconnect HDA controller first.

  @retval installed protocol.
  @retval NULL when conflicting audio implementation is present.
  @retval NULL when installation failed.
**/
OC_AUDIO_PROTOCOL *
OcAudioInstallProtocols (
  IN BOOLEAN  Reinstall,
  IN BOOLEAN  DisconnectHda
  );

/**
  Convert language code to ASCII string.

  @param[in]  LanguageCode   Code.

  @retval ASCII string.
**/
CONST CHAR8 *
OcLanguageCodeToString (
  IN APPLE_VOICE_OVER_LANGUAGE_CODE  LanguageCode
  );

/**
  Get system volume level in 0~100 range.

  @param[in]   Amplifier  Amplification coefficient 1~999.
  @param[out]  Muted      Whether volume is off.

  @retval ASCII string.
**/
UINT8
OcGetVolumeLevel (
  IN  UINT32   Amplifier,
  OUT BOOLEAN  *Muted
  );

/**
  Set VoiceOver language from string.

  @param[in]  Language   Language string, optional for system.

  @retval EFI_SUCCESS on success.
**/
EFI_STATUS
OcSetVoiceOverLanguage (
  CONST CHAR8   *Language  OPTIONAL
  );

/**
  Dump audio data to the specified directory.

  @param[in] Root  Directory to write audio data.

  @retval EFI_SUCCESS on success.
**/
EFI_STATUS
OcAudioDump (
  IN EFI_FILE_PROTOCOL  *Root
  );

#endif // OC_AUDIO_LIB_H
