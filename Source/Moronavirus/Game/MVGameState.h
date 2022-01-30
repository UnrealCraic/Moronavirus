// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "MVGameState.generated.h"

UCLASS(BlueprintType, Blueprintable)
class AMVGameState : public AGameState
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameStateChanged);

public:
	AMVGameState ();

	void OnAllHospitalsDestroyed();
	virtual void HandleMatchHasEnded() override;
	virtual void HandleMatchHasStarted() override;

	UPROPERTY(BlueprintAssignable)
	FOnGameStateChanged OnGameStarted;

	UPROPERTY(BlueprintAssignable)
	FOnGameStateChanged OnGameFinished;

	UFUNCTION(BlueprintPure)
	bool IsInProgress() const { return bInProgress; }


private:

	bool bInProgress = false;
};

