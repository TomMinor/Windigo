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
	GENERATED_UCLASS_BODY()

	virtual void BeginPlay() override;
	
};
