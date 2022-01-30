// Copyright Epic Games, Inc. All Rights Reserved.

#include "MVAICharacterBase.h"

//////////////////////////////////////////////////////////////////////////
// AMVAICharacterBase

AMVAICharacterBase::AMVAICharacterBase()
{
	
}

float AMVAICharacterBase::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	if (bIsDead)
	{
		return 0.0f;
	}

	const float CachedHealth = Health;
	Health = FMath::Max(0.0f, Health - DamageAmount);

	if (Health <= 0.0f)
	{
		Die();
	}

	return CachedHealth - Health;
}

void AMVAICharacterBase::Die()
{
	bIsDead = true;
}