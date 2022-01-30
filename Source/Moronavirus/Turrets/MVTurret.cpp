// Copyright Epic Games, Inc. All Rights Reserved.

#include "MVTurret.h"

#include "Moronavirus/Turrets/MVTurretProjectile.h"

#include "Components/SphereComponent.h"
#include "../AI/MVAICharacterBase.h"

//////////////////////////////////////////////////////////////////////////
// ATurret

AMVTurret::AMVTurret()
{
	PrimaryActorTick.bCanEverTick;
	PrimaryActorTick.bStartWithTickEnabled = false;

	DetectionArea = CreateDefaultSubobject<USphereComponent>(TEXT("DetectionArea"));
	DetectionArea->InitSphereRadius(100.0f);
}
void AMVTurret::BeginPlay()
{
	Super::BeginPlay();

	RemainingAmmo = MaxAmmo;
}

void AMVTurret::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!CurrentTarget || CurrentTarget->GetIsDead())
	{
		FindNewTarget();
	}

	if (SecondsUntilNextShot > 0.0f)
	{
		SecondsUntilNextShot -= DeltaSeconds;
	}

	if (CanFire())
	{
		FireAtTarget();
	}
}

bool AMVTurret::CanFire() const
{
	if (!CurrentTarget || CurrentTarget->GetIsDead())
	{
		return false;
	}

	if (SecondsUntilNextShot > 0.0f)
	{
		return false;
	}

	if (!ProjectileClass)
	{
		return false;
	}

	if (RemainingAmmo < 1)
	{
		return false;
	}

	return true;
}

void AMVTurret::Activate()
{
	SetActorTickEnabled(true);
}

void AMVTurret::Deactivate()
{
	SetActorTickEnabled(false);
}

void AMVTurret::FindNewTarget()
{
	TArray<AActor*> OverlappingActors;
	DetectionArea->GetOverlappingActors(OverlappingActors, AMVAICharacterBase::StaticClass());

	float ClosestDistance = DetectionArea->GetScaledSphereRadius();
	AActor* ClosestActor = nullptr;

	const FVector ThisLocation = GetActorLocation();
	for (AActor* OverlappingActor : OverlappingActors)
	{
		const float CurrentDistance = FVector::Distance(OverlappingActor->GetActorLocation(), ThisLocation);
		if (CurrentDistance < ClosestDistance)
		{
			ClosestDistance = CurrentDistance;
			ClosestActor = OverlappingActor;
		}
	}

	CurrentTarget = Cast<AMVAICharacterBase>(ClosestActor);
}

void AMVTurret::FireAtTarget()
{
	if (!CanFire())
	{
		return;
	}

	const FVector StartingLocation = GetActorLocation();
	const FRotator StartingRotation = GetActorRotation();

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AActor* SpawnedProjectile = GetWorld()->SpawnActor(ProjectileClass, &StartingLocation, &StartingRotation, SpawnParams);

	if (SpawnedProjectile)
	{
		SecondsUntilNextShot = FireRate / 60.0f;
		RemainingAmmo--;
	}
}