// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NetBaseCharacter.h"
#include "NetAvatar.generated.h"

class UCameraComponent;
class USpringArmComponent;

/**
 * 
 */
UCLASS()
class LABWORK4_API ANetAvatar : public ANetBaseCharacter
{
	GENERATED_BODY()
	
public:
	ANetAvatar();

	UPROPERTY(EditAnywhere)
	UCameraComponent* Camera;

	UPROPERTY(EditAnywhere)
	USpringArmComponent* SpringArm;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MovementScale;

	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	void MoveForward(float Amount);
	void MoveRight(float Amount);
};
