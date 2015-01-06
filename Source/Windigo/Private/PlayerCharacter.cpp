// Fill out your copyright notice in the Description page of Project Settings.  






#include "Windigo.h"
#include "PlayerCharacter.h"

APlayerCharacter::APlayerCharacter(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer), fSprintSpeed(600), fWalkSpeed(140)
{
	// Create CameraComponent
	FirstPersonCameraComponent = ObjectInitializer.CreateDefaultSubobject<UCameraComponent>(this, TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->AttachParent = GetCapsuleComponent();

	// Move camera to eye position
	FirstPersonCameraComponent->RelativeLocation = FVector(16.f, 4.f, BaseEyeHeight + 20.f);

	// Allow pawn to control rotation of camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	GetCharacterMovement()->MaxWalkSpeed = fWalkSpeed;
	GetCharacterMovement()->MaxWalkSpeedCrouched = fWalkSpeed * 0.25;

	GetCharacterMovement()->FallingLateralFriction = 10.0f;
	GetCharacterMovement()->AirControl = 0.1f;
	GetCharacterMovement()->bMaintainHorizontalGroundVelocity = false; // If false, then walking movement maintains velocity magnitude parallel to the ramp surface.

	bIsSprinting = false;
	DesiredViewLocation = FVector(0.f, 0.f, 0.f);
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

	InputComponent->BindAxis("Yaw", this, &APlayerCharacter::SmoothCameraYaw);
	InputComponent->BindAxis("Pitch", this, &APlayerCharacter::SmoothCameraPitch);


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
	UE_LOG(WindigoLog, Log, TEXT("StartRightClick"));
}

void APlayerCharacter::OnStopRightClick()
{
	UE_LOG(WindigoLog, Log, TEXT("StopRightClick"));
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

		if (bIsSprinting && val < 0.0f)
		{
			//val = FMath::Abs(val);
			direction = -direction;
		}

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

void APlayerCharacter::AddControllerYawInput(float Val)
{
	float camYaw = FirstPersonCameraComponent->GetComponentTransform().GetRotation().Rotator().Yaw;
	if (GEngine)
	{
		FString output = TEXT("Yaw = ") + FString::SanitizeFloat(camYaw);
		GEngine->AddOnScreenDebugMessage(255, 1.f, FColor::White, output);
	}

	//Super::AddControllerYawInput(Val);
}

void APlayerCharacter::AddControllerPitchInput(float Val)
{
	float camPitch = FirstPersonCameraComponent->GetComponentTransform().GetRotation().Rotator().Pitch;
	if (GEngine)
	{
		FString output = TEXT("Pitch = ") + FString::SanitizeFloat(camPitch);
		GEngine->AddOnScreenDebugMessage(256, 1.f, FColor::Yellow, output);
	}

	//Super::AddControllerPitchInput(Val);
}

void APlayerCharacter::SmoothCameraPitch(float val)
{
	FRotator NewRotation = FirstPersonCameraComponent->GetComponentTransform().GetRotation().Rotator();

	GetController()->ClientSetRotation(NewRotation);
}

void APlayerCharacter::SmoothCameraYaw(float val)
{
	float camYaw = FirstPersonCameraComponent->GetComponentTransform().GetRotation().Rotator().Yaw;

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
	bTemp = false;
}

void APlayerCharacter::Tick(float DelaySeconds)
{
	Super::Tick(DelaySeconds);

	
}