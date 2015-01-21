// Copyright 1998-2014 NCCA - Tom Minor, Tom Becque, George Hulm, Alex Long, Kathleen Kononczuk Sa All Rights Reserved.

#include "Windigo.h"
#include "Runtime/Landscape/Classes/Landscape.h"
#include "LandscapeComponent.h"
#include "LandscapeProxy.h"
#include "LandscapeHeightfieldCollisionComponent.h"
#include "GameFootstepEffect.h"
#include "GameFramework/Character.h"
#include "WindigoBaseCharacter.h"

AWindigoBaseCharacter::AWindigoBaseCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NoiseEmitter = ObjectInitializer.CreateDefaultSubobject<UPawnNoiseEmitterComponent>(this, TEXT("Noise Emitter"));

	/* Character sensing component defaults */
	CharacterSenses = ObjectInitializer.CreateDefaultSubobject<UPawnSensingComponent>(this, TEXT("Creature Senses"));
	CharacterSenses->SensingInterval = 0.1f;
	CharacterSenses->HearingThreshold = 756.f;
	CharacterSenses->LOSHearingThreshold = 1024.f;
	CharacterSenses->bOnlySensePlayers = false;
	CharacterSenses->SetPeripheralVisionAngle(85.0f);

	/* Debug Member Defaults */
	bShouldLogAISenses = false;
	bDrawLastKnownSound = false;
	bDrawDecalNormals = false;
	FootstepLifetime = 32.0f;
	DebugSoundColour = FColor(255, 160, 0);
	SoundRadius = 96.0f;
	SoundRadiusSegments = 12;
	SoundHearingLifetime = 2;

	DebugOccludedHearingRangeColour = FColor(0, 255, 0);
	DebugUnoccludedHearingRangeColour = FColor(128, 0, 0);
	HearingRangeSegments = 16;
	HearingRangeLifetime = 0.025;
}

void AWindigoBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AWindigoBaseCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	CharacterSenses->OnSeePawn.AddDynamic(this, &AWindigoBaseCharacter::OnSeePawn);
	CharacterSenses->OnHearNoise.AddDynamic(this, &AWindigoBaseCharacter::OnHearNoise);
}

void AWindigoBaseCharacter::OnHearNoise(APawn *HeardPawn, const FVector &Location, float Volume)
{
	if (bShouldLogAISenses)
	{
		const FString VolumeDesc = FString::Printf(TEXT(" at volume %f"), Volume);
		FString message = GetName() + TEXT(" heard actor ") + HeardPawn->GetName() + VolumeDesc;
		UE_LOG(WindigoAI, Log, TEXT("%s"), *message);
	}

	if (bDrawLastKnownSound) 
	{
		FHitResult Hit = TraceGround(Location);
		const float HeightDifference = Location.Z - Hit.ImpactPoint.Z;
		FVector NewLocation = Location - FVector(0, 0, (HeightDifference + GetDefaultHalfHeight()));

		// Draw radius of unoccluded hearing
		DrawDebugSphere(GetWorld(),
			NewLocation,
			SoundRadius,
			SoundRadiusSegments,
			DebugSoundColour,
			false,
			SoundHearingLifetime,
			0);
	}


	const FVector PlayerLocation = HeardPawn->GetActorLocation();
	const float Distance = (PlayerLocation- GetActorLocation()).Size();

	OnSeePlayer(PlayerLocation, Distance);
}

void AWindigoBaseCharacter::OnSeePawn(APawn *SeenPawn)
{
	if (bShouldLogAISenses)
	{
		FString message = GetName() + TEXT(" spotted Actor ") + SeenPawn->GetName();
		UE_LOG(WindigoAI, Log, TEXT("%s"), *message);
	}

	const FVector PlayerLocation = SeenPawn->GetActorLocation();
	const float Distance = (PlayerLocation - GetActorLocation()).Size();

	OnSeePlayer(PlayerLocation, Distance);
}

