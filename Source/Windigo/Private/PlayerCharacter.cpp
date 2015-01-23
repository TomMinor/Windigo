// Fill out your copyright notice in the Description page of Project Settings.  

#include "Windigo.h"
#include "PlayerCharacter.h"


//================================== Camera CVars ==================================
static TAutoConsoleVariable<float> CVarCameraInterpSpeed(
	TEXT("WindigoCharacter.CameraInterpSpeed"),
	8.0f,
	TEXT("How quickly the camera will smoothly change to the desired camera view\n"),
	ECVF_Cheat);

static TAutoConsoleVariable<float> CVarBoomInterpSpeed(
	TEXT("WindigoCharacter.CameraBoomInterpSpeed"),
	8.0f,
	TEXT("How quickly the camera Boom will smoothly change to the desired position\n"),
	ECVF_Cheat);

static TAutoConsoleVariable<float> CVarActorInterpSpeed(
	TEXT("WindigoCharacter.ActorInterpSpeed"),
	32.0f,
	TEXT("How quickly the actor will smoothly change to the desired rotation\n"),
	ECVF_Cheat);

static TAutoConsoleVariable<float> CVarCameraRollInterpSpeed(
	TEXT("WindigoCharacter.CameraRollInterpSpeed"),
	2.0f,
	TEXT("How quickly the camera roll will interpolate\n"),
	ECVF_Cheat);

static TAutoConsoleVariable<float> CVarCameraMaxRoll(
	TEXT("WindigoCharacter.CameraMaxLeanRoll"),
	5.0f,
	TEXT("The maximum camera rotation as the player leans.\n"),
	ECVF_Cheat);

static TAutoConsoleVariable<float> CVarCameraMaxLean(
	TEXT("WindigoCharacter.CameraMaxLean"),
	64.0f,
	TEXT("The maximum distance the player can lean.\n"),
	ECVF_Cheat);

static TAutoConsoleVariable<float> CVarCameraMaxPeakHeight(
	TEXT("WindigoCharacter.CameraMaxPeakHeight"),
	16.0f,
	TEXT("The maximum distance the player can lean up.\n"),
	ECVF_Cheat);

static TAutoConsoleVariable<float> CVarCameraMinPeakHeight(
	TEXT("WindigoCharacter.CameraMinPeakHeight"),
	-0.5f,
	TEXT("The minimum distance the player can lean up.\n"),
	ECVF_Cheat);

