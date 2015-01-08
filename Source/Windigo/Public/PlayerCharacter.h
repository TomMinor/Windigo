// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "WindigoBaseCharacter.h"
#include "PlayerCharacter.generated.h"

/* TODO
  - Death state
  - Alert AI (sound/visual)
  - Event when we should go into cover
  - Shake
    + View bob
	+ Land shake
	+ Jump shake
	+ Blizzard shake?
*/

/**
 * 
 */
UCLASS()
class WINDIGO_API APlayerCharacter : public AWindigoBaseCharacter
{
	GENERATED_BODY()

public:
	APlayerCharacter(const class FObjectInitializer& PCIP);

	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;

	// First person camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	UCameraComponent* FirstPersonCameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	USpringArmComponent* LeanCameraBoom;

	/** How fast does the player move when they Sprint? */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement")
	float fSprintSpeed;

	/** How fast does the player move when they Walk? */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement")
	float fWalkSpeed;

	// Gestures
	UFUNCTION(BlueprintImplementableEvent, Category = Events, meta = (FriendlyName = "On Wave"))
	void OnWaveEvent();
	UFUNCTION(BlueprintImplementableEvent, Category = Events, meta = (FriendlyName = "On Point"))
	void OnPointEvent();
	UFUNCTION(BlueprintImplementableEvent, Category = Events, meta = (FriendlyName = "On Shout"))
	void OnShoutEvent();

protected:
	/////////////////////* Input Booleans *//////////////////////
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement")
	bool bIsSprinting;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement")
	bool bRightClick;

	float InitialMeshYaw;

	/* The camera rotation we should interpolate towards (every tick) */
	FRotator DesiredViewRotation;
	FVector DesiredViewLocation;

	/* The actor rotation we should interpolate towards (every tick) */
	FRotator DesiredActorRotation;

	FVector DesiredBoomTargetLocation;

protected:
	/////////////////////* Commands *//////////////////////

	UFUNCTION(exec)
	virtual void TestCommand(float val);

	/////////////////////* Methods *//////////////////////

	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	virtual void Landed(const FHitResult& Hit) override;

	// Limit mouse input
	virtual void ViewYaw(float Val);
	virtual void ViewPitch(float Val);

	virtual void OnStartRightClick();
	virtual void OnStopRightClick();

	//Handle forward/backward movement
	UFUNCTION()
	void MoveForward(float Val);

	//Handle strafing
	UFUNCTION()
	void MoveRight(float Val);

	// Set jump flag on keypress
	UFUNCTION()
	void OnStartJump();

	// Clear jump flag on keypress
	UFUNCTION()
	void OnStopJump();

	// Toggle crouch
	UFUNCTION()
	void OnBeginCrouch();
	UFUNCTION()
	void OnStopCrouch();

	// Toggle sprint
	UFUNCTION()
	void OnStartSprint();
	UFUNCTION()
	void OnStopSprint();

	UFUNCTION()
	void OnWave();
	UFUNCTION()
	void OnPoint();
	UFUNCTION()
	void OnShout();

	void Tick(float DeltaTime) override;
};
