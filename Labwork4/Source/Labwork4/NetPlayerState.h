// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "NetGameInstance.h"
#include "NetPlayerState.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class LABWORK4_API ANetPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	ANetPlayerState();

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_PlayerInfo)
	FSPlayerInfo Data;

	UPROPERTY(BlueprintReadOnly, Replicated)
	int PlayerIndex;

	UPROPERTY(BlueprintReadWrite)
	EPlayerTeam TeamID;

	UPROPERTY(BlueprintReadWrite)
	EGameResults Result;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UFUNCTION()
	void OnRep_PlayerInfo();
};
