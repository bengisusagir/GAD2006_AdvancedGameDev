// Fill out your copyright notice in the Description page of Project Settings.

#include "NetGameInstance.h"
#include "Engine/World.h"
#include "Engine/Engine.h"

void UNetGameInstance::Host(FString MapName, FSPlayerInfo Info)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(0, 1.0f, FColor::Red, TEXT("Hosting Game..."));
	}
	PlayerInfo = Info;
	if (GetWorld())
	{
		GetWorld()->ServerTravel(FString::Printf(TEXT("/Game/Maps/%s?listen"), *MapName));
	}
}

void UNetGameInstance::Join(FString Address, FSPlayerInfo Info)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(0, 1.0f, FColor::Green, FString::Printf(TEXT("Joining Game at %s..."), *Address));
	}
	PlayerInfo = Info;
	if (GetWorld() && GetWorld()->GetFirstPlayerController())
	{
		GetWorld()->GetFirstPlayerController()->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
	}
}
