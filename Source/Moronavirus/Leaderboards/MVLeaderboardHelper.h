// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/IHttpRequest.h"
#include "MVLeaderboardHelper.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUpdateScoreStatusChanged);

USTRUCT(BlueprintType)
struct FEpicLeaderboardEntry
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int rank;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString username;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString score;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString country;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FString, FString> Metadata;

	UPROPERTY()
	FString meta;
};

USTRUCT()
struct FPostHighScoreRequest
{
	GENERATED_USTRUCT_BODY()

	void Reset()
	{
		Username.Empty();
		NewScore = 0;
		bActive = false;
	}

	FString Username;

	int NewScore = 0;

	bool bActive = false;
};

UCLASS()
class UMVLeaderboardHelper : public UObject
{
	GENERATED_UCLASS_BODY()

public:

	UFUNCTION(BlueprintCallable)
	void PostHighScore(const FString& User, int NewHighScore);

	UFUNCTION(BlueprintCallable)
	void RequestHighScores(const FString& PlayerName, bool bAroundPlayer = false);

	void DeserializeMap(FString json, TMap<FString, FString>& map);

	UPROPERTY(BlueprintAssignable)
	FOnUpdateScoreStatusChanged OnNewScorePushed;

	UPROPERTY(BlueprintAssignable)
	FOnUpdateScoreStatusChanged OnHighScoresObtained;

	FOnUpdateScoreStatusChanged OnUpdateScoreRequestSent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "EpicLeaderboard|Properties")
	TArray<FEpicLeaderboardEntry> LeaderboardEntries;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "EpicLeaderboard|Properties")
	FEpicLeaderboardEntry PlayerEntry;

private:

	void OnPostHighScoreFinished(FHttpRequestPtr, FHttpResponsePtr, bool);
	void OnHighScoresResponseReceived(FHttpRequestPtr, FHttpResponsePtr, bool);

private:
	
	FPostHighScoreRequest CurrentUpdateScoreRequest;

	//UPROPERTY()
	//class UYCGameInstance* GameInstance = nullptr;

	bool bObtainingHighScores = false;
};