APlayerCharacter::APlayerCharacter(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer), fSprintSpeed(600), fWalkSpeed(140)
{
	// Create CameraComp
	FirstPersonCameraComponent = ObjectInitializer.CreateDefaultSubobject<UCameraComponent>(this, TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->AttachParent = GetCapsuleComponent();

	// Move camera to eye position
	FirstPersonCameraComponent->RelativeLocation = FVector(16.f, 4.f, BaseEyeHeight + 20.f);

	// Disallow pawn to control rotation of camera
	FirstPersonCameraComponent->bUsePawnControlRotation = false;

	LeanCameraBoom = ObjectInitializer.CreateDefaultSubobject<USpringArmComponent>(this, TEXT("LeanCameraBoom"));
	LeanCameraBoom->AttachTo(RootComponent);
	LeanCameraBoom->TargetArmLength = 10.0f; // The camera follows at this distance behind the character	
	LeanCameraBoom->bUsePawnControlRotation = false; // Rotate the arm based on the controller
	LeanCameraBoom->bDoCollisionTest = true;
	LeanCameraBoom->TargetOffset = FirstPersonCameraComponent->GetComponentLocation();

	DesiredBoomTargetLocation = LeanCameraBoom->TargetOffset;

	FirstPersonCameraComponent->AttachTo(LeanCameraBoom, USpringArmComponent::SocketName);

	GetCharacterMovement()->MaxWalkSpeed = fWalkSpeed;
	GetCharacterMovement()->MaxWalkSpeedCrouched = fWalkSpeed * 0.25;

	GetCharacterMovement()->FallingLateralFriction = 10.0f;
	GetCharacterMovement()->AirControl = 0.1f;
	GetCharacterMovement()->bMaintainHorizontalGroundVelocity = false; // If false, then walking movement maintains velocity magnitude parallel to the ramp surface.

	bSetToActorRotation = false;
	bIsSprinting = false;
	bRightClick = false;

	HeadSocket = FName("cameraView");

	/*DesiredActorRotation = FirstPersonCameraComponent->GetComponentRotation();
	DesiredViewRotation = DesiredActorRotation;
	DesiredViewLocation = FirstPersonCameraComponent->GetComponentLocation();
*/
	InitialMeshYaw = 0.0f;
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void APlayerCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// Hide the head mesh in 1st person (we're drawing the visor directly on the screen) 
	//if (bIsFirstPerson)
	{
		GetMesh()->HideBoneByName(TEXT("head"), PBO_None);
	}

	if (GetMesh()->DoesSocketExist(HeadSocket))
	{
		FVector HeadPosition;
		FRotator HeadRotation;
		GetMesh()->GetSocketWorldLocationAndRotation(HeadSocket, HeadPosition, HeadRotation);
		FirstPersonCameraComponent->SetWorldLocation(HeadPosition);
	}

	DesiredActorRotation = FirstPersonCameraComponent->GetComponentRotation();
	DesiredViewRotation = DesiredActorRotation;
	DesiredViewLocation = FirstPersonCameraComponent->GetComponentLocation();

	InitialMeshYaw = GetMesh()->GetComponentRotation().Yaw;
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* InputComponent)
{
	//Register key axis bindings
	InputComponent->BindAxis("MoveForward", this, &APlayerCharacter::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &APlayerCharacter::MoveRight);

	InputComponent->BindAxis("Yaw", this, &APlayerCharacter::ViewYaw);
	InputComponent->BindAxis("Pitch", this, &APlayerCharacter::ViewPitch);

	// Register key action bindings
	InputComponent->BindAction("Jump", IE_Pressed, this, &APlayerCharacter::OnStartJump);
	InputComponent->BindAction("Jump", IE_Released, this, &APlayerCharacter::OnStopJump);

	InputComponent->BindAction("Crouch", IE_Pressed, this, &APlayerCharacter::OnBeginCrouch);
	
	InputComponent->BindAction("Wave", IE_Pressed, this, &APlayerCharacter::OnWave);
	InputComponent->BindAction("Point", IE_Pressed, this, &APlayerCharacter::OnPoint);
	InputComponent->BindAction("Shout", IE_Pressed, this, &APlayerCharacter::OnShout);

	InputComponent->BindAction("Sprint", IE_Pressed, this, &APlayerCharacter::OnStartSprint);
	InputComponent->BindAction("Sprint", IE_Released, this, &APlayerCharacter::OnStopSprint);

	InputComponent->BindAction("RightClick", IE_Pressed, this, &APlayerCharacter::OnStartRightClick);
	InputComponent->BindAction("RightClick", IE_Released, this, &APlayerCharacter::OnStopRightClick);
}

void APlayerCharacter::OnStartRightClick()
{
	bRightClick = true;
}

void APlayerCharacter::OnStopRightClick()
{
	bRightClick = false;
	ViewYaw(0.0f); /* Force the yaw code to reset the camera position */
}

void APlayerCharacter::MoveForward(float val)
{
	// @todo If the player sprints backwards, automatically interp their view to looking behind them
	if ((Controller != NULL) && (val != 0.0f))
	{
		//Which way is forward?
		FRotator rotation = GetViewRotation();

		//Limit pitch during walk/fall
		if (GetCharacterMovement()->IsMovingOnGround() || GetCharacterMovement()->IsFalling())
		{
			rotation.Pitch = 0.0f;
		}

		//Apply movement in the direction we're facing
		FVector direction = FRotationMatrix(rotation).GetScaledAxis(EAxis::X);

		AddMovementInput(direction, val);
	}
}

void APlayerCharacter::MoveRight(float val)
{
	if ((Controller != NULL) && (val != 0.0f))
	{
		//Which way is right?
		const FRotator rotation = GetViewRotation();
		const FVector direction = FRotationMatrix(rotation).GetScaledAxis(EAxis::Y);

		//Apply movement in that direction
		AddMovementInput(direction, val);
	}
}

void APlayerCharacter::ViewYaw(float Val)
{
	const float LeanMaxRoll = CVarCameraMaxRoll.GetValueOnGameThread();
	const float MaxHeadRotation = GetActorRotation().Yaw + LeanMaxRoll;
	const float MinHeadRotation = GetActorRotation().Yaw - LeanMaxRoll;

	const bool bIsMovingRight = (Val < 0.0f);
	const bool bIsMovingLeft = !bIsMovingRight;
	float ExpectedViewYaw = GetActorRotation().Clamp().Yaw + Val;

#if UE_BUILD_DEBUG
	float camYaw = FirstPersonCameraComponent->GetComponentTransform().GetRotation().Rotator().Yaw;
	if (GEngine)
	{
		FString output = TEXT("Yaw = ") + FString::SanitizeFloat(camYaw);
		GEngine->AddOnScreenDebugMessage(255, 1.f, FColor::White, output);
	}


	if (GEngine)
	{
		FString output = TEXT("Head = ") + FString::SanitizeFloat(GetActorRotation().Yaw);
		GEngine->AddOnScreenDebugMessage(100, 10.f, FColor::Blue, output);

		output = TEXT("View = ") + FString::SanitizeFloat(DesiredViewRotation.Yaw);
		GEngine->AddOnScreenDebugMessage(101, 10.f, FColor::Black, output);

		output = TEXT("Max = ") + FString::SanitizeFloat(MaxHeadRotation);
		GEngine->AddOnScreenDebugMessage(102, 10.f, FColor::Cyan, output);

		output = TEXT("Min = ") + FString::SanitizeFloat(MinHeadRotation);
		GEngine->AddOnScreenDebugMessage(103, 10.f, FColor::Magenta, output);
	}
#endif

	
	//@todo extend capsule so windigo can see us?
	if (bRightClick)
	{
		if (bIsSprinting && GetMovementComponent()->IsMovingOnGround()) /* Move camera separately from body while running */
		{
			
			/*if (bIsMovingLeft && (ExpectedViewYaw < MaxHeadRotation))
			{
				DesiredViewRotation.Yaw += Val;
			}

			if (bIsMovingRight && (ExpectedViewYaw > MinHeadRotation))
			{
				DesiredViewRotation.Yaw += Val;
			}*/

			DesiredViewRotation.Yaw += Val;
			bSetToActorRotation = true;
		}
		else if (!FMath::IsNearlyZero(Val) ) /* Cover mode */
		{
			const float MaxLean = CVarCameraMaxLean.GetValueOnGameThread();;
			const float LeanRollInterp = CVarCameraRollInterpSpeed.GetValueOnGameThread();

			/* We need to invert the value */
			const float YOffset = -Val;																 
			
			/* Fix rotation to look forward */
			const float Yaw = InitialMeshYaw + FirstPersonCameraComponent->GetComponentRotation().Yaw; // @bug InitialYaw is incorrect if the playerstart is rotated
			
			//const float Yaw = FirstPersonCameraComponent->GetComponentRotation().Yaw;
			/* Are we learning right or left? */
			const float RollRotation = bIsMovingLeft ? 5.f : -5.f;

			const FVector Offset = FVector(YOffset * FMath::Cos(FMath::DegreesToRadians(Yaw)),
										   YOffset * FMath::Sin(FMath::DegreesToRadians(Yaw)),
										   0.f);

			//FVector Offset = FRotationMatrix(FRotator(0.f, FirstPersonCameraComponent->GetComponentRotation().Yaw, 0.f)).GetScaledAxis(EAxis::Y);
			const FVector ExpectedTargetLocation = DesiredBoomTargetLocation + Offset;

			/* Only allow leaning if we aren't expecting to go above the lean limit */
			if (ExpectedTargetLocation.Size() < MaxLean)
			{
				DesiredBoomTargetLocation += Offset;
				DesiredViewRotation.Roll = FMath::FInterpTo(DesiredViewRotation.Roll, RollRotation, GetWorld()->GetDeltaSeconds(), LeanRollInterp);
			}
		}
	}
	else /* Walking/sprinting normally */
	{
		/* Interp back to no roll or boom offset when we release the rmb */
		DesiredBoomTargetLocation = FVector(0.f, 0.f, 0.f);
		DesiredViewRotation.Roll = 0;

		/* Update desired view rotation */
		DesiredActorRotation.Yaw += Val;

		if (bSetToActorRotation)
		{
			DesiredViewRotation.Yaw = FMath::Fmod(DesiredActorRotation.Yaw, 360.f);
			bSetToActorRotation = false;
		}
		else
		{
			DesiredViewRotation.Yaw += Val;
		}
	}
	//Super::AddControllerYawInput(Val);
}

void APlayerCharacter::ViewPitch(float Val)
{
#if UE_BUILD_DEBUG
	float camPitch = FirstPersonCameraComponent->GetComponentTransform().GetRotation().Rotator().Pitch;
	if (GEngine)
	{
		FString output = TEXT("Pitch = ") + FString::SanitizeFloat(camPitch);
		GEngine->AddOnScreenDebugMessage(256, 1.f, FColor::Yellow, output);
	}
#endif

	if (bRightClick)
	{
		if (bIsSprinting && GetMovementComponent()->IsMovingOnGround()) /* Move camera separately from body while running */
		{
			/* Change just the cam yaw without affecting the rotation of the actor's mesh */
			DesiredViewRotation.Pitch -= Val;
		}
		else if (!FMath::IsNearlyZero(Val)) /* Cover mode peak - Can only peak up, not down */
		{
			const float ZOffset = -Val;		/* We need to invert the value */

			//@todo Make these UPROPERTY
			const float MaxPeakHeight = CVarCameraMaxPeakHeight.GetValueOnGameThread();
			const float MinPeakHeight = CVarCameraMinPeakHeight.GetValueOnGameThread();
			
			const FVector Offset = FVector(0.f, 0.f, ZOffset);
			const FVector ExpectedTargetLocation = DesiredBoomTargetLocation + Offset;

			/* Only allow peaking up to MaxHeight */
			if ((FMath::Abs(ExpectedTargetLocation.Z) < MaxPeakHeight) &&
				(FMath::Abs(ExpectedTargetLocation.Z) > MinPeakHeight))
			{
				DesiredBoomTargetLocation += Offset;
			}
		}
	}
	else /* Walking/sprinting normally */
	{
		/* Update desired view rotation */
		DesiredViewRotation.Pitch -= Val;
	}
}

void APlayerCharacter::OnStartJump()
{
	bPressedJump = true;
	OnJumped();
}

void APlayerCharacter::OnStopJump()
{
	bPressedJump = false;
}

void APlayerCharacter::OnBeginCrouch()
{
	//@todo Interp crouch
	// bCanCrouch is enabled in CharacterMovementComponent in blueprint
	if (bIsCrouching)
	{
		UnCrouch();
		bIsCrouching = false;
	}
	else
	{
		Crouch();
		bIsCrouching = true;
	}
	
}

void APlayerCharacter::OnStopCrouch()
{
	//unused
}

//void APlayerCharacter::CrouchImpl(float DeltaTime)
//{
//	const float TargetBEH = bIsCrouching ? CrouchedEyeHeight : DecBEH;
//	const float TargetCapsuleSize = bIsCrouching ? CharacterMovement->CrouchedHalfHeight : DecCapsHeight;
//	if (Controller != NULL)
//	{
//		BaseEyeHeight = FMath::FInterpTo(BaseEyeHeight, TargetBEH, DeltaTime, 10.0f);
//		CapsuleComponent->SetCapsuleHalfHeight(FMath::FInterpTo(CapsuleComponent->GetUnscaledCapsuleHalfHeight(), TargetCapsuleSize, DeltaTime, 10.0f), true);
//		// Dist and DeltaMovCaps are used for the interpolation value added to RelativeLocation.Z
//		const float Dist = TargetCapsuleSize - CapsuleComponent->GetUnscaledCapsuleHalfHeight();
//		const float DeltaMovCaps = Dist*FMath::Clamp<float>(DeltaTime*10.0f, 0.f, 1.f);
//		CapsuleComponent->SetRelativeLocation(FVector(CapsuleComponent->RelativeLocation.X, CapsuleComponent->RelativeLocation.Y, (CapsuleComponent->RelativeLocation.Z + DeltaMovCaps)), true);
//	}
//}

void APlayerCharacter::Landed(const FHitResult& Hit)
{
	const float MaxHitRotation = 45.f;

	FRandomStream rand;
	rand.GenerateNewSeed();

	DesiredViewRotation += FRotator(0.f, 0.f, rand.FRandRange(-MaxHitRotation, MaxHitRotation));
	DesiredViewLocation -= FVector(0.f, 0.f, rand.FRandRange(-20.f, -8.f));

	NoiseEmitter->MakeNoise(this, 0.8f, GetActorLocation());

	OnLanded(Hit);
}

void APlayerCharacter::OnWave()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Cyan, TEXT("Wave Event"));
	}
	OnWaveEvent();
}

