// Copyright Epic Games, Inc. All Rights Reserved.

#include "MVGameInstance.h"
#include "Moronavirus/Leaderboards/MVLeaderboardHelper.h"

//////////////////////////////////////////////////////////////////////////
// AMVGameInstance

UMVGameInstance::UMVGameInstance()
{
	LeaderboardHelper = NewObject<UMVLeaderboardHelper>();
}


void UMVGameInstance::SetLocalPlayerName(const FString& InPlayerName)
{
	LocalPlayerName = InPlayerName;
}