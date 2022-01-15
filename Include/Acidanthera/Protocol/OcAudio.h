/** @file
  Copyright (C) 2020, vit9696. All rights reserved.

  All rights reserved.

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
**/

#ifndef OC_AUDIO_PROTOCOL_H
#define OC_AUDIO_PROTOCOL_H

#include <Protocol/AudioIo.h>
#include <Protocol/AppleVoiceOver.h>
#include <Protocol/DevicePath.h>

#define OC_AUDIO_PROTOCOL_REVISION  0x050000

//
// OC_AUDIO_PROTOCOL_GUID
// 4B228577-6274-4A48-82AE-0713A1171987
//
#define OC_AUDIO_PROTOCOL_GUID  \
  { 0x4B228577, 0x6274, 0x4A48, \
    { 0x82, 0xAE, 0x07, 0x13, 0xA1, 0x17, 0x19, 0x87 } }

typedef struct OC_AUDIO_PROTOCOL_ OC_AUDIO_PROTOCOL;

/**
  Custom OpenCore audio files.
**/
typedef enum {
  OcVoiceOverAudioFileBase                  = 0x1000,

  OcVoiceOverAudioFileIndexBase             = 0x1000,
  OcVoiceOverAudioFile1                     = 0x1001,
  OcVoiceOverAudioFile2                     = 0x1002,
  OcVoiceOverAudioFile3                     = 0x1003,
  OcVoiceOverAudioFile4                     = 0x1004,
  OcVoiceOverAudioFile5                     = 0x1005,
  OcVoiceOverAudioFile6                     = 0x1006,
  OcVoiceOverAudioFile7                     = 0x1007,
  OcVoiceOverAudioFile8                     = 0x1008,
  OcVoiceOverAudioFile9                     = 0x1009,
  OcVoiceOverAudioFileIndexAlphabetical     = 0x100A,
  OcVoiceOverAudioFileLetterA               = 0x100A,
  OcVoiceOverAudioFileLetterB               = 0x100B,
  OcVoiceOverAudioFileLetterC               = 0x100C,
  OcVoiceOverAudioFileLetterD               = 0x100D,
  OcVoiceOverAudioFileLetterE               = 0x100E,
  OcVoiceOverAudioFileLetterF               = 0x100F,
  OcVoiceOverAudioFileLetterG               = 0x1010,
  OcVoiceOverAudioFileLetterH               = 0x1011,
  OcVoiceOverAudioFileLetterI               = 0x1012,
  OcVoiceOverAudioFileLetterJ               = 0x1013,
  OcVoiceOverAudioFileLetterK               = 0x1014,
  OcVoiceOverAudioFileLetterL               = 0x1015,
  OcVoiceOverAudioFileLetterM               = 0x1016,
  OcVoiceOverAudioFileLetterN               = 0x1017,
  OcVoiceOverAudioFileLetterO               = 0x1018,
  OcVoiceOverAudioFileLetterP               = 0x1019,
  OcVoiceOverAudioFileLetterQ               = 0x101A,
  OcVoiceOverAudioFileLetterR               = 0x101B,
  OcVoiceOverAudioFileLetterS               = 0x101C,
  OcVoiceOverAudioFileLetterT               = 0x101D,
  OcVoiceOverAudioFileLetterU               = 0x101E,
  OcVoiceOverAudioFileLetterV               = 0x101F,
  OcVoiceOverAudioFileLetterW               = 0x1020,
  OcVoiceOverAudioFileLetterX               = 0x1021,
  OcVoiceOverAudioFileLetterY               = 0x1022,
  OcVoiceOverAudioFileLetterZ               = 0x1023,
  OcVoiceOverAudioFileIndexMax              = 0x1023,

  OcVoiceOverAudioFileAbortTimeout          = 0x1030,
  OcVoiceOverAudioFileChooseOS              = 0x1031,
  OcVoiceOverAudioFileDefault               = 0x1032,
  OcVoiceOverAudioFileDiskImage             = 0x1033,
  OcVoiceOverAudioFileEnterPassword         = 0x1034,
  OcVoiceOverAudioFileExecutionFailure      = 0x1035,
  OcVoiceOverAudioFileExecutionSuccessful   = 0x1036,
  OcVoiceOverAudioFileExternal              = 0x1037,
  OcVoiceOverAudioFileExternalOS            = 0x1038,
  OcVoiceOverAudioFileExternalTool          = 0x1039,
  OcVoiceOverAudioFileLoading               = 0x103A,
  OcVoiceOverAudioFilemacOS                 = 0x103B,
  OcVoiceOverAudioFilemacOS_Recovery        = 0x103C,
  OcVoiceOverAudioFilemacOS_TimeMachine     = 0x103D,
  OcVoiceOverAudioFilemacOS_UpdateFw        = 0x103E,
  OcVoiceOverAudioFileOtherOS               = 0x103F,
  OcVoiceOverAudioFilePasswordAccepted      = 0x1040,
  OcVoiceOverAudioFilePasswordIncorrect     = 0x1041,
  OcVoiceOverAudioFilePasswordRetryLimit    = 0x1042,
  OcVoiceOverAudioFileReloading             = 0x1043,
  OcVoiceOverAudioFileResetNVRAM            = 0x1044,
  OcVoiceOverAudioFileRestart               = 0x1045,
  OcVoiceOverAudioFileSelected              = 0x1046,
  OcVoiceOverAudioFileShowAuxiliary         = 0x1047,
  OcVoiceOverAudioFileShutDown              = 0x1048,
  OcVoiceOverAudioFileSIPIsDisabled         = 0x1049,
  OcVoiceOverAudioFileSIPIsEnabled          = 0x104A,
  OcVoiceOverAudioFileTimeout               = 0x104B,
  OcVoiceOverAudioFileUEFI_Shell            = 0x104C,
  OcVoiceOverAudioFileWelcome               = 0x104D,
  OcVoiceOverAudioFileWindows               = 0x104E,

  OcVoiceOverAudioFileMax                   = 0x104F,
} OC_VOICE_OVER_AUDIO_FILE;

