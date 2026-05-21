// Fill out your copyright notice in the Description page of Project Settings.

#include "GameManager.h"
#include "TBPlayerController.h"
#include "Commands/MoveCommand.h"
#include "Engine/World.h"

AGameManager::AGameManager()
{
	PrimaryActorTick.bCanEverTick = true;

	CurrentLevel = 0;
	ThePlayer = nullptr;
	GameGrid = nullptr;
}

void AGameManager::BeginPlay()
{
	Super::BeginPlay();

	if (auto PlayerController = GetWorld()->GetFirstPlayerController<ATBPlayerController>())
	{
		PlayerController->GameManager = this;
	}

	if (Levels.IsValidIndex(CurrentLevel))
	{
		CreateLevelActors(Levels[CurrentLevel]);
	}
}

void AGameManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CurrentCommand.IsValid())
	{
		CurrentCommand->Update(DeltaTime);
	}
}

void AGameManager::CreateLevelActors(FSLevelInfo& Info)
{
	ThePlayer = nullptr;

	for (auto UnitInfo : Info.Units)
	{
		if (GameGrid)
		{
			if (AGameSlot* Slot = GameGrid->GetSlot(UnitInfo.StartPosition))
			{
				Slot->SpawnUnitHere(UnitInfo.UnitClass);

				if (Slot->Unit && Slot->Unit->IsControlledByThePlayer())
				{
					ThePlayer = Slot->Unit;
				}
			}
		}
	}
}

void AGameManager::OnActorClicked(AActor* Actor, FKey button)
{
	if (CurrentCommand.IsValid() && CurrentCommand->IsExecuting()) return;

	AGameSlot* Slot = Cast<AGameSlot>(Actor);

	if (!Slot) return;

	if (!ThePlayer)
	{
		return;
	}

	if (Slot->Unit == nullptr && ThePlayer->Slot)
	{
		TSharedRef<MoveCommand> Cmd = MakeShared<MoveCommand>(ThePlayer->Slot->GridPosition, Slot->GridPosition);
		CommandPool.Add(Cmd);
		Cmd->Execute();
		CurrentCommand = Cmd;
	}
}

bool AGameManager::UndoLastMove()
{
	if (CommandPool.Num() == 0) return false;

	TSharedRef<Command> LastCmd = CommandPool.Pop();
	LastCmd->Revert();

	if (CurrentCommand == LastCmd)
	{
		CurrentCommand = nullptr;
	}

	return true;
}
