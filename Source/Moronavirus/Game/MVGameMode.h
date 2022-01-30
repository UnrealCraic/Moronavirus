// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "MVGameMode.generated.h"

UCLASS(BlueprintType, Blueprintable)
class AMVGameMode : public AGameMode
{
	GENERATED_BODY()

	
public:
	AMVGameMode ();

	virtual void HandleMatchHasStarted();

private:

	void OnHospitalHealthDepleted();

private:

	TArray<class AMVHospital*> Hospitals;
};

