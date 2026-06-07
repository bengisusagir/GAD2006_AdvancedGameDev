// Fill out your copyright notice in the Description page of Project Settings.

#include "NetBaseCharacter.h"
#include "NetGameInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/World.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/CharacterMovementComponent.h"

static UDataTable* SBodyParts = nullptr;

static const TArray<FString> BodyPartNames
{
	TEXT("Face"),
	TEXT("Hair"),
	TEXT("Chest"),
	TEXT("Hands"),
	TEXT("Legs"),
	TEXT("Beard"),
	TEXT("Eyebrows")
};

// Sets default values
ANetBaseCharacter::ANetBaseCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bIsRunning = false;

	PartFace = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Face"));
	PartFace->SetupAttachment(GetMesh());
	PartFace->SetLeaderPoseComponent(GetMesh());

	PartHands = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Hands"));
	PartHands->SetupAttachment(GetMesh());
	PartHands->SetLeaderPoseComponent(GetMesh());

	PartLegs = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Legs"));
	PartLegs->SetupAttachment(GetMesh());
	PartLegs->SetLeaderPoseComponent(GetMesh());

	PartHair = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Hair"));
	PartHair->SetupAttachment(PartFace, FName("headSocket"));

	PartBeard = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Beard"));
	PartBeard->SetupAttachment(PartFace, FName("headSocket"));

	PartEyebrows = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Eyebrows"));
	PartEyebrows->SetupAttachment(PartFace, FName("headSocket"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SK_Eyes(TEXT("StaticMesh'/Game/StylizedModularChar/Meshes/SM_Eyes.SM_Eyes'"));

	PartEyes = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Eyes"));
	PartEyes->SetupAttachment(PartFace, FName("headSocket"));
	if (SK_Eyes.Succeeded())
	{
		PartEyes->SetStaticMesh(SK_Eyes.Object);
	}

	static ConstructorHelpers::FObjectFinder<UDataTable> DT_BodyParts(TEXT("DataTable'/Game/Blueprints/DT_BodyParts.DT_BodyParts'"));
	if (DT_BodyParts.Succeeded())
	{
		SBodyParts = DT_BodyParts.Object;
	}

	bReplicates = true;
}

// Called when the game starts or when spawned
void ANetBaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocallyControlled())
	{
		UNetGameInstance* Instance = GetWorld() ? Cast<UNetGameInstance>(GetWorld()->GetGameInstance()) : nullptr;
		if (Instance && Instance->PlayerInfo.Ready)
		{
			SubmitPlayerInfoToServer(Instance->PlayerInfo);
		}
	}
}

void ANetBaseCharacter::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	UpdateBodyParts();
}

// Called every frame
void ANetBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ANetBaseCharacter::ChangeBodyPart(EBodyPart index, int value, bool DirectSet)
{
	if (index == EBodyPart::BP_Beard && PartSelection.isFemale)
	{
		PartBeard->SetStaticMesh(nullptr);
		return;
	}

	FSMeshAssetList* List = GetBodyPartList(index, PartSelection.isFemale);
	if (List == nullptr) return;

	int CurrentIndex = PartSelection.Indices[(int)index];

	if (DirectSet) {
		CurrentIndex = value;
	} else {
		CurrentIndex += value;
	}

	int Num = List->ListSkeletal.Num() + List->ListStatic.Num();
	if (Num == 0) return;

	if (CurrentIndex < 0)
		CurrentIndex += Num;
	else
		CurrentIndex %= Num;

	PartSelection.Indices[(int)index] = CurrentIndex;

	switch (index)
	{
	case EBodyPart::BP_Face:
		if (List->ListSkeletal.IsValidIndex(CurrentIndex))
		{
			PartFace->SetSkeletalMeshAsset(List->ListSkeletal[CurrentIndex]);
		}
		break;
	case EBodyPart::BP_Beard:
		if (List->ListStatic.IsValidIndex(CurrentIndex))
		{
			PartBeard->SetStaticMesh(List->ListStatic[CurrentIndex]);
		}
		break;
	case EBodyPart::BP_Eyebrows:
		if (List->ListStatic.IsValidIndex(CurrentIndex))
		{
			PartEyebrows->SetStaticMesh(List->ListStatic[CurrentIndex]);
		}
		break;
	case EBodyPart::BP_Chest:
		if (List->ListSkeletal.IsValidIndex(CurrentIndex))
		{
			GetMesh()->SetSkeletalMeshAsset(List->ListSkeletal[CurrentIndex]);
		}
		break;
	case EBodyPart::BP_Hair:
		if (List->ListStatic.IsValidIndex(CurrentIndex))
		{
			PartHair->SetStaticMesh(List->ListStatic[CurrentIndex]);
		}
		break;
	case EBodyPart::BP_Hands:
		if (List->ListSkeletal.IsValidIndex(CurrentIndex))
		{
			PartHands->SetSkeletalMeshAsset(List->ListSkeletal[CurrentIndex]);
		}
		break;
	case EBodyPart::BP_Legs:
		if (List->ListSkeletal.IsValidIndex(CurrentIndex))
		{
			PartLegs->SetSkeletalMeshAsset(List->ListSkeletal[CurrentIndex]);
		}
		break;
	}
}

void ANetBaseCharacter::ChangeGender(bool isFemale)
{
	PartSelection.isFemale = isFemale;
	UpdateBodyParts();
}

void ANetBaseCharacter::OnRep_PlayerInfoChanged()
{
	UpdateBodyParts();
}

void ANetBaseCharacter::SubmitPlayerInfoToServer_Implementation(FSPlayerInfo Info)
{
	PartSelection = Info.BodyParts;
	if (HasAuthority())
	{
		OnRep_PlayerInfoChanged();
	}
}

void ANetBaseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ANetBaseCharacter, PartSelection);
	DOREPLIFETIME(ANetBaseCharacter, bIsRunning);
}

void ANetBaseCharacter::UpdateBodyParts()
{
	ChangeBodyPart(EBodyPart::BP_Face, 0, false);
	ChangeBodyPart(EBodyPart::BP_Beard, 0, false);
	ChangeBodyPart(EBodyPart::BP_Eyebrows, 0, false);
	ChangeBodyPart(EBodyPart::BP_Chest, 0, false);
	ChangeBodyPart(EBodyPart::BP_Hair, 0, false);
	ChangeBodyPart(EBodyPart::BP_Hands, 0, false);
	ChangeBodyPart(EBodyPart::BP_Legs, 0, false);
}

void ANetBaseCharacter::StartRunning()
{
	if (!bIsRunning)
	{
		bIsRunning = true;
		OnRep_IsRunning();
		if (!HasAuthority())
		{
			Server_SetRunning(true);
		}
	}
}

void ANetBaseCharacter::StopRunning()
{
	if (bIsRunning)
	{
		bIsRunning = false;
		OnRep_IsRunning();
		if (!HasAuthority())
		{
			Server_SetRunning(false);
		}
	}
}

void ANetBaseCharacter::Server_SetRunning_Implementation(bool bRunning)
{
	bIsRunning = bRunning;
	OnRep_IsRunning();
}

void ANetBaseCharacter::OnRep_IsRunning()
{
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = bIsRunning ? 600.0f : 300.0f;
	}
}

FSMeshAssetList* ANetBaseCharacter::GetBodyPartList(EBodyPart part, bool isFemale)
{
	FString Name = FString::Printf(TEXT("%s%s"), isFemale ? TEXT("Female") : TEXT("Male"), *BodyPartNames[(int)part]);
	return SBodyParts ? SBodyParts->FindRow<FSMeshAssetList>(*Name, nullptr) : nullptr;
}
