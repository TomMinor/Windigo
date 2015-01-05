// Copyright 1998-2014 NCCA - Tom Minor, Tom Becque, George Hulm, Alex Long, Kathleen Kononczuk Sa All Rights Reserved.

#pragma once

#include "CharacterFootstepProperties.generated.h"

USTRUCT()
struct FSurfaceTypeInfo
{
	GENERATED_USTRUCT_BODY()

	FSurfaceTypeInfo() {}

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Noise")
	TArray<USoundCue*> SoundEffects;

	/* FX */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Visual")
	TArray<UParticleSystem*> ParticleFX;

	/* Decals */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Visual")
	TArray<UMaterial*> LeftFootprintMaterials;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Visual")
	TArray<UMaterial*> RightFootprintMaterials;
};

/**
 * 
 */
UCLASS(Blueprintable)
class WINDIGO_API UCharacterFootstepProperties : public UObject
{
	GENERATED_BODY()

private:
	const FSurfaceTypeInfo& GetSurfaceInfo(EPhysicalSurface PhysicalSurface) const;

public:
	UFUNCTION()
	const TArray<USoundCue*>& GetSurfaceSounds(EPhysicalSurface PhysicalSurface) const;

	UFUNCTION()
	const TArray<UParticleSystem*>& GetSurfaceParticles(EPhysicalSurface PhysicalSurface) const;

	UFUNCTION()
	const TArray<UMaterial*>& GetSurfaceDecals(EPhysicalSurface PhysicalSurface, bool bIsLeft) const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Physical Surface")
	FSurfaceTypeInfo SurfaceType_Default;

	/* Support 15 custom physical surface types */
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Physical Surface")
	FSurfaceTypeInfo SurfaceType1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Physical Surface")
	FSurfaceTypeInfo SurfaceType2;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Physical Surface")
	FSurfaceTypeInfo SurfaceType3;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Physical Surface")
	FSurfaceTypeInfo SurfaceType4;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Physical Surface")
	FSurfaceTypeInfo SurfaceType5;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Physical Surface")
	FSurfaceTypeInfo SurfaceType6;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Physical Surface")
	FSurfaceTypeInfo SurfaceType7;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Physical Surface")
	FSurfaceTypeInfo SurfaceType8;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Physical Surface")
	FSurfaceTypeInfo SurfaceType9;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Physical Surface")
	FSurfaceTypeInfo SurfaceType10;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Physical Surface")
	FSurfaceTypeInfo SurfaceType11;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Physical Surface")
	FSurfaceTypeInfo SurfaceType12;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Physical Surface")
	FSurfaceTypeInfo SurfaceType13;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Physical Surface")
	FSurfaceTypeInfo SurfaceType14;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Physical Surface")
	FSurfaceTypeInfo SurfaceType15;
};