void APlayerCharacter::OnPoint()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Cyan, TEXT("Point Event"));
	}
	OnPointEvent();
}

void APlayerCharacter::OnShout()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Cyan, TEXT("Shout Event"));
	}
	NoiseEmitter->MakeNoise(this, 1.f, GetActorLocation());
	OnShoutEvent();
}

void APlayerCharacter::OnStartSprint()
{
	bIsSprinting = true;
	GetCharacterMovement()->MaxWalkSpeed = fSprintSpeed;
}

void APlayerCharacter::OnStopSprint()
{
	bIsSprinting = false;
	GetCharacterMovement()->MaxWalkSpeed = fWalkSpeed;
}


bool APlayerCharacter::WillCollideWithWorld(const FVector _origin, const FVector _current, const FVector _new, FVector& _result, const float _radiusScale, bool bDrawTrace)
{
	// Single Line
	FCollisionQueryParams TraceParams(FName(TEXT("TraceWorld")), false, GetOwner());
	TraceParams.bTraceComplex = true;
	TraceParams.AddIgnoredActor(this);

	FHitResult HitResult(ForceInit);

	FVector direction = _new - _current;
	direction *= _radiusScale;

	if (bDrawTrace)
	{
		GetWorld()->DebugDrawTraceTag = FName(TEXT("TraceWorld"));
	}
	GetWorld()->LineTraceSingle(HitResult, _origin, _origin + direction, ECC_Pawn, TraceParams);

	_result = HitResult.Location;

	if (HitResult.bBlockingHit)
	{
		return true;
	}
	else
	{
		FCollisionQueryParams TraceParams(FName(TEXT("Spherical Trace")), true, this);
		TraceParams.bTraceComplex = true;
		//TraceParams.bTraceAsyncScene = true;
		TraceParams.bReturnPhysicalMaterial = false;
		//Ignore Actors
		TraceParams.AddIgnoredActor(this);

		//Re-initialize hit info
		FHitResult HitOut = FHitResult(ForceInit);

		GetWorld()->SweepSingle(
			HitOut,
			_origin, 
			_origin,
			FQuat(),
			ECC_Pawn,
			FCollisionShape::MakeSphere(_radiusScale),
			TraceParams
			);

		_result = HitResult.Location;

		if (HitOut.bBlockingHit || HitOut.bStartPenetrating)
		{
			return true;
		}
	}

	return false;
}


