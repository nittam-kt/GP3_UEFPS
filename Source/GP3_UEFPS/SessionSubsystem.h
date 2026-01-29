// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Online/OnlineSessionNames.h"
#include "OnlineSessionSettings.h"
#include "SessionSubsystem.generated.h"

/**
 * Online Subsystem の IOnlineSession を窓口に、Create/Find/Join を行う
 */
USTRUCT(BlueprintType)
struct FFoundSessionRow
{
    GENERATED_BODY();

    UPROPERTY(BlueprintReadOnly) FString DisplayName; // 部屋の表示名（オーナー名など）
    UPROPERTY(BlueprintReadOnly) int32   PingMs = 0;
    UPROPERTY(BlueprintReadOnly) int32   OpenConnections = 0;
    UPROPERTY(BlueprintReadOnly) int32   MaxConnections = 0;

    // 実際に Join するときに使う内部インデックス（UI から戻すだけ）
    int32 SearchIndex = INDEX_NONE;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCreateFinished, bool, bSuccess);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFindFinished, const TArray<FFoundSessionRow>&, Results);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnJoinFinished, bool, bSuccess);

UCLASS()
class GP3_UEFPS_API USessionSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, Category = "Session")
    void CreateLanSession(int32 PublicConnections = 3);

    UFUNCTION(BlueprintCallable, Category = "Session")
    void StartSession();

    UFUNCTION(BlueprintCallable, Category = "Session")
    void FindLanSessions(int32 MaxResults = 50);

    UFUNCTION(BlueprintCallable, Category = "Session")
    void JoinBySearchIndex(int32 SearchIndex);

    // UIと繋ぎやすいようにイベント化
    UPROPERTY(BlueprintAssignable) FOnCreateFinished OnCreateFinished;
    UPROPERTY(BlueprintAssignable) FOnFindFinished   OnFindFinished;
    UPROPERTY(BlueprintAssignable) FOnJoinFinished   OnJoinFinished;

private:
    // Online Subsystem 取得
    bool EnsureOnline();

    // デリゲートハンドル管理（解除忘れ防止）
    void ClearDelegates();

    // Create の前に既存セッションがあれば壊す
    void DestroyThenRecreate(int32 PublicConnections);

    // Online インタフェース
    IOnlineSubsystem* OSS = nullptr;
    IOnlineSessionPtr  Session;

    // 検索結果（UI表示用に整形）
    TSharedPtr<FOnlineSessionSearch> LastSearch;
    TArray<FFoundSessionRow>         LastRows;

    // 次に遷移するマップ名（Create 成功後に ?listen で開く）
    FString NextTravelMap;

    // デリゲート
    FDelegateHandle OnCreateHandle, OnStartHandle, OnDestroyHandle, OnFindHandle, OnJoinHandle;

    // コールバック
    void OnCreateComplete(FName SessionName, bool bOk);
    void OnStartComplete(FName SessionName, bool bOk);
    void OnDestroyComplete(FName SessionName, bool bOk);
    void OnFindComplete(bool bOk);
    void OnJoinComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
};
