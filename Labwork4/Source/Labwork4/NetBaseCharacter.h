// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Runtime/Engine/Classes/Engine/DataTable.h"
#include "Net/UnrealNetwork.h"
#include "NetGameInstance.h"
#include "NetBaseCharacter.generated.h"

class USkeletalMesh;
class UStaticMesh;
class USkeletalMeshComponent;
class UStaticMeshComponent;

UENUM(BlueprintType)
enum class EBodyPart : uint8
{
	BP_Face = 0,
	BP_Hair = 1,
	BP_Chest = 2,
	BP_Hands = 3,
	BP_Legs = 4,
	BP_Beard = 5,
	BP_BodyType = 6,
	BP_COUNT = 7,
};

USTRUCT(BlueprintType)
struct FSMeshAssetList : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<USkeletalMesh*> ListSkeletal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UStaticMesh*> ListStatic;
};

UCLASS()
class LABWORK4_API ANetBaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ANetBaseCharacter();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void ChangeBodyPart(EBodyPart index, int value, bool DirectSet);

	UFUNCTION(BlueprintCallable)
	void ChangeGender(bool isFemale);

	UFUNCTION(BlueprintCallable)
	void StartRunning();

	UFUNCTION(BlueprintCallable)
	void StopRunning();

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_IsRunning)
	bool bIsRunning;

	UFUNCTION(Server, Reliable)
	void SubmitPlayerInfoToServer(FSPlayerInfo Info);

	UFUNCTION(Server, Reliable)
	void Server_SetRunning(bool bRunning);

	UFUNCTION()
	void OnRep_IsRunning();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(BlueprintReadOnly, Category = "Player Info")
	FSPlayerInfo Data;

	UPROPERTY(BlueprintReadOnly)
	bool PlayerInfoReceived;

	UFUNCTION(BlueprintImplementableEvent)
	void OnPlayerInfoChanged();

	UFUNCTION(BlueprintPure)
	FString GetCustomizationData();

	UFUNCTION(BlueprintCallable)
	void ParseCustomizationData(FString BodyPartData);

	UFUNCTION()
	void CheckPlayerState();

	UFUNCTION()
	void CheckPlayerInfo();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMeshComponent* PartFace;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* PartHair;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* PartBeard;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* PartEyes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMeshComponent* PartHands;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMeshComponent* PartLegs;

private:
	int BodyPartIndices[(int)EBodyPart::BP_COUNT];

	FTimerHandle ClientDataCheckTimer;

	static FSMeshAssetList* GetBodyPartList(EBodyPart part, bool isFemale);

	void UpdateBodyParts();
};
