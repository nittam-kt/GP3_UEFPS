// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GP3_UEFPSGameMode.h"
#include "LobbyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class GP3_UEFPS_API ALobbyGameMode : public AGP3_UEFPSGameMode
{
	GENERATED_BODY()

	virtual void BeginPlay() override;
	virtual FString InitNewPlayer(APlayerController* NewPlayer, const FUniqueNetIdRepl& UniqueId,
		const FString& Options, const FString& Portal) override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;
	void TryStartIfReady();

public:
	static constexpr int MaxPlayers = 2;
};
