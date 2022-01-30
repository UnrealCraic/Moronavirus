// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MVTurret.generated.h"

UCLASS(config=Game)
class AMVTurret : public AActor
{
	GENERATED_BODY()

	
public:
	AMVTurret();

	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void Activate();

	UFUNCTION(BlueprintCallable)
	void Deactivate();

	bool CanFire() const;

private:

	void FindNewTarget();

	void FireAtTarget();

public:

	UPROPERTY(EditDefaultsOnly, Category = Projectiles)
	TSubclassOf<class AMVTurretProjectile> ProjectileClass = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = Projectiles)
	float FireRate = 60.0f;

	UPROPERTY(EditDefaultsOnly, Category = Detection)
	class USphereComponent* DetectionArea = nullptr;

private:

	class AMVAICharacterBase* CurrentTarget = nullptr;

	float SecondsUntilNextShot = 0.0f;

	int32 RemainingAmmo = 0;

	int32 MaxAmmo = 100;
};

