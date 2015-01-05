// Copyright 1998-2014 NCCA - Tom Minor, Tom Becque, George Hulm, Alex Long, Kathleen Kononczuk Sa All Rights Reserved.

#include "Windigo.h"
#include "WindigoSingleton.h"

UWindigoSingleton::UWindigoSingleton(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// SurfaceType_Default
	UE_LOG(WindigoSingleton, Log, TEXT("Searching for PhysicalMaterial.SurfaceTypes"));

	PhysicalMaterialFootstepAudio.Add( FSurfaceFootstepProperties(TEXT("DefaultSurfaceType"), TEXT("Default")) );

	if (GConfig)
	{
		/* Query for custom physical surface types */
		for (int i = 1; i < EPhysicalSurface::SurfaceType_Max; i++)
		{
			FString SurfaceType = FString::Printf(TEXT("SurfaceType%d"), i);
			FString Value;
			GConfig->GetString(TEXT("PhysicalMaterial.SurfaceTypes"), *SurfaceType, Value, GEngineIni);

			PhysicalMaterialFootstepAudio.Add( FSurfaceFootstepProperties(SurfaceType, (Value.Len() > 0) ? Value : TEXT("None")) );
		}

		if (PhysicalMaterialFootstepAudio.Max() == 0)
		{
			UE_LOG(WindigoSingleton, Log, TEXT("No custom physical materials found"));
		}
	}
	else
	{
		UE_LOG(WindigoSingleton, Error, TEXT("Could not load configuration file, GConfig is NULL"));
	}
}


