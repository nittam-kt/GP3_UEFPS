// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"

#include "Kismet/GameplayStatics.h"
#include "SessionSubsystem.h"
#include "OnlineSubsystem.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/GameStateBase.h"
#include "GenericPlatform/GenericPlatformHttp.h"
#include "Misc/CommandLine.h"
#include "GP3PlayerState.h"


void ALobbyGameMode::BeginPlay()
{
    Super::BeginPlay();

    if(!HasAuthority()) return; // サーバのみ

    // URLオプションを読む（HostSession=1 なら起動直後にセッション作成）
    const FString HostSessionOpt = UGameplayStatics::ParseOption(
        GetWorld()->GetAuthGameMode()->OptionsString,
        TEXT("HostSession")
    );

    const bool bShouldHost = (HostSessionOpt == TEXT("1") || HostSessionOpt.Equals(TEXT("true"), ESearchCase::IgnoreCase));

    if(bShouldHost)
    {
        UE_LOG(LogTemp, Log, TEXT("HostSession option detected. Starting CreateSession flow..."));
        if(auto* Sub = GetGameInstance()->GetSubsystem<USessionSubsystem>())
        {
            Sub->StartSession();
        }
    }
}

FString ALobbyGameMode::InitNewPlayer(APlayerController* NewPlayer, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal)
{
    FString result = Super::InitNewPlayer(NewPlayer, UniqueId, Options, Portal);


    // Options から NickName= を取得（URL に付けたやつがここに来る）
    const FString Key = TEXT("NickName=");
    const FString& OptionsStr = Options;
    FString RawName;
    int32 StartPos = OptionsStr.Find(Key, ESearchCase::IgnoreCase, ESearchDir::FromEnd);
    if (StartPos != INDEX_NONE)
    {
        StartPos += Key.Len();
        // '&' または終端までを切り出す
        int32 EndPos = OptionsStr.Find(TEXT("?"), ESearchCase::IgnoreCase, ESearchDir::FromStart, StartPos);
        if (EndPos == INDEX_NONE)
        {
            RawName = OptionsStr.Mid(StartPos);
        }
        else
        {
            RawName = OptionsStr.Mid(StartPos, EndPos - StartPos);
        }
    }

    // クライアント側で URL エンコードしていた場合はデコード
    if (!RawName.IsEmpty())
    {
        FString DecodedName = FGenericPlatformHttp::UrlDecode(RawName);
        if (APlayerState* PS = NewPlayer->GetPlayerState<APlayerState>())
        {
            PS->SetPlayerName(DecodedName);
            auto str = FString::Printf(TEXT("Join: %s"), *DecodedName);
            UKismetSystemLibrary::PrintString(this, str,
                true, true, FColor::Green, 6.f, TEXT("None"));
        }
    }
    return result;
}

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);

    // サーバー以外では処理しない
    if (!HasAuthority()) return;

    // スタンドアロン（ネットワークなし）なら無視
    if (GetNetMode() == NM_Standalone) return;
    // セッションが立っている（Listen化も済み）ときだけ扱う

    IOnlineSubsystem* OSS = IOnlineSubsystem::Get();
    if (OSS)
    {
        IOnlineSessionPtr Session = OSS->GetSessionInterface();
        if (!Session.IsValid() || !Session->GetNamedSession(NAME_GameSession))
        {
            // まだ CreateSession 前/直後など → 待機
            return;
        }
    }

    TryStartIfReady();
}

void ALobbyGameMode::Logout(AController* Exiting)
{
    Super::Logout(Exiting);
    auto str = FString::Printf(TEXT("Players left: %s"), *GetNameSafe(Exiting));
    UKismetSystemLibrary::PrintString(this, str, false, true, FColor::Red, 6.f, TEXT("None"));
}


void ALobbyGameMode::TryStartIfReady()
{
    const int32 NumConnected = GameState ? GameState->PlayerArray.Num() : 0;
    auto str = FString::Printf(TEXT("Players: %d"), NumConnected);
    UKismetSystemLibrary::PrintString(this, str, true, true, FColor::Green, 6.f, TEXT("None"));

    if (NumConnected >= MaxPlayers)
    {
        // 直後の同期ズレ回避で次フレームに遷移
        FTimerHandle TH;
        GetWorldTimerManager().SetTimer(TH, [this]()
            {
                GetWorld()->ServerTravel(TEXT("/FpsFramework/Maps/FpsGameMap?listen"), /*bAbsolute=*/true);
            }, 0.1f, false);
    }
}
