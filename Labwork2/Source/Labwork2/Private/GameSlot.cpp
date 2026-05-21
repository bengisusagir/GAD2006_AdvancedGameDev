// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSlot.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Materials/MaterialInterface.h"
#include "UnitBase.h"
#include "TBPlayerController.h"
#include "Engine/World.h"

// Sets default values
AGameSlot::AGameSlot()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	Box->SetupAttachment(RootComponent);
	Box->SetBoxExtent(FVector(50, 50, 2));
	Box->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Box->SetCollisionResponseToAllChannels(ECR_Block);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> DefaultSlotMesh(TEXT("/Engine/BasicShapes/Plane"));

	Plane = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Plane"));
	Plane->SetupAttachment(RootComponent);
	Plane->SetStaticMesh(DefaultSlotMesh.Object);
	Plane->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Plane->SetCollisionResponseToAllChannels(ECR_Block);

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> DefaultSlotMaterial(TEXT("Material'/Game/Materials/MAT_GridSlot.MAT_GridSlot'"));
	DefaultMaterial = DefaultSlotMaterial.Object;

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> DefaultSlotMaterial_H(TEXT("MaterialInstanceConstant'/Game/Materials/MAT_GridSlot_Highlighted.MAT_GridSlot_Highlighted'"));
	HighlightedMaterial = DefaultSlotMaterial_H.Object;

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> DefaultSlotMaterial_O(TEXT("MaterialInstanceConstant'/Game/Materials/MAT_GridSlot_Offensive.MAT_GridSlot_Offensive'"));
	OffensiveMaterial = DefaultSlotMaterial_O.Object;

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> DefaultSlotMaterial_S(TEXT("MaterialInstanceConstant'/Game/Materials/MAT_GridSlot_Supportive.MAT_GridSlot_Supportive'"));
	SupportiveMaterial = DefaultSlotMaterial_S.Object;

	SetState(EGridState::GS_Default);
	Unit = nullptr;
	bVisited = false;
}

// Called when the game starts or when spawned
void AGameSlot::BeginPlay()
{
	Super::BeginPlay();
	
	OnClicked.AddUniqueDynamic(this, &AGameSlot::OnGridClicked);
	Box->OnClicked.AddUniqueDynamic(this, &AGameSlot::OnComponentClicked);
	Plane->OnClicked.AddUniqueDynamic(this, &AGameSlot::OnComponentClicked);

}

// Called every frame
void AGameSlot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGameSlot::SetState(EGridState NewState)
{
	GridState = NewState;

	if (!Plane) return;

	switch (NewState)
	{
	case EGridState::GS_Default:
		Plane->SetMaterial(0, DefaultMaterial);
		break;
	case EGridState::GS_Highlighted:
		Plane->SetMaterial(0, HighlightedMaterial);
		break;
	case EGridState::GS_Offensive:
		Plane->SetMaterial(0, OffensiveMaterial);
		break;
	case EGridState::GS_Supportive:
		Plane->SetMaterial(0, SupportiveMaterial);
		break;
	}
}

void AGameSlot::SpawnUnitHere(TSubclassOf<AUnitBase>& UnitClass)
{
	FVector Location = GetActorLocation();
	AUnitBase* NewUnit = Cast<AUnitBase>(GetWorld()->SpawnActor(UnitClass, &Location));
	if (NewUnit) {
		NewUnit->AssignToSlot(this);
	}
}

void AGameSlot::OnGridClicked(AActor* TouchedActor, FKey ButtonPressed)
{

	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (PC)
	{
		ATBPlayerController* TBPC = Cast<ATBPlayerController>(PC);
		if (TBPC)
		{
			TBPC->OnActorClicked(this, ButtonPressed);
		}
	}
}

void AGameSlot::OnComponentClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed)
{

	OnGridClicked(this, ButtonPressed);
}