// http://javierramello.com/index.php/tutorials/50-unreal-engine-4-hit-surface-detection
void AWindigoBaseCharacter::CreateFootstep(const FName& SocketName, bool bIsLeftfoot) 
{
	FRotator SocketRotation = GetActorRotation();;
	FVector SocketLocation = GetActorLocation();
	static FName FootstepTraceTag = FName(TEXT("FootstepTrace"));

	if (GetMesh())
	{
		if (GetMesh()->DoesSocketExist(SocketName))
		{
			GetMesh()->GetSocketWorldLocationAndRotation(SocketName, SocketLocation, SocketRotation);
		}
		else
		{
			UE_LOG(WindigoLog, Warning, TEXT("Footstep socket not found, defaulting to actor location"));
		}

		FHitResult Hit = TraceGround(SocketLocation);

		if (FootstepTemplate)
		{
			SocketRotation.Yaw = GetActorRotation().Yaw;

			AGameFootstepEffect* EffectActor = GetWorld()->SpawnActorDeferred<AGameFootstepEffect>(FootstepTemplate, SocketLocation, GetActorRotation(), this);
			if (EffectActor)
			{
				const UPhysicalMaterial* PhysicalMaterial = GetGroundPhysMaterial(Hit).Get();
				if (PhysicalMaterial)
				{
					EPhysicalSurface PhysSurface = PhysicalMaterial->SurfaceType;

					EffectActor->SoundComponent = GetRandomFootstepSound(PhysSurface);
					EffectActor->ParticleFX = GetRandomFootstepParticle(PhysSurface);
					EffectActor->DecalMaterial.Material = GetRandomFootstepDecal(PhysSurface, bIsLeftfoot);
					EffectActor->SurfaceHit = Hit;
					EffectActor->Lifespan = FootstepLifetime;
					EffectActor->bDebugDrawFootstepNormal = bDrawDecalNormals;

					UGameplayStatics::FinishSpawningActor(EffectActor, FTransform(SocketRotation, Hit.Location));
				}
			}
		}
	}

	CreateNoise(SocketLocation);
}

void AWindigoBaseCharacter::CreateNoise(const FVector& Location)
{
	NoiseEmitter->MakeNoise(this, 1.0f, Location);
}

//@todo BUG : Fails to get the PhysMaterial of landscape components
TWeakObjectPtr<class UPhysicalMaterial> AWindigoBaseCharacter::GetGroundPhysMaterial(const FHitResult& HitResult) const
{
	TWeakObjectPtr<class UPhysicalMaterial> PhysicalMaterial = NULL;

	AActor*	HitActor = HitResult.GetActor();
	if (HitActor)
	{
#if UE_BUILD_DEBUG
		//if (HitActor->IsA(AStaticMeshActor::StaticClass()))
		//{
		//	UE_LOG(WindigoLog, Log, TEXT("Trace hit static mesh : %s"), *HitActor->GetName());
		//}
		//else if (HitActor->IsA(ALandscape::StaticClass()))
		//{
		//	UE_LOG(WindigoLog, Log, TEXT("Trace hit landscape : %s"), *HitActor->GetName());
		//}
#endif

		PhysicalMaterial = HitResult.PhysMaterial;
	}

	return PhysicalMaterial;
}

FHitResult AWindigoBaseCharacter::TraceGround(const FVector& Location, bool bDrawTrace) const
{
	const float VerticalTraceDistance = 128;

	FCollisionQueryParams TraceParams( FName(TEXT("TraceGround")), false, GetOwner() );
	TraceParams.bTraceComplex = true;
	TraceParams.bReturnPhysicalMaterial = true;

	FHitResult HitResult(ForceInit);
	
	const FVector TraceStart = Location;
	const FVector TraceEnd = TraceStart - FVector(0, 0, VerticalTraceDistance);

	if (bDrawTrace)
	{
		GetWorld()->DebugDrawTraceTag = FName(TEXT("TraceGround"));
	}
	GetWorld()->LineTraceSingle(HitResult, TraceStart, TraceEnd, ECC_Pawn, TraceParams);

	return HitResult;
}

USoundCue* AWindigoBaseCharacter::GetRandomFootstepSound(EPhysicalSurface PhysicalSurface) const
{
	USoundCue* FootstepSound = NULL;

	if (FootstepInfo)
	{
		const UCharacterFootstepProperties* Info = FootstepInfo.GetDefaultObject();

		if (Info)
		{
			const auto& SurfaceFootstepSounds = Info->GetSurfaceSounds(PhysicalSurface);
			if (SurfaceFootstepSounds.Max() > 0)
			{
				FootstepSound = SurfaceFootstepSounds[FMath::RandRange(0, SurfaceFootstepSounds.Max() - 1)];
			}
			else /* No footstep sounds supplied for this surface type */
			{
				const FString SurfaceTypeString = FString::Printf(TEXT("SurfaceType%d"), (int32)PhysicalSurface);
				UE_LOG(WindigoLog, Warning, TEXT("GetRandomFootstep: Attempted to play footstep sounds %s, but none were found"), *SurfaceTypeString);
			}
		}

	}
	else /* No FootstepInfo instance*/
	{
		UE_LOG(WindigoLog, Error, TEXT("GetRandomFootstep: Can't access Footstep Info"));
	}

	return FootstepSound;
}

