// Fill out your copyright notice in the Description page of Project Settings.  

#include "Windigo.h"
#include "PlayerCharacter.h"


// CVars
static TAutoConsoleVariable<int32> CVarCameraInterpSpeed(
	TEXT("WindigoCharacter.CameraInterpSpeed"),
	64.0f,
	TEXT("How quickly the camera will smoothly change to the desired camera view\n"),
	ECVF_Cheat);

static TAutoConsoleVariable<int32> CVarActorInterpSpeed(
	TEXT("WindigoCharacter.ActorInterpSpeed"),
	32.0f,
	TEXT("How quickly the actor will smoothly change to the desired rotation\n"),
	ECVF_Cheat);


APlayerCharacter::APlayerCharacter(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer), fSprintSpeed(600), fWalkSpeed(140)
{
	// Create CameraComponent
	FirstPersonCameraComponent = ObjectInitializer.CreateDefaultSubobject<UCameraComponent>(this, TEXT("FirstPersonCamera"));
	//FirstPersonCameraComponent->AttachParent = GetCapsuleComponent();

	// Move camera to eye position
	FirstPersonCameraComponent->RelativeLocation = FVector(16.f, 4.f, BaseEyeHeight + 20.f);

	// Allow pawn to control rotation of camera
	FirstPersonCameraComponent->bUsePawnControlRotation = false;

	LeanCameraBoom = ObjectInitializer.CreateDefaultSubobject<USpringArmComponent>(this, TEXT("LeanCameraBoom"));
	LeanCameraBoom->AttachTo(RootComponent);
	LeanCameraBoom->TargetArmLength = 10.0f; // The camera follows at this distance behind the character	
	LeanCameraBoom->bUsePawnControlRotation = false; // Rotate the arm based on the controller
	//LeanCameraBoom->SetAbsolute(false, true);

	DesiredBoomTargetLocation = LeanCameraBoom->TargetOffset;

	FirstPersonCameraComponent->AttachTo(LeanCameraBoom, USpringArmComponent::SocketName);

	GetCharacterMovement()->MaxWalkSpeed = fWalkSpeed;
	GetCharacterMovement()->MaxWalkSpeedCrouched = fWalkSpeed * 0.25;

	GetCharacterMovement()->FallingLateralFriction = 10.0f;
	GetCharacterMovement()->AirControl = 0.1f;
	GetCharacterMovement()->bMaintainHorizontalGroundVelocity = false; // If false, then walking movement maintains velocity magnitude parallel to the ramp surface.

	bIsSprinting = false;
	bRightClick = false;

	DesiredActorRotation = FirstPersonCameraComponent->GetComponentRotation();
	DesiredViewRotation = DesiredActorRotation;
	DesiredViewLocation = FirstPersonCameraComponent->GetComponentLocation();

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

	if (GetMesh()->DoesSocketExist(FName("cameraView")))
	{
		FVector HeadPosition;
		FRotator HeadRotation;
		GetMesh()->GetSocketWorldLocationAndRotation(FName("cameraView"), HeadPosition, HeadRotation);
		FirstPersonCameraComponent->SetWorldLocation(HeadPosition);
	}

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
	InputComponent->BindAction("Crouch", IE_Released, this, &APlayerCharacter::OnStopCrouch);

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
		FName SocketName("cameraView");
		//Which way is forward?
		FRotator rotation = GetViewRotation();

		//Limit pitch during walk/fall
		if (GetCharacterMovement()->IsMovingOnGround() || GetCharacterMovement()->IsFalling())
		{
			rotation.Pitch = 0.0f;

			//if (GetMesh()->DoesSocketExist(SocketName))
			//{
			//	FVector HeadPosition;
			//	FRotator HeadRotation;
			//	GetMesh()->GetSocketWorldLocationAndRotation(SocketName, HeadPosition, HeadRotation);
			//	HeadRotation.Roll = FirstPersonCameraComponent->GetComponentRotation().Roll;
			//	HeadPosition.X = GetActorLocation().X;
			//	HeadPosition.Y = GetActorLocation().Y;

			//	/*DesiredViewRotation = HeadRotation;
			//	DesiredViewLocation = HeadPosition;*/

			//	if (GEngine)
			//	{
			//		FString output = FString::Printf(TEXT("Desired Location = %f %f %f"), DesiredViewLocation.X, DesiredViewLocation.Y, DesiredViewLocation.Z);
			//		GEngine->AddOnScreenDebugMessage(235, 1.f, FColor::White, output);

			//		const FVector a = HeadPosition;
			//			output = FString::Printf(TEXT("Actual Location = %f %f %f"), a.X, a.Y, a.Z);
			//		GEngine->AddOnScreenDebugMessage(234, 1.f, FColor::White, output);
			//	}
			//}
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
	float camYaw = FirstPersonCameraComponent->GetComponentTransform().GetRotation().Rotator().Yaw;
	if (GEngine)
	{
		FString output = TEXT("Yaw = ") + FString::SanitizeFloat(camYaw);
		GEngine->AddOnScreenDebugMessage(255, 1.f, FColor::White, output);
	}

	//@todo extend capsule so windigo can see us?
	if (bRightClick)
	{
		if (bIsSprinting && GetMovementComponent()->IsMovingOnGround()) /* Move camera separately from body while running */
		{
			/* Change just the cam yaw without affecting the rotation of the actor's mesh */
			DesiredViewRotation.Yaw += Val;
		}
		else if (!FMath::IsNearlyZero(Val) ) /* Cover mode */
		{
			const float MaxLean = 64.f;
			const float LeanRollInterp = 2.0f;

			/* We need to invert the value */
			const float YOffset = -Val;																 
			/* Fix rotation to look forward */
			const float Yaw = InitialMeshYaw + FirstPersonCameraComponent->GetComponentRotation().Yaw;
			/* Are we learning right or left? */
			const float RollRotation = (YOffset < 0.0f) ? 5.f : -5.f;									 

			const FVector Offset = FVector(YOffset * FMath::Cos(FMath::DegreesToRadians(Yaw)),
										   YOffset * FMath::Sin(FMath::DegreesToRadians(Yaw)),
										   0.f);
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
		DesiredViewRotation.Yaw += Val;
		DesiredActorRotation.Yaw += Val;
	}
	//Super::AddControllerYawInput(Val);
}

void APlayerCharacter::ViewPitch(float Val)
{
	float camPitch = FirstPersonCameraComponent->GetComponentTransform().GetRotation().Rotator().Pitch;
	if (GEngine)
	{
		FString output = TEXT("Pitch = ") + FString::SanitizeFloat(camPitch);
		GEngine->AddOnScreenDebugMessage(256, 1.f, FColor::Yellow, output);
	}

	if (!FMath::IsNearlyZero(Val))
	{
		//FRotator CamRotation = FirstPersonCameraComponent->GetComponentRotation();
		//CamRotation.Pitch -= Val;

		//DesiredViewRotation = CamRotation;
		DesiredViewRotation.Pitch -= Val;
		//DesiredActorRotation.Pitch -= Val;
		//GetController()->ClientSetRotation(CamRotation);

		//	Super::AddControllerPitchInput(Val);
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
	// bCanCrouch is enabled in CharacterMovementComponent in blueprint
	Crouch();
}

void APlayerCharacter::OnStopCrouch()
{
	UnCrouch();
}

void APlayerCharacter::Landed(const FHitResult& Hit)
{
	//TODO
	// Add camera shake
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

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//LeanCameraBoom->SetWorldRotation(FirstPersonCameraComponent->GetComponentRotation());

	const float CamInterpSpeed = CVarCameraInterpSpeed.GetValueOnGameThread();
	const FRotator FPSCamRotation = FMath::RInterpTo(FirstPersonCameraComponent->GetComponentRotation(), DesiredViewRotation, DeltaTime, CamInterpSpeed);
	FirstPersonCameraComponent->SetWorldRotation(FPSCamRotation);
	
	const float ActorInterpSpeed = CVarActorInterpSpeed.GetValueOnGameThread();
	const FRotator ActorRotation = FMath::RInterpTo(GetActorRotation(), DesiredActorRotation, DeltaTime, ActorInterpSpeed);
	ClientSetRotation(ActorRotation);

	const float BoomInterpSpeed = 8.f;
	const FVector BoomTargetLocation = FMath::VInterpTo(LeanCameraBoom->TargetOffset, DesiredBoomTargetLocation, DeltaTime, BoomInterpSpeed);
	LeanCameraBoom->TargetOffset = BoomTargetLocation;

	//if (bIsSprinting)
	//{
	//	FVector HeadPosition;
	//	if (GetMesh()->DoesSocketExist(FName("cameraView")))
	//	{
	//		
	//		FRotator HeadRotation;
	//		GetMesh()->GetSocketWorldLocationAndRotation(FName("cameraView"), HeadPosition, HeadRotation);
	//		HeadRotation.Roll = FirstPersonCameraComponent->GetComponentRotation().Roll;
	//		HeadPosition.X = GetActorLocation().X;
	//		HeadPosition.Y = GetActorLocation().Y;

	//		/*HeadPosition = */

	//		//DesiredViewRotation = HeadRotation;
	//	}

	//	const float CameraLocationInterpSpeed = 2.f;
	//	const FVector CameraLocation = FMath::VInterpTo(FirstPersonCameraComponent->GetComponentLocation(), DesiredViewLocation, DeltaTime, CameraLocationInterpSpeed);
	//	FirstPersonCameraComponent->SetWorldLocation(CameraLocation);

	//	if (GEngine)
	//	{
	//		FString output = FString::Printf(TEXT("Desired Location = %f %f %f"), DesiredViewLocation.X, DesiredViewLocation.Y, DesiredViewLocation.Z);
	//		GEngine->AddOnScreenDebugMessage(235, 1.f, FColor::White, output);

	//		const FVector a = HeadPosition;
	//		output = FString::Printf(TEXT("Actual Location = %f %f %f"), a.X, a.Y, a.Z);
	//		GEngine->AddOnScreenDebugMessage(234, 1.f, FColor::White, output);
	//	}
	//}
}



void APlayerCharacter::TestCommand(float val)
{
	UE_LOG(WindigoLog, Log, TEXT("OUTPUT TEST %f"), val);
}