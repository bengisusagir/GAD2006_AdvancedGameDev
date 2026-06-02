#include "TileBase.h"

// Sets default values
ATileBase::ATileBase()
{
	// Set this actor to call Tick() every frame. You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	InstancedMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("InstancedMesh"));
	InstancedMesh->SetupAttachment(RootComponent);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));

	if (CubeMesh.Succeeded())
	{
		InstancedMesh->SetStaticMesh(CubeMesh.Object);
	}
	InstancedMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ATileBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	if (BaseMesh)
	{
		InstancedMesh->SetStaticMesh(BaseMesh);
	}
}

// Called when the game starts or when spawned
void ATileBase::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ATileBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
