// Fill out your copyright notice in the Description page of Project Settings.

#include "MoveCommand.h"
#include "GameGrid.h"
#include "UnitBase.h"

MoveCommand::MoveCommand(FSGridPosition Src, FSGridPosition Dst) :
	Source(Src),
	Destination(Dst)
{
}

MoveCommand::~MoveCommand()
{
}

void MoveCommand::Execute()
{
	AGameSlot* SlotA = AGameGrid::FindSlot(Source);
	AGameSlot* SlotB = AGameGrid::FindSlot(Destination);

	if (SlotA && SlotB)
	{
		AUnitBase* UnitA = SlotA->Unit;
		check(UnitA);
		UnitA->AssignToSlot(SlotB);
		SlotB->SetState(EGridState::GS_Highlighted);
	}
}

void MoveCommand::Revert()
{
	AGameSlot* SlotA = AGameGrid::FindSlot(Source);
	AGameSlot* SlotB = AGameGrid::FindSlot(Destination);

	if (SlotA && SlotB)
	{
		AUnitBase* UnitB = SlotB->Unit;
		check(UnitB);
		UnitB->AssignToSlot(SlotA);
		SlotB->SetState(EGridState::GS_Default);
	}
}
