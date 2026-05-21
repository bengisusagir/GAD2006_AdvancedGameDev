// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UnitBase.h"
#include "GameGrid.h"
#include "Commands/Command.h"
#include "GameManager.generated.h"

USTRUCT(BlueprintType)
struct FSUnitInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
	TSubclassOf<AUnitBase> UnitClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
	FSGridPosition StartPosition;
};

USTRUCT(BlueprintType)
struct FSLevelInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
	TArray<FSUnitInfo> Units;
};

UCLASS()
class LABWORK2_API AGameManager : public AActor
{
	GENERATED_BODY()
	
public:	
	AGameManager();

	void OnActorClicked(AActor* Actor, FKey button);

	void CreateLevelActors(FSLevelInfo& Info);

	UFUNCTION(BlueprintCallable, Category = "GameManager")
	bool UndoLastMove();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game")
	int CurrentLevel;

	UPROPERTY(EditAnywhere, Category = "Game")
	TArray<FSLevelInfo> Levels;

	UPROPERTY(EditAnywhere, Category = "Game")
	AGameGrid* GameGrid;

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(VisibleAnywhere, Category = "Game")
	AUnitBase* ThePlayer;

	TArray<TSharedRef<Command>> CommandPool;
	TSharedPtr<Command> CurrentCommand;
};
