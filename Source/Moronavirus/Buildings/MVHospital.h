// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MVHospital.generated.h"

UCLASS(BlueprintType, Blueprintable)
class AMVHospital : public AActor
{
	GENERATED_BODY()

	DECLARE_EVENT(ThisClass, FOnHealthDepleted);
	
public:
	AMVHospital ();

	virtual void BeginPlay() override;
	virtual float TakeDamage(float DamageAmount,	struct FDamageEvent const& DamageEvent,	class AController* EventInstigator,	AActor* DamageCauser) override;

	FOnHealthDepleted& GetOnHealthDepleted() { return HealthDepletedEvent; }
	bool IsHealthDepleted() const { return Health <= 0.0f; }

	UFUNCTION(BlueprintPure)
	float GetHealth() const { return Health;}

	UFUNCTION(BlueprintPure)
	float GetMaxHealth() const { return MaxHealth; }

	UFUNCTION(BlueprintCallable)
	FVector GetRandomAttackTargetFrom(const FVector& EnemyLocation);

private:

	void OnHealthDepleted();

private:

	TArray<FVector> TargetPoints;

	FOnHealthDepleted HealthDepletedEvent;

	float Health = 0.0f;

	UPROPERTY(EditDefaultsOnly)
	float MaxHealth = 0.0f;
};

