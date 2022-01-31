// Copyright Epic Games, Inc. All Rights Reserved.

#include "MVHospital.h"
#include "Components/SphereComponent.h"

//////////////////////////////////////////////////////////////////////////
// AMVHospital

AMVHospital::AMVHospital()
{
	
}

void AMVHospital::BeginPlay()
{
	Super::BeginPlay();

	Health = MaxHealth;

	TArray<UActorComponent*> Components;
	GetComponents(USphereComponent::StaticClass(), Components);

	for (UActorComponent* Component : Components)
	{
		if (USphereComponent* SPhereCOmponent = Cast<USphereComponent>(Component))
		{
			TargetPoints.Add(SPhereCOmponent->GetComponentLocation());
		}
	}
}

float AMVHospital::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (!CanBeDamaged())
	{
		return 0;
	}

	const float OriginalHealth = Health;
	Health = FMath::Max(0.0f, Health - DamageAmount);
	if (Health == 0.0f)
	{
		OnHealthDepleted();
	}

	return OriginalHealth - Health;
}

void AMVHospital::OnHealthDepleted()
{
	SetCanBeDamaged(false);

	HealthDepletedEvent.Broadcast();
}


FVector AMVHospital::GetRandomAttackTargetFrom(const FVector& EnemyLocation)
{
	if (TargetPoints.Num() < 1)
	{
		return GetActorLocation();
	}

	TargetPoints.Sort([EnemyLocation](const FVector& A, const FVector& B)
	{
		float DistanceA = FVector::DistSquared(EnemyLocation, A);
		float DistanceB = FVector::DistSquared(EnemyLocation, B);

		return DistanceA > DistanceB;
	});

	int32 NumTargetPoints = TargetPoints.Num();
	int32 RandomIndexMax = NumTargetPoints / 2;
	int32 IndexToUse = FMath::RandRange(0, RandomIndexMax);
	
	return TargetPoints[IndexToUse];
}