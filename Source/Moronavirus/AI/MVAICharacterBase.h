// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MVAICharacterBase.generated.h"

UCLASS(BlueprintType, Blueprintable)
class AMVAICharacterBase : public AActor
{
	GENERATED_BODY()

	
public:
	AMVAICharacterBase ();

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintPure)
	float GetCurrentHealth() const { return Health;}

	UFUNCTION(BlueprintPure)
	float GetMaxHealth() const { return MaxHealth; }

	UFUNCTION(BlueprintPure)
	float GetIsDead() const { return bIsDead; }

private:

	void Die();

private:

	bool bIsDead = false;

	float Health = 0.0f;

	UPROPERTY(EditDefaultsOnly, Category = Setup)
	float MaxHealth = 100.0f;
};