UParticleSystem* AWindigoBaseCharacter::GetRandomFootstepParticle(EPhysicalSurface PhysicalSurface) const
{
	UParticleSystem* FootstepFX = NULL;

	if (FootstepInfo)
	{
		const UCharacterFootstepProperties* Info = FootstepInfo.GetDefaultObject();

		if (Info)
		{
			const auto& SurfaceFootstepParticles = Info->GetSurfaceParticles(PhysicalSurface);
			if (SurfaceFootstepParticles.Max() > 0)
			{
				FootstepFX = SurfaceFootstepParticles[FMath::RandRange(0, SurfaceFootstepParticles.Max() - 1)];
			}
			else /* No footstep sounds supplied for this surface type */
			{
				const FString SurfaceTypeString = FString::Printf(TEXT("SurfaceType%d"), (int32)PhysicalSurface);
				UE_LOG(WindigoLog, Warning, TEXT("GetRandomFootstep: Attempted to load particle effects for surfacetype %s, but none were found"), *SurfaceTypeString);
			}
		}

	}
	else /* No FootstepInfo instance*/
	{
		UE_LOG(WindigoLog, Error, TEXT("GetRandomFootstep: Can't access Footstep Properties"));
	}

	return FootstepFX;
}

UMaterial* AWindigoBaseCharacter::GetRandomFootstepDecal(EPhysicalSurface PhysicalSurface, bool bIsLeft) const
{
	UMaterial* FootstepDecal = NULL;

	if (FootstepInfo)
	{
		const UCharacterFootstepProperties* Info = FootstepInfo.GetDefaultObject();

		if (Info)
		{
			const auto& SurfaceFootstepDecals = Info->GetSurfaceDecals(PhysicalSurface, bIsLeft);
			if (SurfaceFootstepDecals.Max() > 0)
			{
				FootstepDecal = SurfaceFootstepDecals[FMath::RandRange(0, SurfaceFootstepDecals.Max() - 1)];
			}
			else /* No footstep sounds supplied for this surface type */
			{
				const FString SurfaceTypeString = FString::Printf(TEXT("SurfaceType%d"), (int32)PhysicalSurface);
				UE_LOG(WindigoLog, Warning, TEXT("GetRandomFootstep: Attempted to load footstep decals for surfacetype %s, but none were found"), *SurfaceTypeString);
			}
		}
	}
	else /* No FootstepInfo instance*/
	{
		UE_LOG(WindigoLog, Error, TEXT("GetRandomFootstep: Can't access Footstep Properties"));
	}

	return FootstepDecal;
}

void AWindigoBaseCharacter::SimulateLeftFootstep()
{
	CreateFootstep(FName("LeftFootstepSocket"), true);
	OnLeftFootstepEvent();
}

void AWindigoBaseCharacter::SimulateRightFootstep()
{
	CreateFootstep(FName("RightFootstepSocket"), false);
	OnRightFootstepEvent();
}

void AWindigoBaseCharacter::Tick(float DeltaSeconds)
{
	if (bDrawLastKnownSound)
	{
		// Visualise hearing range
		FVector ActorLocation = GetActorLocation();
		FHitResult Hit = TraceGround(ActorLocation);
		const float HeightDifference = ActorLocation.Z - Hit.ImpactPoint.Z;
		FVector NewLocation = ActorLocation - FVector(0, 0, (HeightDifference + GetDefaultHalfHeight()));

		// Draw radius of unoccluded hearing
		DrawDebugSphere(GetWorld(),
			NewLocation,
			CharacterSenses->LOSHearingThreshold,
			HearingRangeSegments,
			DebugUnoccludedHearingRangeColour,
			false,
			HearingRangeLifetime,
			0);

		// Draw radius of occluded hearing
		DrawDebugSphere(GetWorld(),
			NewLocation,
			CharacterSenses->HearingThreshold,
			HearingRangeSegments,
			DebugOccludedHearingRangeColour,
			false,
			HearingRangeLifetime,
			0);
	}
}

