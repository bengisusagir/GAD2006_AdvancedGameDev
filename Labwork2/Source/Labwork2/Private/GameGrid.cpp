// Fill out your copyright notice in the Description page of Project Settings.

#include "GameGrid.h"
#include "Components/ChildActorComponent.h"
#include "Components/BoxComponent.h"
#include "GameSlot.h"

//Global reference to the gamegrid
AGameGrid* AGameGrid::GameGrid = nullptr;

// Sets default values
AGameGrid::AGameGrid()
{
	NumRows = 8;
	NumCols = 8;
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	GameGrid = this;
}

void AGameGrid::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	for (auto Grid : GridActors)
	{
		if (Grid)
		{
			Grid->DestroyComponent();
		}
	}

	GridActors.Empty();

	if (!GridClass->IsValidLowLevel()) return;

	AGameSlot* Slot = GridClass->GetDefaultObject<AGameSlot>();

	if (Slot == nullptr) return;

	FVector Extends = Slot->Box->GetScaledBoxExtent() * 2;

	for (int i = 0; i < NumRows; i++)
	{
		for (int j = 0; j < NumCols; j++)
		{
			FName GridName(FString::Printf(TEXT("Slot%dx%d"), j, i));
			auto Grid = NewObject<UChildActorComponent>(this, UChildActorComponent::StaticClass(), GridName);
			Grid->RegisterComponent();
			Grid->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
			Grid->SetRelativeLocation(
				FVector(
					(NumRows - i - 1) * Extends.X - (NumRows * 0.5f - 0.5f) * Extends.X,
									j * Extends.Y - (NumCols * 0.5f - 0.5f) * Extends.Y, 1.0f));

			GridActors.Add(Grid);

			Grid->SetChildActorClass(GridClass);
			if (AGameSlot* GameSlot = Cast<AGameSlot>(Grid->GetChildActor()))
			{
#if WITH_EDITOR
				GameSlot->SetActorLabel(GridName.ToString());
#endif
			}
		}
	}
}

// Called when the game starts or when spawned
void AGameGrid::BeginPlay()
{
	Super::BeginPlay();

	int GridIndex = 0;

	for (int i = 0; i < NumRows; i++)
	{
		for (int j = 0; j < NumCols; j++)
		{
			if (AGameSlot* GameSlot = Cast<AGameSlot>(GridActors[GridIndex]->GetChildActor()))
			{
				GameSlot->GridPosition = FSGridPosition(j, i);
			}
			GridIndex++;
		}
	}
}

// Called every frame
void AGameGrid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

AGameSlot* AGameGrid::GetSlot(FSGridPosition& Position)
{
	int GridIndex = Position.Row * NumCols + Position.Col;
	if (GridActors.IsValidIndex(GridIndex)) {
		return Cast<AGameSlot>(GridActors[GridIndex]->GetChildActor());
	}
	return nullptr;
}

AGameSlot* AGameGrid::FindSlot(FSGridPosition Position)
{
	if (GameGrid) {
		return GameGrid->GetSlot(Position);
	}
	return nullptr;
}

