// Fill out your copyright notice in the Description page of Project Settings.

#include "Windigo.h"
#include "WindigoGameMode.h"
#include "Engine.h"

AWindigoGameMode::AWindigoGameMode(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	// Set default pawn to BP
	static ConstructorHelpers::FObjectFinder<UBlueprint> PlayerPawnObject(TEXT("Blueprint'/Game/Blueprints/BP_PlayerCharacter.BP_PlayerCharacter'"));
	if (PlayerPawnObject.Object != NULL)
	{
		DefaultPawnClass = (UClass*)PlayerPawnObject.Object->GeneratedClass;
	}
}

void AWindigoGameMode::BeginPlay()
{
	Super::BeginPlay();
}