STATIC_ASSERT (OcVoiceOverAudioFileIndexMax - OcVoiceOverAudioFileIndexBase == 9 + 26, "Invalid index count");

/**
  Connect to Audio I/O.

  @param[in,out] This             Audio protocol instance.
  @param[in]     DevicePath       Controller device path, optional.
  @param[in]     CodecAddress     Codec address, optional.
  @param[in]     OutputIndexMask  Output index mask.

  @retval EFI_SUCESS on success.
  @retval EFI_NOT_FOUND when missing.
  @retval EFI_UNSUPPORTED on failure.
**/
typedef
EFI_STATUS
(EFIAPI* OC_AUDIO_CONNECT) (
  IN OUT OC_AUDIO_PROTOCOL         *This,
  IN     EFI_DEVICE_PATH_PROTOCOL  *DevicePath      OPTIONAL,
  IN     UINT8                     CodecAddress     OPTIONAL,
  IN     UINT64                    OutputIndexMask
  );

/**
  Set Audio I/O default gain.

  @param[in,out] This             Audio protocol instance.
  @param[in]     Gain             The amplifier gain (or attenuation if negative) in dB to use, relative to 0 dB level (0 dB
                                  is usually at at or near max available volume, but is not required to be so in the spec).

  @retval EFI_SUCESS on success.
**/
typedef
EFI_STATUS
(EFIAPI* OC_AUDIO_SET_DEFAULT_GAIN) (
  IN OUT OC_AUDIO_PROTOCOL         *This,
  IN     INT8                      Gain
  );

/**
  Retrieve file contents callback.

  @param[in,out]  Context      Externally specified context.
  @param[in]      File         File identifier, see APPLE_VOICE_OVER_AUDIO_FILE.
  @param[in]      LanguageCode Language code for the file.
  @param[out]     Buffer       Pointer to buffer.
  @param[out]     BufferSize   Pointer to buffer size.
  @param[out]     Frequency    Decoded PCM frequency.
  @param[out]     Bits         Decoded bit count.
  @param[out]     Channels     Decoded amount of channels.

  @retval EFI_SUCCESS on successful file lookup.
**/
typedef
EFI_STATUS
(EFIAPI* OC_AUDIO_PROVIDER_ACQUIRE) (
  IN  VOID                            *Context,
  IN  UINT32                          File,
  IN  APPLE_VOICE_OVER_LANGUAGE_CODE  LanguageCode,
  OUT UINT8                           **Buffer,
  OUT UINT32                          *BufferSize,
  OUT EFI_AUDIO_IO_PROTOCOL_FREQ      *Frequency,
  OUT EFI_AUDIO_IO_PROTOCOL_BITS      *Bits,
  OUT UINT8                           *Channels
  );

