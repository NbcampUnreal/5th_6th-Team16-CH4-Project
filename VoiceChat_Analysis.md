# Voice Chat (EOS) Analysis

## Where it is wired in

- EOS voice chat is enabled as project plugins and configured in the engine ini.

```json
// Tarcopy.uproject
"Plugins": [
  { "Name": "EOSVoiceChat", "Enabled": true },
  { "Name": "OnlineSubsystemEOS", "Enabled": true },
  { "Name": "EOSVoiceChatPTT", "Enabled": true }
]
```

```ini
; Config/DefaultEngine.ini
[OnlineSubsystem]
DefaultPlatformService=EOS

[EOSVoiceChat]
bEnabled=true
RTCBackgroundMode=KeepRoomsAlive

[/Script/EOSVoiceChatPTT.UEOSVoiceChatSubsystem]
bEnableVoiceLobby=true
VoiceLobbyIdPrefix=VOICE
VoiceLobbyBucketId=voice
VoiceLobbyMaxMembers=16
bEnableMicActivityLogs=true
MicActivityLogIntervalSeconds=0.1
MicActivityAmplitudeThreshold=50
bAlwaysTransmitByDefault=true

[Voice]
bEnabled=true
```

## Core implementation

The runtime behavior is implemented in the plugin subsystem `UEOSVoiceChatSubsystem` (a `UGameInstanceSubsystem`). It owns:
- Voice chat initialization and login against EOS
- Lobby RTC routing and fallback to normal EOS voice chat channels
- Push-to-talk (PTT) and always-transmit behavior
- Auto-join logic on a specific map

```cpp
// Plugins/EOSVoiceChatPTT/Source/EOSVoiceChatPTT/Public/UEOSVoiceChatSubsystem.h
UCLASS(Config=Engine)
class EOSVOICECHATPTT_API UEOSVoiceChatSubsystem : public UGameInstanceSubsystem
{
  ...
  UFUNCTION(BlueprintCallable, Category="EOS Voice Chat")
  void ToggleMute(bool bMute);

  UFUNCTION(BlueprintCallable, Category="EOS Voice Chat")
  void SetPTTEnabled(bool bEnabled);

  UFUNCTION(BlueprintCallable, Category="EOS Voice Chat")
  void SetAlwaysTransmit(bool bEnabled);

  UFUNCTION(BlueprintCallable, Category="EOS Voice Chat")
  void JoinChannel(const FString& ChannelName);

  UFUNCTION(BlueprintCallable, Category="EOS Voice Chat")
  void LeaveChannel();
  ...
};
```

## Initialization flow

When the game instance subsystem starts, it loads config, initializes voice chat, connects, and logs in. If an EOS Product User ID is not already available, it logs in via EOS Connect Device ID.

```cpp
// Plugins/EOSVoiceChatPTT/Source/EOSVoiceChatPTT/Private/UEOSVoiceChatSubsystem.cpp
void UEOSVoiceChatSubsystem::Initialize(...)
{
  LoadConfig();
  bPTTEnabled = bEnablePTTByDefault;
  bAlwaysTransmit = bAlwaysTransmitByDefault;
  InitializeVoiceChat();
  ...
}

void UEOSVoiceChatSubsystem::InitializeVoiceChat()
{
  VoiceChat = IVoiceChat::Get();
  if (!VoiceChat->IsInitialized())
  {
    VoiceChat->Initialize(...HandleVoiceChatInitialized...);
  }
  else
  {
    HandleVoiceChatInitialized(FVoiceChatResult::CreateSuccess());
  }
}

void UEOSVoiceChatSubsystem::HandleVoiceChatInitialized(...)
{
  EnsureVoiceChatUser();
  if (!VoiceChat->IsConnected())
  {
    VoiceChat->Connect(...HandleVoiceChatConnected...);
  }
  else
  {
    HandleVoiceChatConnected(...);
  }
}

void UEOSVoiceChatSubsystem::HandleVoiceChatConnected(...)
{
  EnsureConnectLogin();
}
```

## Auto-join and lobby RTC

Auto-join is triggered when a map named `Redwood` loads (default set in the subsystem constructor). When triggered, it tries to use lobby RTC first. If lobby RTC is not ready or not available, it falls back to a standard EOS voice chat channel join.

```cpp
// Plugins/EOSVoiceChatPTT/Source/EOSVoiceChatPTT/Private/UEOSVoiceChatSubsystem.cpp
AutoJoinMapName = TEXT("Redwood");

void UEOSVoiceChatSubsystem::TryAutoJoin()
{
  if (bEnableVoiceLobby && bPreferLobbyRtc)
  {
    if (ActiveLobbyId.IsEmpty())
    {
      EnsureVoiceLobby();
      return;
    }

    if (TryUseLobbyRtc(ActiveLobbyId))
    {
      bPendingAutoJoin = false;
      return;
    }
  }

  JoinChannel(TEXT(""));
}
```

The lobby ID is derived from the server address (CRC32 hash) and prefixed with `VOICE_` (configurable). If the lobby does not exist, it is created.

```cpp
FString UEOSVoiceChatSubsystem::BuildVoiceLobbyIdOverride() const
{
  const uint32 Hash = FCrc::StrCrc32(*ServerKey);
  const FString Base = FString::Printf(TEXT("%s_%08X"), *VoiceLobbyIdPrefix, Hash);
  return SanitizeChannelName(Base);
}

void UEOSVoiceChatSubsystem::StartVoiceLobbyJoinById(...)
{
  EOS_Lobby_JoinLobbyById(...EOS_LRRJAT_AutomaticJoin...);
}

void UEOSVoiceChatSubsystem::HandleVoiceLobbyJoinByIdComplete(...)
{
  if (Data->ResultCode == EOS_EResult::EOS_NotFound)
  {
    StartVoiceLobbyCreate(VoiceLobbyIdOverride);
  }
}
```

