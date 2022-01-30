// Copyright Epic Games, Inc. All Rights Reserved.

#include "MVHospital.h"

//////////////////////////////////////////////////////////////////////////
// AMVHospital

AMVHospital::AMVHospital()
{
	
}

void AMVHospital::BeginPlay()
{
	Super::BeginPlay();

	Health = MaxHealth;
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