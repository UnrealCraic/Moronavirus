// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MVTurretProjectile.generated.h"

UCLASS(config=Game)
class AMVTurretProjectile : public AActor
{
	GENERATED_BODY()

	
public:
	AMVTurretProjectile();

public:

	UPROPERTY(EditDefaultsOnly, Category = Movement)
	class UProjectileMovementComponent* MovementComponent = nullptr;

	UFUNCTION(BlueprintPure)
	float GetDamage() const { return Damage; }

	void SetDamage(const float InDamage) { Damage = InDamage; }

private:

	float Damage = 0.0f;
};

