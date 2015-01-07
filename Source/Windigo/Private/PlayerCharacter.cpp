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

	GetCharacterMovement()->MaxWalkSpeed = fWalkSpeed;
	GetCharacterMovement()->MaxWalkSpeedCrouched = fWalkSpeed * 0.25;

	GetCharacterMovement()->FallingLateralFriction = 10.0f;
	GetCharacterMovement()->AirControl = 0.1f;
	GetCharacterMovement()->bMaintainHorizontalGroundVelocity = false; // If false, then walking movement maintains velocity magnitude parallel to the ramp surface.

	bIsSprinting = false;
	bRightClick = false;

	DesiredActorRotation = DesiredViewRotation = FirstPersonCameraComponent->GetComponentRotation();
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

		//if (bIsSprinting && val < 0.0f)
		//{
		//	//val = FMath::Abs(val);
		//	direction = -direction;
		//}

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

	if (!FMath::IsNearlyZero(Val))
	{
		FRotator CamRotation = FirstPersonCameraComponent->GetComponentRotation();
		if (bRightClick)
		{
			//CamRotation.Yaw += Val;
			//DesiredViewRotation = GetActorRotation() + FRotator(0.f, 180.f, 0.f);

			if (bIsSprinting && GetMovementComponent()->IsMovingOnGround()) /* Move camera separately from body while running */
			{

			}
			else /* Cover mode */
			{

			}
		}
		else /* Walking/sprinting normally */
		{
			/*CamRotation.Yaw += Val / 2.0;
			GetController()->ClientSetRotation(CamRotation);
			DesiredViewRotation = CamRotation;*/

			DesiredViewRotation.Yaw += Val;
			DesiredActorRotation.Yaw += Val;
		}
	}

	//FirstPersonCameraComponent->SetWorldRotation(CamRotation);
	//DesiredViewRotation = CamRotation;
	
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
		FRotator CamRotation = FirstPersonCameraComponent->GetComponentRotation();
		CamRotation.Pitch -= Val;

		//DesiredViewRotation = CamRotation;
		DesiredViewRotation.Pitch -= Val;
		DesiredActorRotation.Pitch -= Val;
		//GetController()->ClientSetRotation(CamRotation);

		//	Super::AddControllerPitchInput(Val);
	}
}

void APlayerCharacter::SmoothCameraPitch(float val)
{

}

void APlayerCharacter::SmoothCameraYaw(float val)
{
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

	const float CamInterpSpeed = CVarCameraInterpSpeed.GetValueOnGameThread();
	const FRotator FPSCamRotation = FMath::RInterpTo(FirstPersonCameraComponent->GetComponentRotation(), DesiredViewRotation, DeltaTime, CamInterpSpeed);
	FirstPersonCameraComponent->SetWorldRotation(FPSCamRotation);
	
	const float ActorInterpSpeed = CVarActorInterpSpeed.GetValueOnGameThread();
	const FRotator ActorRotation = FMath::RInterpTo(GetActorRotation(), DesiredActorRotation, DeltaTime, ActorInterpSpeed);
	ClientSetRotation(ActorRotation);

	if (GEngine)
	{
		FString output = FString::Printf(TEXT("Desired = %f %f %f"), DesiredViewRotation.Pitch, DesiredViewRotation.Yaw, DesiredViewRotation.Roll);
		GEngine->AddOnScreenDebugMessage(235, 1.f, FColor::White, output);

		output = FString::Printf(TEXT("Desired = %f %f %f"), ActorRotation.Pitch, ActorRotation.Yaw, ActorRotation.Roll);
		GEngine->AddOnScreenDebugMessage(235, 1.f, FColor::White, output);
	}
}



void APlayerCharacter::TestCommand(float val)
{
	UE_LOG(WindigoLog, Log, TEXT("OUTPUT TEST %f"), val);
}