/**
  Release file contents given by acquire callback.

  @param[in,out]  Context      Externally specified context.
  @param[out]     Buffer       Pointer to buffer.

  @retval EFI_SUCCESS on successful release.
**/
typedef
EFI_STATUS
(EFIAPI* OC_AUDIO_PROVIDER_RELEASE) (
  IN  VOID                            *Context,
  IN  UINT8                           *Buffer
  );

/**
  Set resource provider.

  @param[in,out] This         Audio protocol instance.
  @param[in]     Acquire      Resource acquire handler.
  @param[in]     Release      Resource release handler, optional.
  @param[in]     Context      Resource handler context.

  @retval EFI_SUCCESS on successful provider update.
**/
typedef
EFI_STATUS
(EFIAPI* OC_AUDIO_SET_PROVIDER) (
  IN OUT OC_AUDIO_PROTOCOL          *This,
  IN     OC_AUDIO_PROVIDER_ACQUIRE  Acquire,
  IN     OC_AUDIO_PROVIDER_RELEASE  Release  OPTIONAL,
  IN     VOID                       *Context
  );

/**
  Convert raw amplifier gain setting to decibel gain value; converts using the parameters of the first
  channel specified for sound on the current codec which has non-zero amp capabilities.

  @param[in,out] This         Audio protocol instance.
  @param[in]     GainParam    Raw codec gain param.
  @param[out]    Gain         The amplifier gain (or attenuation if negative) in dB to use, relative to 0 dB level.

  @retval EFI_SUCCESS on successful conversion.
**/
typedef
EFI_STATUS
(EFIAPI* OC_AUDIO_RAW_GAIN_TO_DECIBELS) (
  IN OUT OC_AUDIO_PROTOCOL          *This,
  IN     UINT8                      GainParam,
     OUT INT8                       *Gain
  );

/**
  Play file.

  @param[in,out] This         Audio protocol instance.
  @param[in]     File         File to play.
  @param[in]     Gain         The amplifier gain (or attenuation if negative) in dB to use, relative to 0 dB level.
  @param[in]     UseGain      If TRUE use provided volume level, otherwise use stored global volume level.
  @param[in]     Wait         Wait for completion of the previous track.

  @retval EFI_SUCCESS on successful playback startup.
**/
typedef
EFI_STATUS
(EFIAPI* OC_AUDIO_PLAY_FILE) (
  IN OUT OC_AUDIO_PROTOCOL          *This,
  IN     UINT32                     File,
  IN     INT8                       Gain  OPTIONAL,
  IN     BOOLEAN                    UseGain,
  IN     BOOLEAN                    Wait
  );

/**
  Stop playback.

  @param[in,out] This         Audio protocol instance.
  @param[in]     Wait         Wait for audio completion.

  @retval EFI_SUCCESS on successful playback stop.
**/
typedef
EFI_STATUS
(EFIAPI* OC_AUDIO_STOP_PLAYBACK) (
  IN OUT OC_AUDIO_PROTOCOL          *This,
  IN     BOOLEAN                    Wait
  );

/**
  Set playback delay.

  @param[in,out] This         Audio protocol instance.
  @param[in]     Delay        Delay after audio configuration in microseconds.

  @return previous delay, defaults to 0.
**/
typedef
UINTN
(EFIAPI* OC_AUDIO_SET_DELAY) (
  IN OUT OC_AUDIO_PROTOCOL          *This,
  IN     UINTN                      Delay
  );

//
// Includes a revision for debugging reasons.
//
struct OC_AUDIO_PROTOCOL_ {
  UINTN                         Revision;
  OC_AUDIO_CONNECT              Connect;
  OC_AUDIO_RAW_GAIN_TO_DECIBELS RawGainToDecibels;
  OC_AUDIO_SET_DEFAULT_GAIN     SetDefaultGain;
  OC_AUDIO_SET_PROVIDER         SetProvider;
  OC_AUDIO_PLAY_FILE            PlayFile;
  OC_AUDIO_STOP_PLAYBACK        StopPlayback;
  OC_AUDIO_SET_DELAY            SetDelay;
};

extern EFI_GUID gOcAudioProtocolGuid;

#endif // OC_AUDIO_PROTOCOL_H
