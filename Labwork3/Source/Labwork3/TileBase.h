#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "TileBase.generated.h"

UCLASS()
class LABWORK3_API ATileBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATileBase();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tile")
	UStaticMesh* BaseMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tile")
	UInstancedStaticMeshComponent* InstancedMesh;

	virtual void OnConstruction(const FTransform& Transform) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
