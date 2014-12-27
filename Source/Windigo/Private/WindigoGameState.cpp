// Copyright 1998-2014 NCCA - Tom Minor, Tom Becque, George Hulm, Alex Long, Kathleen Kononczuk Sa All Rights Reserved.

#include "Windigo.h"
#include "PartnerPlayerState.h"
#include "WindigoGameState.h"

AWindigoGameState::AWindigoGameState(const class FObjectInitializer & ObjectInitializer)
	: Super(ObjectInitializer)
{
	UE_LOG(WindigoInit, Warning, TEXT("WINDIGO GAMESTATE CREATED"));
}