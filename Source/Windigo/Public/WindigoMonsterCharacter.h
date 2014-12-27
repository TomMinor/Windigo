// Copyright 1998-2014 NCCA - Tom Minor, Tom Becque, George Hulm, Alex Long, Kathleen Kononczuk Sa All Rights Reserved.

#pragma once

#include "GameFramework/Character.h"
#include "WindigoMonsterCharacter.generated.h"

/* TODO
   - Investigate sounds
   - Move to visible player (or last known pos)
   - Investigate footsteps
*/

/**
 * 
 */
UCLASS()
class WINDIGO_API AWindigoMonsterCharacter : public ACharacter
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	
	virtual void PostInitializeComponents() override;
	
};
