// Copyright 1998-2014 NCCA - Tom Minor, Tom Becque, George Hulm, Alex Long, Kathleen Kononczuk Sa All Rights Reserved.

#pragma once

#include "Object.h"
#include "Array.h"
#include "WindigoSingleton.generated.h"

USTRUCT()
struct FSurfaceFootstepProperties
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString SurfaceType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString SurfaceTypeName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<USoundWave*> SoundEffects;

	FSurfaceFootstepProperties()
	{
		SurfaceType = TEXT("Undefined");
		SurfaceTypeName = TEXT("None");
	}

	FSurfaceFootstepProperties(const FString& _SurfaceType, const FString& _SurfaceTypeLabel)
	{
		SurfaceType = _SurfaceType;
		SurfaceTypeName = _SurfaceTypeLabel;
	}
};

/**
 * 
 */
UCLASS(Blueprintable)
class WINDIGO_API UWindigoSingleton : public UObject
{
	GENERATED_BODY()
	
public:
	UWindigoSingleton(const class FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Windigo Global Data")
	TArray<FSurfaceFootstepProperties> PhysicalMaterialFootstepAudio;
};
