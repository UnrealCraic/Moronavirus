// Copyright Epic Games, Inc. All Rights Reserved.

#include "MVTurret.h"

#include "Moronavirus/Turrets/MVTurretProjectile.h"

#include "Components/SphereComponent.h"
#include "../AI/MVAICharacterBase.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/World.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "../Game/MVGameState.h"

//////////////////////////////////////////////////////////////////////////
// ATurret

AMVTurret::AMVTurret()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	DetectionArea = CreateDefaultSubobject<USphereComponent>(TEXT("DetectionArea"));
	DetectionArea->InitSphereRadius(100.0f);

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
	BaseMesh->SetupAttachment(DetectionArea);

	BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMesh"));
	BodyMesh->SetupAttachment(BaseMesh);

	MuzzleLocation = CreateDefaultSubobject<USphereComponent>(TEXT("Muzzle"));
	MuzzleLocation->SetupAttachment(BodyMesh);
	MuzzleLocation->SetCollisionEnabled(ECollisionEnabled::NoCollision);


}
void AMVTurret::BeginPlay()
{
	Super::BeginPlay();

	RemainingAmmo = MaxAmmo;
}

void AMVTurret::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (GetIsBeingPlaced())
	{
		return;
	}

	if (!CurrentTarget || CurrentTarget->GetIsDead() || FVector::Distance(GetActorLocation(), CurrentTarget->GetActorLocation()) > DetectionArea->GetScaledSphereRadius())
	{
		FindNewTarget();
	}

	if (SecondsUntilNextShot > 0.0f)
	{
		SecondsUntilNextShot -= DeltaSeconds;
	}

	if (CurrentTarget)
	{
		UpdateRotation(DeltaSeconds);

		if (CanFire())
		{
			FireAtTarget();
		}
	}
}

void AMVTurret::UpdateRotation(float DeltaSeconds)
{
	if (CurrentTarget)
	{
		FRotator Rotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), CurrentTarget->GetActorLocation());
		BodyMesh->SetWorldRotation(Rotation);
	}
}

void AMVTurret::FinishPlacement()
{
	bIsBeingPlaced = false;

	OnPlaced();

	if (AMVGameState* MVGameState = GetWorld()->GetGameState<AMVGameState>())
	{
		if (MVGameState->HasMatchStarted())
		{
			Activate();
		}
	}
}

void AMVTurret::OnPlaced_Implementation()
{
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

	if (bLimitedAmmo && RemainingAmmo < 1)
	{
		return false;
	}

	return true;
}

void AMVTurret::Reload()
{
	RemainingAmmo = MaxAmmo;
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
	CurrentTarget = nullptr;

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

	const FRotator Rotation = UKismetMathLibrary::FindLookAtRotation(StartingLocation, CurrentTarget->GetActorLocation());

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	FTransform SpawnTransform = FTransform(Rotation, MuzzleLocation->GetComponentLocation());
	AMVTurretProjectile* SpawnedProjectile = GetWorld()->SpawnActorDeferred<AMVTurretProjectile>(ProjectileClass, SpawnTransform, this, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	if (SpawnedProjectile)
	{
		SpawnedProjectile->MovementComponent->InitialSpeed = MuzzleVelocity;
		SpawnedProjectile->MovementComponent->MaxSpeed = MuzzleVelocity;
		SpawnedProjectile->SetDamage(Damage);

		UGameplayStatics::FinishSpawningActor(SpawnedProjectile, SpawnTransform);

		SecondsUntilNextShot = FireRate / 60.0f;

		if (bLimitedAmmo)
		{
			RemainingAmmo--;
		}
	}
}