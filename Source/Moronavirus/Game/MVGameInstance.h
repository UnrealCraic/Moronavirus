// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MVGameInstance.generated.h"

UCLASS(BlueprintType, Blueprintable)
class UMVGameInstance : public UGameInstance
{
	GENERATED_BODY()

	
public:
	UMVGameInstance ();

	UFUNCTION(BlueprintCallable)
	class UMVLeaderboardHelper* GetLeaderboardHelper() { return LeaderboardHelper;}

	UFUNCTION(BlueprintCallable)
	void SetLocalPlayerName(const FString& InPlayerName);

	UFUNCTION(BlueprintCallable)
	const FString& GetLocalPlayerName() { return LocalPlayerName; }

private:

	UPROPERTY()
	class UMVLeaderboardHelper* LeaderboardHelper = nullptr;

	UPROPERTY()
	FString LocalPlayerName;
};

