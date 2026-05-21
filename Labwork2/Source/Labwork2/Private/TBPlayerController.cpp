// Fill out your copyright notice in the Description page of Project Settings.

#include "TBPlayerController.h"
#include "GameManager.h"

ATBPlayerController::ATBPlayerController()
{
	GameManager = nullptr;
}

void ATBPlayerController::BeginPlay()
{
	Super::BeginPlay();

	bEnableClickEvents = true;
	bShowMouseCursor = true;
	ClickEventKeys.AddUnique(EKeys::RightMouseButton);
	ClickEventKeys.AddUnique(EKeys::LeftMouseButton);

	FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputMode.SetHideCursorDuringCapture(false);
	SetInputMode(InputMode);

}

void ATBPlayerController::OnActorClicked(AActor* Actor, FKey key)
{
	if (GameManager)
	{
		GameManager->OnActorClicked(Actor, key);
	}
}
