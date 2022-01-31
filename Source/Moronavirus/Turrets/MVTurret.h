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

	UFUNCTION(BlueprintCallable)
	int32 GetRemainingAmmo() {return RemainingAmmo;}

	UFUNCTION(BlueprintCallable)
	int32 GetMaxAmmo() {return MaxAmmo;}

	UFUNCTION(BlueprintCallable)
	void Reload();

private:

	void FindNewTarget();

	void FireAtTarget();

	void UpdateRotation(float DeltaSeconds);

public:

	UPROPERTY(EditDefaultsOnly, Category = Projectiles)
	class UStaticMeshComponent* BaseMesh = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = Projectiles)
	class UStaticMeshComponent* BodyMesh = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = Projectiles)
	class USphereComponent* MuzzleLocation = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = Projectiles)
	TSubclassOf<class AMVTurretProjectile> ProjectileClass = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = Projectiles)
	float FireRate = 60.0f;

	UPROPERTY(EditDefaultsOnly, Category = Projectiles)
	float MuzzleVelocity = 600.0f;

	UPROPERTY(EditDefaultsOnly, Category = Projectiles)
	bool bLimitedAmmo = false;

	UPROPERTY(EditDefaultsOnly, Category = Projectiles)
	float Damage = 10.0f;

	UPROPERTY(EditDefaultsOnly, Category = Projectiles)
	float TurnRate = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Projectiles, meta = (EditCondition = "bLimitedAmmo"))
	int32 MaxAmmo = 100;

	UPROPERTY(EditDefaultsOnly, Category = Detection)
	class USphereComponent* DetectionArea = nullptr;

private:

	class AMVAICharacterBase* CurrentTarget = nullptr;

	float SecondsUntilNextShot = 0.0f;

	int32 RemainingAmmo = 0;

};

