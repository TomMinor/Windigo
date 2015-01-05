// Copyright 1998-2014 NCCA - Tom Minor, Tom Becque, George Hulm, Alex Long, Kathleen Kononczuk Sa All Rights Reserved.

#include "Windigo.h"
#include "CharacterFootstepProperties.h"


const TArray<USoundCue*>& UCharacterFootstepProperties::GetSurfaceSounds(EPhysicalSurface PhysicalSurface) const
{
	return GetSurfaceInfo(PhysicalSurface).SoundEffects;
}

const TArray<UParticleSystem*>& UCharacterFootstepProperties::GetSurfaceParticles(EPhysicalSurface PhysicalSurface) const
{
	return GetSurfaceInfo(PhysicalSurface).ParticleFX;
}

const TArray<UMaterial*>& UCharacterFootstepProperties::GetSurfaceDecals(EPhysicalSurface PhysicalSurface, bool bIsLeft) const
{
	return !bIsLeft ? GetSurfaceInfo(PhysicalSurface).LeftFootprintMaterials : GetSurfaceInfo(PhysicalSurface).RightFootprintMaterials;
}

const FSurfaceTypeInfo& UCharacterFootstepProperties::GetSurfaceInfo(EPhysicalSurface PhysicalSurface) const
{
	switch (PhysicalSurface)
	{
		case EPhysicalSurface::SurfaceType1:	{ return SurfaceType1; }
		case EPhysicalSurface::SurfaceType2:	{ return SurfaceType2; }
		case EPhysicalSurface::SurfaceType3:	{ return SurfaceType3; }
		case EPhysicalSurface::SurfaceType4:	{ return SurfaceType4; }
		case EPhysicalSurface::SurfaceType5:	{ return SurfaceType5; }
		case EPhysicalSurface::SurfaceType6:	{ return SurfaceType6; }
		case EPhysicalSurface::SurfaceType7:	{ return SurfaceType7; }
		case EPhysicalSurface::SurfaceType8:	{ return SurfaceType8; }
		case EPhysicalSurface::SurfaceType9:	{ return SurfaceType9; }
		case EPhysicalSurface::SurfaceType10:	{ return SurfaceType10; }
		case EPhysicalSurface::SurfaceType11:	{ return SurfaceType11; }
		case EPhysicalSurface::SurfaceType12:	{ return SurfaceType12; }
		case EPhysicalSurface::SurfaceType13:	{ return SurfaceType13; }
		case EPhysicalSurface::SurfaceType14:	{ return SurfaceType14; }
		case EPhysicalSurface::SurfaceType15:	{ return SurfaceType15; }
		default:								{ return SurfaceType_Default; }
	}
}

