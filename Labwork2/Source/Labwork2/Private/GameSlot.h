// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameSlot.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class AUnitBase;
class UMaterialInterface;

USTRUCT(Blueprintable)
struct FSGridPosition
{
	GENERATED_USTRUCT_BODY();

	FSGridPosition() {}
	FSGridPosition(int col, int row) : Col(col), Row(row) {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 Col;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 Row;
};

UENUM(BlueprintType)
enum class EGridState : uint8
{
	GS_Default,
	GS_Highlighted,
	GS_Offensive,
	GS_Supportive,
};

UCLASS()
class AGameSlot : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AGameSlot();

	UPROPERTY(EditAnywhere)
	UBoxComponent* Box;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* Plane;

	FSGridPosition GridPosition;

	EGridState GridState;

	UFUNCTION()
	void SetState(EGridState NewState);

	UPROPERTY(VisibleAnywhere)
	AUnitBase* Unit;

	void SpawnUnitHere(TSubclassOf<AUnitBase>& UnitClass);

	UFUNCTION()
	void OnGridClicked(AActor* TouchedActor, FKey ButtonPressed);

	UFUNCTION()
	void OnComponentClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY()
	UMaterialInterface* DefaultMaterial;

	UPROPERTY()
	UMaterialInterface* HighlightedMaterial;

	UPROPERTY()
	UMaterialInterface* OffensiveMaterial;

	UPROPERTY()
	UMaterialInterface* SupportiveMaterial;
};
