#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TilePlayerController.generated.h"

class ATileGameManager;

UCLASS()
class LABWORK3_API ATilePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintReadWrite, Category = "Tile System")
	ATileGameManager* GameManager;
};