## Channel selection and fallback

`JoinChannel` picks a channel name (sanitized), then tries in this order:
1. Lobby RTC via active voice lobby
2. Lobby RTC via current session lobby
3. Manual RTC (if configured credentials exist)
4. Standard EOS VoiceChat channel join

```cpp
void UEOSVoiceChatSubsystem::JoinChannel(const FString& ChannelName)
{
  const FString Desired = ChannelName.IsEmpty() ? GetAutoChannelName() : ChannelName;
  const FString Sanitized = SanitizeChannelName(Desired);
  ...
  if (bEnableVoiceLobby && bPreferLobbyRtc)
  {
    if (ActiveLobbyId.IsEmpty()) { EnsureVoiceLobby(); ... }
    if (TryUseLobbyRtc(ActiveLobbyId)) { ... }
  }

  if (bPreferLobbyRtc)
  {
    const FString LobbyId = GetLobbyIdFromSession();
    if (!LobbyId.IsEmpty() && TryUseLobbyRtc(LobbyId)) { ... }
  }

  if (!ManualClientBaseUrl.IsEmpty() && !ManualParticipantToken.IsEmpty())
  {
    if (TryJoinManualRtc(Sanitized)) { ... }
  }

  // Fallback
  ActiveRoomName = Sanitized;
  ChannelMode = EEOSVoiceChannelMode::VoiceChat;
  VoiceChatUser->JoinChannel(Sanitized, TEXT(""), EVoiceChatChannelType::NonPositional, ...);
}
```

## Push-to-talk (PTT) and always transmit

PTT is bound to the `V` key by default, using Enhanced Input when available and a legacy input component as fallback. Always-transmit disables PTT gating entirely.

```cpp
bool UEOSVoiceChatSubsystem::BindEnhancedInput(APlayerController* PC)
{
  if (!PTTMappingContext)
  {
    PTTMappingContext = NewObject<UInputMappingContext>(this);
    PTTMappingContext->MapKey(PTTAction, EKeys::V);
  }
  ...
}

bool UEOSVoiceChatSubsystem::BindLegacyInput(APlayerController* PC)
{
  NewInputComponent->BindKey(EKeys::V, IE_Pressed, this, &UEOSVoiceChatSubsystem::OnPTTPressed);
  NewInputComponent->BindKey(EKeys::V, IE_Released, this, &UEOSVoiceChatSubsystem::OnPTTReleased);
}

void UEOSVoiceChatSubsystem::OnPTTPressed()
{
  if (bAlwaysTransmit || !bPTTEnabled || bMuted) { return; }
  bIsPTTActive = true;
  UpdateRTCSending(true);
}

void UEOSVoiceChatSubsystem::OnPTTReleased()
{
  if (bAlwaysTransmit) { return; }
  bIsPTTActive = false;
  UpdateRTCSending(false);
}
```

`UpdateRTCSending` selects how to send audio depending on the active channel mode:
- `VoiceChat`: `TransmitToSpecificChannels` with the active room
- `LobbyRtc`: `TransmitToAllChannels`
- `LobbyRtcSdk` / `ManualRtc`: EOS RTC audio update calls

```cpp
void UEOSVoiceChatSubsystem::UpdateRTCSending(bool bEnable)
{
  const bool bShouldSend = bAlwaysTransmit ? !bMuted : (bEnable && !bMuted);

  if (ChannelMode == EEOSVoiceChannelMode::VoiceChat)
  {
    if (!bShouldSend) { VoiceChatUser->TransmitToNoChannels(); return; }
    VoiceChatUser->TransmitToSpecificChannels({ ActiveRoomName });
    return;
  }

  if (ChannelMode == EEOSVoiceChannelMode::LobbyRtc)
  {
    if (!bShouldSend) { VoiceChatUser->TransmitToNoChannels(); return; }
    VoiceChatUser->TransmitToAllChannels();
    return;
  }

  ... EOS_RTCAudio_UpdateSending(...)
}
```

## Mic activity logs

RTC audio before-send notification is used to log microphone activity if enabled. It relies on config thresholds and intervals to avoid spamming logs.

```cpp
void UEOSVoiceChatSubsystem::RegisterRtcAudioNotify()
{
  if (!bEnableMicActivityLogs) { return; }
  EOS_RTCAudio_AddNotifyAudioBeforeSend(...);
}

void UEOSVoiceChatSubsystem::HandleRtcAudioBeforeSend(...)
{
  if (MaxAbs < MicActivityAmplitudeThreshold) { return; }
  if ((Now - LastMicLogTime) < MicActivityLogIntervalSeconds) { return; }
  UE_LOG(..., TEXT("Voice input detected..."));
}
```

## Summary of current behavior

- Voice chat is EOS-based and enabled globally via `DefaultEngine.ini`.
- The `UEOSVoiceChatSubsystem` auto-initializes, connects, and logs in to EOS voice on game start (client only).
- On the `Redwood` map, it auto-joins voice using a lobby RTC room keyed to the server address; if that fails, it falls back to a normal EOS voice chat channel.
- PTT is bound to the `V` key, but `bAlwaysTransmitByDefault=true` means the default behavior is open mic unless changed at runtime.

## Key files

- `Tarcopy.uproject`
- `Config/DefaultEngine.ini`
- `Plugins/EOSVoiceChatPTT/Source/EOSVoiceChatPTT/Public/UEOSVoiceChatSubsystem.h`
- `Plugins/EOSVoiceChatPTT/Source/EOSVoiceChatPTT/Private/UEOSVoiceChatSubsystem.cpp`
