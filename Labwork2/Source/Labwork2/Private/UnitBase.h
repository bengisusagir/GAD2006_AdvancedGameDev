// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UnitBase.generated.h"

class AGameSlot;

UCLASS()
class LABWORK2_API AUnitBase : public AActor
{
	GENERATED_BODY()
	
public:	
	AUnitBase();

	void AssignToSlot(AGameSlot* NewSlot);

	UFUNCTION(BlueprintImplementableEvent, BlueprintPure)
	bool IsControlledByThePlayer();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit")
	FVector StartOffset;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Unit")
	AGameSlot* Slot;

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

};
