// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameMode.h"
#include "WindigoGameMode.generated.h"

/**
 * 
 */
UCLASS()
class WINDIGO_API AWindigoGameMode : public AGameMode
{
	GENERATED_BODY()

	AWindigoGameMode(const class FObjectInitializer& PCIP);

	virtual void BeginPlay() override;

	/* Find a player start */
	virtual AActor * FindPlayerStart ( AController * Player, const FString & IncomingName) override;
	
	/* Query the world for a suitable player start */
	virtual AActor * ChoosePlayerStart ( AController * Player ) override;
};
