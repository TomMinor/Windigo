// Copyright 1998-2014 NCCA - Tom Minor, Tom Becque, George Hulm, Alex Long, Kathleen Kononczuk Sa All Rights Reserved.

#include "Windigo.h"
#include "WindigoSingletonLibrary.h"



UWindigoSingleton* UWindigoSingletonLibrary::GetWindigoGlobalData(bool& IsValid)
{
	IsValid = false;
	UWindigoSingleton* DataInstance = Cast<UWindigoSingleton>(GEngine->GameSingleton);

	if (!DataInstance) return NULL;
	if (!DataInstance->IsValidLowLevel()) return NULL;

	IsValid = true;
	return DataInstance;
}

