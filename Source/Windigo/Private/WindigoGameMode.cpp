// Fill out your copyright notice in the Description page of Project Settings.

#include "Windigo.h"
#include "WindigoGameState.h"
#include "WindigoGameMode.h"
#include "PartnerPlayerState.h"
#include "Engine.h"

AWindigoGameMode::AWindigoGameMode(const class FObjectInitializer& PCIP)
	: Super(PCIP)
{
	UE_LOG(WindigoInit, Warning, TEXT("WINDIGO GAMEMODE CREATED"));

	// Set default pawn to BP
	static ConstructorHelpers::FObjectFinder<UBlueprint> PlayerPawnObject(TEXT("Blueprint'/Game/Blueprints/BP_PlayerCharacter.BP_PlayerCharacter'"));
	if (PlayerPawnObject.Object != NULL)
	{
		DefaultPawnClass = (UClass*)PlayerPawnObject.Object->GeneratedClass;
		GameStateClass = AWindigoGameState::StaticClass();
		PlayerStateClass = APartnerPlayerState::StaticClass();
	}
}

void AWindigoGameMode::BeginPlay()
{
	Super::BeginPlay();
}

AActor * AWindigoGameMode::FindPlayerStart(AController * Player, const FString & IncomingName)
{
	return Super::FindPlayerStart(Player, IncomingName);
}

AActor * AWindigoGameMode::ChoosePlayerStart(AController * Player)
{
	return Super::ChoosePlayerStart(Player);
}