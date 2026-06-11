// Fill out your copyright notice in the Description page of Project Settings.

#include "NetGameMode.h"
#include "NetBaseCharacter.h"
#include "NetGameState.h"
#include "NetPlayerState.h"
#include "NetAvatar.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerStart.h"
#include "Components/CapsuleComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"

ANetGameMode::ANetGameMode()
{
	DefaultPawnClass = ANetBaseCharacter::StaticClass();
	PlayerStateClass = ANetPlayerState::StaticClass();
	GameStateClass = ANetGameState::StaticClass();

	TotalPlayerCount = 0;
	TotalGames = 0;
	PlayerStartIndex = 0;
}

AActor* ANetGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	AActor* Start = AssignTeamAndPlayerStart(Player);
	return Start ? Start : Super::ChoosePlayerStart_Implementation(Player);
}

AActor* ANetGameMode::AssignTeamAndPlayerStart(AController* Player)
{
	AActor* Start = nullptr;
	ANetPlayerState* State = Player->GetPlayerState<ANetPlayerState>();
	if (State)
	{
		if (TotalGames == 0)
		{
			State->TeamID = TotalPlayerCount == 0 ? EPlayerTeam::TEAM_Blue : EPlayerTeam::TEAM_Red;
			State->PlayerIndex = TotalPlayerCount++;
			AllPlayers.Add(Cast<APlayerController>(Player));
		}
		else
		{
			State->TeamID = State->Result == EGameResults::RESULT_Won ? EPlayerTeam::TEAM_Blue : EPlayerTeam::TEAM_Red;
		}

		if (State->TeamID == EPlayerTeam::TEAM_Blue)
		{
			Start = GetPlayerStart("Blue", -1);
		}
		else
		{
			Start = GetPlayerStart("Red", PlayerStartIndex++);
		}
	}
	return Start;
}

AActor* ANetGameMode::GetPlayerStart(FString Name, int Index)
{
	FName PSName;
	if (Index < 0)
	{
		PSName = *Name;
	}
	else
	{
		PSName = *FString::Printf(TEXT("%s%d"), *Name, Index % 4);
	}

	for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It)
	{
		APlayerStart* PS = *It;
		if (PS && PS->PlayerStartTag == PSName)
		{
			return PS;
		}
	}
	return nullptr;
}

void ANetGameMode::AvatarsOverlapped(ANetAvatar* AvatarA, ANetAvatar* AvatarB)
{
	ANetGameState* GState = GetGameState<ANetGameState>();
	if (GState == nullptr || GState->WinningPlayer >= 0) return;

	ANetPlayerState* StateA = AvatarA->GetPlayerState<ANetPlayerState>();
	ANetPlayerState* StateB = AvatarB->GetPlayerState<ANetPlayerState>();
	if (StateA == nullptr || StateB == nullptr) return;
	if (StateA->TeamID == StateB->TeamID) return;

	if (StateA->TeamID == EPlayerTeam::TEAM_Blue)
	{
		GState->WinningPlayer = StateB->PlayerIndex;
	}
	else
	{
		GState->WinningPlayer = StateA->PlayerIndex;
	}

	AvatarA->GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	AvatarB->GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);

	GState->OnVictory();

	for (APlayerController* Player : AllPlayers)
	{
		ANetPlayerState* PState = Player->GetPlayerState<ANetPlayerState>();
		if (PState)
		{
			if (PState->TeamID == EPlayerTeam::TEAM_Blue)
			{
				PState->Result = EGameResults::RESULT_Lost;
			}
			else
			{
				PState->Result = EGameResults::RESULT_Won;
			}
		}
	}

	FTimerHandle EndGameTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(EndGameTimerHandle, this, &ANetGameMode::EndGame, 2.5f, false);
}

void ANetGameMode::EndGame()
{
	PlayerStartIndex = 0;
	TotalGames++;
	
	ANetGameState* GState = GetGameState<ANetGameState>();
	if (GState)
	{
		GState->WinningPlayer = -1;
	}

	for (APlayerController* Player : AllPlayers)
	{
		APawn* Pawn = Player->GetPawn();
		Player->UnPossess();
		if (Pawn)
		{
			Pawn->Destroy();
		}
		Player->StartSpot.Reset();
		RestartPlayer(Player);
	}

	if (GState)
	{
		GState->TriggerRestart();
	}
}
