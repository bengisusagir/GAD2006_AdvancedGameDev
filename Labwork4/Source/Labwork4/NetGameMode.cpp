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
	CatcherPlayerIndex = -1;
	LastBluePlayerIndex = -1;
}

void ANetGameMode::BeginPlay()
{
	Super::BeginPlay();

	FString MapName = GetWorld()->GetMapName();
	if (MapName.Contains(TEXT("Lobby")))
	{
		return;
	}

	ANetGameState* GState = GetGameState<ANetGameState>();
	if (GState)
	{
		GState->RemainingTime = 30.0f;
	}

	GetWorld()->GetTimerManager().SetTimer(GameTimerHandle, this, &ANetGameMode::OnGameTimerExpired, 30.0f, false);
	GetWorld()->GetTimerManager().SetTimer(CountdownTimerHandle, this, &ANetGameMode::UpdateRemainingTime, 1.0f, true);
}

void ANetGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	GetWorld()->GetTimerManager().ClearTimer(GameTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(CountdownTimerHandle);
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
			// First game: First connected player is Blue, others are Red.
			State->TeamID = TotalPlayerCount == 0 ? EPlayerTeam::TEAM_Blue : EPlayerTeam::TEAM_Red;
			State->PlayerIndex = TotalPlayerCount++;
			if (State->TeamID == EPlayerTeam::TEAM_Blue)
			{
				LastBluePlayerIndex = State->PlayerIndex;
			}
			AllPlayers.Add(Cast<APlayerController>(Player));

			FString MapName = GetWorld()->GetMapName();
			if (!MapName.Contains(TEXT("Lobby")))
			{
				ANetGameState* GState = GetGameState<ANetGameState>();
				if (GState)
				{
					GState->RemainingTime = 30.0f;
				}
				GetWorld()->GetTimerManager().SetTimer(GameTimerHandle, this, &ANetGameMode::OnGameTimerExpired, 30.0f, false);
				GetWorld()->GetTimerManager().SetTimer(CountdownTimerHandle, this, &ANetGameMode::UpdateRemainingTime, 1.0f, true);
			}
		}
		else
		{
			// Subsequent games: The new Blue player index has already been calculated and stored in LastBluePlayerIndex by EndGame().
			State->TeamID = State->PlayerIndex == LastBluePlayerIndex ? EPlayerTeam::TEAM_Blue : EPlayerTeam::TEAM_Red;
		}

		if (State->TeamID == EPlayerTeam::TEAM_Blue)
		{
			Start = GetPlayerStart("Blue", -1);
		}
		else
		{
			Start = GetPlayerStart("Red", PlayerStartIndex++);
		}

		// Sync the replicated Data struct with the newly assigned team ID so that clients receive the update
		State->Data.TeamID = State->TeamID;
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

	GetWorld()->GetTimerManager().ClearTimer(GameTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(CountdownTimerHandle);

	if (StateA->TeamID == EPlayerTeam::TEAM_Blue)
	{
		GState->WinningPlayer = StateB->PlayerIndex;
		CatcherPlayerIndex = StateB->PlayerIndex;
	}
	else
	{
		GState->WinningPlayer = StateA->PlayerIndex;
		CatcherPlayerIndex = StateA->PlayerIndex;
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

void ANetGameMode::OnGameTimerExpired()
{
	GetWorld()->GetTimerManager().ClearTimer(CountdownTimerHandle);
	ANetGameState* GState = GetGameState<ANetGameState>();
	if (GState == nullptr || GState->WinningPlayer >= 0) return;

	// Timer ran out: Blue team player survived and wins, Red team loses.
	int BluePlayerIndex = -1;
	for (APlayerController* Player : AllPlayers)
	{
		if (Player)
		{
			ANetPlayerState* PState = Player->GetPlayerState<ANetPlayerState>();
			if (PState && PState->TeamID == EPlayerTeam::TEAM_Blue)
			{
				BluePlayerIndex = PState->PlayerIndex;
				break;
			}
		}
	}

	GState->WinningPlayer = BluePlayerIndex;
	GState->OnVictory();

	for (APlayerController* Player : AllPlayers)
	{
		if (Player)
		{
			ANetPlayerState* PState = Player->GetPlayerState<ANetPlayerState>();
			if (PState)
			{
				if (PState->TeamID == EPlayerTeam::TEAM_Blue)
				{
					PState->Result = EGameResults::RESULT_Won;
				}
				else
				{
					PState->Result = EGameResults::RESULT_Lost;
				}
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
	
	int NewBlueIndex = -1;
	if (CatcherPlayerIndex >= 0)
	{
		// Catcher wins: Catcher is the new Blue player
		NewBlueIndex = CatcherPlayerIndex;
	}
	else
	{
		// Blue wins (timer ran out): Select next player sequentially as the new Blue player
		NewBlueIndex = (LastBluePlayerIndex + 1) % TotalPlayerCount;
	}
	LastBluePlayerIndex = NewBlueIndex;
	
	// Reset catcher index for the new round
	CatcherPlayerIndex = -1;

	// Clear game timer (safety check)
	GetWorld()->GetTimerManager().ClearTimer(GameTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(CountdownTimerHandle);

	ANetGameState* GState = GetGameState<ANetGameState>();
	if (GState)
	{
		GState->WinningPlayer = -1;
		GState->RemainingTime = 30.0f;
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

	// Restart the 30-second game timer for the new round
	GetWorld()->GetTimerManager().SetTimer(GameTimerHandle, this, &ANetGameMode::OnGameTimerExpired, 30.0f, false);
	// Restart the countdown timer for the new round
	GetWorld()->GetTimerManager().SetTimer(CountdownTimerHandle, this, &ANetGameMode::UpdateRemainingTime, 1.0f, true);

	if (GState)
	{
		GState->TriggerRestart();
	}
}

void ANetGameMode::UpdateRemainingTime()
{
	ANetGameState* GState = GetGameState<ANetGameState>();
	if (GState)
	{
		GState->RemainingTime = FMath::Max(0.0f, GState->RemainingTime - 1.0f);
	}
}
