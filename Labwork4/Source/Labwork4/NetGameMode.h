// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "NetGameInstance.h"
#include "NetGameMode.generated.h"

class ANetAvatar;
class APlayerController;

/**
 * 
 */
UCLASS()
class LABWORK4_API ANetGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ANetGameMode();

	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintCallable)
	void AvatarsOverlapped(ANetAvatar* AvatarA, ANetAvatar* AvatarB);

	UFUNCTION(BlueprintCallable)
	void EndGame();

	void OnGameTimerExpired();
	void UpdateRemainingTime();

private:
	int TotalPlayerCount;
	int TotalGames;
	int PlayerStartIndex;
	int CatcherPlayerIndex;
	int LastBluePlayerIndex;

	FTimerHandle GameTimerHandle;
	FTimerHandle CountdownTimerHandle;

	UPROPERTY()
	TArray<APlayerController*> AllPlayers;

	AActor* GetPlayerStart(FString Name, int Index);
	AActor* AssignTeamAndPlayerStart(AController* Player);
};
