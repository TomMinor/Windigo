// Copyright 1998-2014 NCCA - Tom Minor, Tom Becque, George Hulm, Alex Long, Kathleen Kononczuk Sa All Rights Reserved.

#pragma once

#include "WindigoSingleton.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "WindigoSingletonLibrary.generated.h"

/**
 * 
 */
UCLASS()
class WINDIGO_API UWindigoSingletonLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Windigo Global Data")
	static UWindigoSingleton* GetWindigoGlobalData(bool& IsValid);	
};
