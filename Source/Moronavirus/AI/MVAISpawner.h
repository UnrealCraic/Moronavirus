// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MVAISpawner.generated.h"

UCLASS(BlueprintType, Blueprintable)
class AMVAISpawner : public AActor
{
	GENERATED_BODY()

	
public:
	AMVAISpawner ();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

private: 

	UFUNCTION()
	void Activate();

	UFUNCTION()
	void Deactivate();

	void SpawnAI(int32 NumToSpawn);

	bool CanSpawn() const;

public:

	UPROPERTY(EditDefaultsOnly, Category = Spawning)
	float TimeBetweenTicks = 2.0f;

	UPROPERTY(EditDefaultsOnly, Category = Spawning)
	float MaxSpawnRadius = 50.0f;

	UPROPERTY(EditDefaultsOnly, Category = Spawning)
	int32 NumAIToSpawnPerTick = 1;

	UPROPERTY(EditDefaultsOnly, Category = Spawning)
	int32 MaxNumAliveAI = 10;

	UPROPERTY(EditDefaultsOnly, Category = Spawning)
	TArray<TSubclassOf<class AMVAICharacterBase>> SpawnClasses;

private:

	TArray<class AMVAICharacterBase*> SpawnedCharacters;

	float TimeUntilNextTick = 0.0f;

	bool bIsActive = false;
};

