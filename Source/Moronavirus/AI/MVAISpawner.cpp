// Copyright Epic Games, Inc. All Rights Reserved.

#include "MVAISpawner.h"
#include "MVAICharacterBase.h"
#include "NavigationSystem.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "AI/NavigationSystemBase.h"
#include "../Game/MVGameState.h"
#include "Components/CapsuleComponent.h"

//////////////////////////////////////////////////////////////////////////
// AMVAISpawner

AMVAISpawner::AMVAISpawner()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

void AMVAISpawner::BeginPlay()
{
	Super::BeginPlay();

	if (AMVGameState* GameState = GetWorld()->GetGameState<AMVGameState>())
	{
		if (GameState->HasMatchStarted())
		{
			Activate();
		}
		else
		{
			GameState->OnGameStarted.AddDynamic(this, &ThisClass::Activate);
		}
		GameState->OnGameFinished.AddDynamic(this, &ThisClass::Deactivate);
	}
}

void AMVAISpawner::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	SpawnedCharacters.RemoveAllSwap([](const AMVAICharacterBase* Character) { return Character->GetIsDead(); });

	if (TimeUntilNextTick > 0.0f)
	{
		TimeUntilNextTick -= DeltaSeconds;
	}

	if (CanSpawn())
	{
		SpawnAI(NumAIToSpawnPerTick);
	}
}

bool AMVAISpawner::CanSpawn() const
{
	if (TimeUntilNextTick > 0.0f)
	{
		return false;
	}

	if (SpawnClasses.Num() < 1)
	{
		return false;
	}

	if (SpawnedCharacters.Num() >= MaxNumAliveAI)
	{
		return false;
	}

	return true;
}

void AMVAISpawner::SpawnAI(int32 NumToSpawn)
{
	if (TimeUntilNextTick > 0.0f)
	{
		return;
	}

	int32 RandSpawnClassIndex = FMath::RandRange(0, SpawnClasses.Num() - 1);

	FVector FoundLocation;
	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	bool bFoundLocation = NavSys->K2_GetRandomReachablePointInRadius(GetWorld(), GetActorLocation(), FoundLocation, MaxSpawnRadius);
	
	if (!bFoundLocation)
	{
		return;
	}

	FoundLocation.Z += 100.0f;
	const FRotator SpawnRotation = GetActorRotation();
	FTransform SpawnTransform = FTransform(SpawnRotation, FoundLocation);

	AMVAICharacterBase* SpawnedAI = GetWorld()->SpawnActorDeferred<AMVAICharacterBase>(SpawnClasses[RandSpawnClassIndex], SpawnTransform, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
	if (SpawnedAI)
	{
		FHitResult HitResult;
		GetWorld()->LineTraceSingleByChannel(HitResult, SpawnedAI->GetActorLocation(), SpawnedAI->GetActorLocation() + (SpawnedAI->GetActorUpVector() * -150.0f), ECC_Visibility);

		FTransform AdjustedTransform = FTransform(SpawnedAI->GetActorRotation(), SpawnedAI->GetActorLocation());

		if (HitResult.bBlockingHit)
		{
			AdjustedTransform.SetLocation(HitResult.ImpactPoint + (SpawnedAI->GetActorUpVector() * SpawnedAI->GetCapsuleComponent()->GetScaledCapsuleHalfHeight()));
		}		
		
		UGameplayStatics::FinishSpawningActor(SpawnedAI, AdjustedTransform);

		SpawnedCharacters.Add(SpawnedAI);
		TimeUntilNextTick = TimeBetweenTicks;
	}
}

void AMVAISpawner::Activate()
{
	if (!bIsActive)
	{
		bIsActive = true;
		SetActorTickEnabled(true);
	}
}

void AMVAISpawner::Deactivate()
{
	if (bIsActive)
	{
		bIsActive = false;
		SetActorTickEnabled(false);
	}
}