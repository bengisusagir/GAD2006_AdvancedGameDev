#include "TileGameManager.h"
#include "TilePlayerController.h"
#include "GameFramework/PlayerInput.h"
#include "InputCoreTypes.h"

// Sets default values
ATileGameManager::ATileGameManager() :
	GridSize(100),
	GridOffset(0, 0, 0.5f)
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	GridSelection = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GridMesh"));
	GridSelection->SetupAttachment(RootComponent);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> PlaneMesh(TEXT("/Engine/BasicShapes/Plane.Plane"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> GridMaterial(TEXT("/Game/UI/MAT_GridSlot.MAT_GridSlot"));

	GridSelection->SetStaticMesh(PlaneMesh.Object);
	GridSelection->SetMaterial(0, GridMaterial.Object);
	GridSelection->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	MapExtendsInGrids = 25;
	CurrentTileIndex = 0;
	CurrentRotationYaw = 0.0f;

	for (int i = 0; i < MAX_MAP_GRID_SIZE; i++)
	{
		for (int j = 0; j < MAX_MAP_GRID_SIZE; j++)
		{
			Map[i][j] = nullptr;
		}
	}
}

// Called when the game starts or when spawned
void ATileGameManager::BeginPlay()
{
	Super::BeginPlay();

	if (auto PlayerController = Cast<ATilePlayerController>(GWorld->GetFirstPlayerController()))
	{
		PlayerController->GameManager = this;
	}

	if (TileTypes.IsValidIndex(0))
	{
		ApplyGridSelection();
	}
}

// Called every frame
void ATileGameManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATileGameManager::OnActorInteraction(AActor* HitActor, FVector& Location, bool IsPressed)
{
	if (TileTypes.Num() == 0) return;

	FVector GridLoc = GridOffset;
	GridLoc.X = FMath::GridSnap(Location.X, GridSize);
	GridLoc.Y = FMath::GridSnap(Location.Y, GridSize);
	GridLoc.Z = Location.Z;

	UPlayerInput* Input = GWorld->GetFirstPlayerController()->PlayerInput;

	if (Input->WasJustPressed(EKeys::LeftMouseButton))
	{
		int GridX = GridLoc.X / GridSize + MapExtendsInGrids;
		int GridY = GridLoc.Y / GridSize + MapExtendsInGrids;

		if (GridX < 0 || GridY < 0 || GridX >= MapExtendsInGrids*2 || GridY >= MapExtendsInGrids*2)
		{
			return;
		}

		if (Map[GridX][GridY] != nullptr) return;

		if (TileTypes.IsValidIndex(CurrentTileIndex))
		{
			ATileBase* SelectedTile = TileTypes[CurrentTileIndex];
			Map[GridX][GridY] = SelectedTile;

			FTransform TileTransform(FRotator(0.0f, CurrentRotationYaw, 0.0f), GridLoc + GridOffset);
			SelectedTile->InstancedMesh->AddInstance(
				SelectedTile->InstancedMesh->GetRelativeTransform() * TileTransform,
				true
			);
		}
	}
	else if (Input->WasJustPressed(EKeys::MouseScrollDown))
	{
		CurrentTileIndex = (CurrentTileIndex + 1) % TileTypes.Num();
		ApplyGridSelection();
		UE_LOG(LogTemp, Warning, TEXT("TileType: %s"), *TileTypes[CurrentTileIndex]->GetActorLabel());
	}
	else if (Input->WasJustPressed(EKeys::MouseScrollUp))
	{
		CurrentTileIndex = (CurrentTileIndex - 1 + TileTypes.Num()) % TileTypes.Num();
		ApplyGridSelection();
		UE_LOG(LogTemp, Warning, TEXT("TileType: %s"), *TileTypes[CurrentTileIndex]->GetActorLabel());
	}
	else if (Input->WasJustPressed(EKeys::RightMouseButton))
	{
		CurrentRotationYaw = FMath::Fmod(CurrentRotationYaw + 90.0f, 360.0f);
	}
	else
	{
		GridSelection->SetWorldLocationAndRotation(GridLoc + GridOffset, FRotator(0.0f, CurrentRotationYaw, 0.0f));
	}

	if (IsPressed)
	{
		UE_LOG(LogTemp, Warning, TEXT("Hit: %s - %f,%f,%f"),
			HitActor ? *HitActor->GetActorLabel() : TEXT("None"),
			Location.X,
			Location.Y,
			Location.Z);
	}
}

void ATileGameManager::ApplyGridSelection()
{
	ATileBase* Tile = TileTypes[CurrentTileIndex];
	if (!Tile || !Tile->BaseMesh) return;

	GridSelection->SetStaticMesh(Tile->BaseMesh);
	GridSelection->EmptyOverrideMaterials();
	GridSelection->SetRelativeScale3D(Tile->InstancedMesh->GetRelativeScale3D());
}
