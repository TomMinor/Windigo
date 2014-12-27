// Fill out your copyright notice in the Description page of Project Settings.

#include "Windigo.h"
#include "PlayerCharacter.h"


APlayerCharacter::APlayerCharacter(const class FObjectInitializer& PCIP)
	: Super(PCIP), fSprintSpeed(600), fWalkSpeed(140)
{
	// Create CameraComponent
	FirstPersonCameraComponent = PCIP.CreateDefaultSubobject<UCameraComponent>(this, TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->AttachParent = GetCapsuleComponent();

	// Move camera to eye position
	FirstPersonCameraComponent->RelativeLocation = FVector(16.f, 4.f, BaseEyeHeight + 20.f);

	// Allow pawn to control rotation of camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	GetCharacterMovement()->MaxWalkSpeed = fWalkSpeed;
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

	InputComponent->BindAxis("Yaw", this, &APlayerCharacter::AddControllerYawInput);
	InputComponent->BindAxis("Pitch", this, &APlayerCharacter::AddControllerPitchInput);


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
}

void APlayerCharacter::MoveForward(float val)
{
	if ((Controller != NULL) && (val != 0.0f))
	{
		//Which way is forward?
		FRotator rotation = Controller->GetControlRotation();

		//Limit pitch during walk/fall
		if (GetCharacterMovement()->IsMovingOnGround() || GetCharacterMovement()->IsFalling())
		{
			rotation.Pitch = 0.0f;
		}

		//Apply movement in the direction we're facing
		const FVector direction = FRotationMatrix(rotation).GetScaledAxis(EAxis::X);
		AddMovementInput(direction, val);
	}
}

void APlayerCharacter::MoveRight(float val)
{
	if ((Controller != NULL) && (val != 0.0f))
	{
		//Which way is right?
		const FRotator rotation = Controller->GetControlRotation();
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

	Super::AddControllerYawInput(Val);
}

void APlayerCharacter::AddControllerPitchInput(float Val)
{
	float camPitch = FirstPersonCameraComponent->GetComponentTransform().GetRotation().Rotator().Pitch;
	if (GEngine)
	{
		FString output = TEXT("Pitch = ") + FString::SanitizeFloat(camPitch);
		GEngine->AddOnScreenDebugMessage(256, 1.f, FColor::Yellow, output);
	}

	Super::AddControllerPitchInput(Val);
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
	GetCharacterMovement()->MaxWalkSpeed = fSprintSpeed;
}

void APlayerCharacter::OnStopSprint()
{
	GetCharacterMovement()->MaxWalkSpeed = fWalkSpeed;
}