// Copyright 1998-2014 NCCA - Tom Minor, Tom Becque, George Hulm, Alex Long, Kathleen Kononczuk Sa All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "GameFootstepEffect.generated.h"


USTRUCT()
struct FDecalInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, Category = Decal)
	UMaterial* Material;

	UPROPERTY(EditDefaultsOnly, Category = Decal)
	float Size;

	FDecalInfo()
		: Material(NULL),
		Size(24.f)
	{}
};

/**
 * Actor class Stores the state of a single footstep.
 *  - The sound it plays
 *  - The decal material it spawns
 *  - The particle effects it spawns 
 *  - The lifespan
 */
UCLASS()
class WINDIGO_API AGameFootstepEffect : public AActor
{
	GENERATED_BODY()

public:
	AGameFootstepEffect(const FObjectInitializer& ObjectInitializer);

	virtual void PostInitializeComponents() override;

	UPROPERTY()
	USoundCue*			SoundComponent;

	UPROPERTY()
	FDecalInfo			DecalMaterial;

	UPROPERTY()
	UParticleSystem*	ParticleFX;

	UPROPERTY()
	FHitResult			SurfaceHit;

	/* How long will the footstep persist */
	UPROPERTY()
	float				Lifespan;

	UPROPERTY()
	bool				bDebugDrawFootstepNormal;

protected:
	virtual void Tick(float Delta);
};
