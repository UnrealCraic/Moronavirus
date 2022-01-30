// Copyright Epic Games, Inc. All Rights Reserved.

#include "MVTurretProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"


//////////////////////////////////////////////////////////////////////////
// ATurret

AMVTurretProjectile::AMVTurretProjectile()
{
	MovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MovementCOmponent"));
}