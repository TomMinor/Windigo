// Copyright 1998-2014 NCCA - Tom Minor, Tom Becque, George Hulm, Alex Long, Kathleen Kononczuk Sa All Rights Reserved.

#include "Windigo.h"
#include "GameFootstepEffect.h"


AGameFootstepEffect::AGameFootstepEffect(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Enable tick so we can destroy it after a certain time
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	bDebugDrawFootstepNormal = false;
	Lifespan = 2.0f;
	
	SoundComponent = NULL;
	ParticleFX = NULL;
}

void AGameFootstepEffect::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (ParticleFX)
	{
		UParticleSystemComponent* FXComponent = UGameplayStatics::SpawnEmitterAtLocation(this, ParticleFX, SurfaceHit.ImpactPoint, SurfaceHit.ImpactNormal.Rotation());

		// Add to component list so it will automatically be destroyed
		if (FXComponent)
		{
			SerializedComponents.Add(FXComponent);
		}
	}

	if (SoundComponent)
	{
		UGameplayStatics::PlaySoundAtLocation(this, SoundComponent, GetActorLocation());
	}

	if (DecalMaterial.Material)
	{
		FRotator DecalRotation = SurfaceHit.ImpactNormal.Rotation();
		DecalRotation.Yaw = GetOwner()->GetActorRotation().Yaw; // Match character's yaw orientation
		DecalRotation += FRotator(0, 90, 0); // Fix decal rotation so it faces forward

		if (bDebugDrawFootstepNormal) 
		{
			// Draw a line representating the decal normal
			const float TraceLength = 64.0f;

			DrawDebugLine(
				GetWorld(),
				SurfaceHit.ImpactPoint,
				SurfaceHit.ImpactPoint + TraceLength * SurfaceHit.ImpactNormal,
				FColor(255, 0, 0),
				false,
				Lifespan,
				0,
				3
				);
		}

		UGameplayStatics::SpawnDecalAttached(
			DecalMaterial.Material,
			FVector(DecalMaterial.Size, DecalMaterial.Size, 64.0f),
			SurfaceHit.Component.Get(),
			SurfaceHit.BoneName,
			SurfaceHit.ImpactPoint,
			DecalRotation,
			EAttachLocation::KeepWorldPosition,
			Lifespan);
	}
}

void AGameFootstepEffect::Tick(float Delta)
{
	Super::Tick(Delta);

	const float Lifetime = GetWorld()->GetTimeSeconds() - CreationTime;
	if (Lifetime > Lifespan)
	{
		Destroy(true);
	}
}

