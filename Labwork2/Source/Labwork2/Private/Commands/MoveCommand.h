// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Command.h"

/**
 * MoveCommand represents a grid-based unit movement action.
 */
class MoveCommand : public Command
{
public:
	MoveCommand(FSGridPosition Src, FSGridPosition Dst);
	virtual ~MoveCommand();

	virtual void Execute() override;
	virtual void Revert() override;

private:
	FSGridPosition Source;
	FSGridPosition Destination;
};