void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// =============================== FPS Cam Rotation Smoothing ===============================	
	const float CamInterpSpeed = CVarCameraInterpSpeed.GetValueOnGameThread();
	const FRotator FPSCamRotation = FMath::RInterpTo(FirstPersonCameraComponent->GetComponentRotation(), DesiredViewRotation, DeltaTime, CamInterpSpeed);
	FirstPersonCameraComponent->SetWorldRotation(FPSCamRotation);

	// =============================== Actor Rotation Smoothing ===============================	
	const float ActorInterpSpeed = CVarActorInterpSpeed.GetValueOnGameThread();
	const FRotator ActorRotation = FMath::RInterpTo(GetActorRotation(), DesiredActorRotation, DeltaTime, ActorInterpSpeed);
	ClientSetRotation(ActorRotation);
	
	// =============================== View Bob ===============================	
	FVector ViewBob(0.f, 0.f, 0.f);
	const float ViewBobScalar = bIsSprinting ? 1.0f : 4.0f;
	if (GetMesh()->DoesSocketExist(HeadSocket))
	{
		FVector HeadPosition;
		FRotator HeadRotation;
		GetMesh()->GetSocketWorldLocationAndRotation(HeadSocket, HeadPosition, HeadRotation);

		ViewBob = HeadPosition - FirstPersonCameraComponent->GetComponentLocation();
	}

	//static float lastTime = 0.0f;
	//static FVector oldBoomPos = DesiredBoomTargetLocation;
	//if (GetWorld()->TimeSince(lastTime) > 1.0f)
	//{
	//	lastTime = GetWorld()->TimeSeconds;
	//	oldBoomPos = DesiredBoomTargetLocation;
	//}
	
	// =============================== Lean / Camera Boom Target Offset Smoothing ===============================	
	const float BoomInterpSpeed = CVarBoomInterpSpeed.GetValueOnGameThread() ;
	const FVector IntendedLocation = (DesiredBoomTargetLocation * ViewBobScalar) + ViewBob;
	FVector BoomTargetLocation = FMath::VInterpTo(LeanCameraBoom->TargetOffset, IntendedLocation, DeltaTime, BoomInterpSpeed);

	FVector hitLocation;
	bool isCollidingWithWorld = WillCollideWithWorld(FirstPersonCameraComponent->GetComponentLocation(), 
													 BoomTargetLocation, 
													 IntendedLocation, 
													 hitLocation,
													 1.f, 
													 true);
	bool isMovingAway = LeanCameraBoom->TargetOffset.Size() < DesiredBoomTargetLocation.Size() - 3.f;
	//hitLocation -= BoomTargetLocation;

	/*if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(64, -1, FColor::Cyan, FString::Printf(TEXT("From : (%f, %f, %f)"),  LeanCameraBoom->TargetOffset.X, LeanCameraBoom->TargetOffset.Y, LeanCameraBoom->TargetOffset.Z));
		GEngine->AddOnScreenDebugMessage(63, -1, FColor::Yellow, FString::Printf(TEXT("To : (%f, %f, %f)"),  IntendedLocation.X, IntendedLocation.Y, IntendedLocation.Z));
		GEngine->AddOnScreenDebugMessage(65, -1, FColor::Blue, FString::Printf(TEXT("Is Moving Away : %s"),  isMovingAway ? TEXT("True") : TEXT("False") ));
		GEngine->AddOnScreenDebugMessage(66, -1, FColor::Red, FString::Printf(TEXT("Is Colliding : %s"),	 isCollidingWithWorld ? TEXT("True") : TEXT("False")));
		GEngine->AddOnScreenDebugMessage(67, -1, FColor::Yellow, FString::Printf(TEXT("Hit : (%f, %f, %f)"), hitLocation.X, hitLocation.Y, hitLocation.Z));
	}*/
	
	if (!isCollidingWithWorld)
	{
		LeanCameraBoom->TargetOffset = BoomTargetLocation;
	}

	//if (!isMovingAway || !isCollidingWithWorld)
	//{
	//	
	//}
}