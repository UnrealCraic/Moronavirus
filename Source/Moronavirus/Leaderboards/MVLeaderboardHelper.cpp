// Copyright Epic Games, Inc. All Rights Reserved.

#include "MVLeaderboardHelper.h"

#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"

#include "Runtime/JsonUtilities/Public/JsonUtilities.h"
#include "Kismet/GameplayStatics.h"

static const FString ID("4246098d39fb4c497c3dd780f9026a72");
static const FString Key("f5fcc010cd");


UMVLeaderboardHelper::UMVLeaderboardHelper(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer)
{

}


void UMVLeaderboardHelper::OnPostHighScoreFinished(FHttpRequestPtr Reuest, FHttpResponsePtr Response, bool bSuccess)
{
	check(CurrentUpdateScoreRequest.bActive);

	UE_LOG(LogTemp, Log, TEXT("%s"), *Response->GetContentAsString());

	CurrentUpdateScoreRequest.Reset();
	OnNewScorePushed.Broadcast();
}

void UMVLeaderboardHelper::PostHighScore(const FString& User, int NewHighScore)
{
	//cleanup the name
	const float score = (float)NewHighScore;
	const FString username = User;
	const FString Metadata;

	//generate the key
	FString hashSource = FString::Printf(TEXT("%s%.3f%s%s"), *username, score, *Key, *Metadata);
	FString generatedKey = FMD5::HashAnsiString(*hashSource);

	//setup the request
	FString url = FString::Printf(TEXT("http://%s/api/submitScore.php"), TEXT("epicleaderboard.com"));

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &ThisClass::OnPostHighScoreFinished);

	Request->SetURL(url);
	Request->SetVerb("POST");
	Request->SetHeader(TEXT("User-Agent"), "X-UnrealEngine-EpicLeaderboard");
	Request->SetHeader("Content-Type", TEXT("application/x-www-form-urlencoded"));
	Request->SetHeader("Accept", TEXT("application/vnd.epicleaderboard.v2+json"));

	FString content = FString::Printf(TEXT("accessID=%s&username=%s&score=%.3f&meta=%s&hash=%s"), *ID, *username, score, *Metadata, *generatedKey);

	Request->SetContentAsString(content);

	CurrentUpdateScoreRequest.Username = User;
	CurrentUpdateScoreRequest.NewScore = NewHighScore;
	CurrentUpdateScoreRequest.bActive = true;

	OnUpdateScoreRequestSent.Broadcast();

	Request->ProcessRequest();
}

void UMVLeaderboardHelper::RequestHighScores(const FString& PlayerName, bool bAroundPlayer)
{
	if (bObtainingHighScores)
	{
		UE_LOG(LogTemp, Warning, TEXT("UMVLeaderboardHelper::RequestHighScores Request already sent"));
		return;
	}
	//setup the request
	FString url = FString::Printf(TEXT("http://%s/api/getScores.php?accessID=%s&username=%s&around=%d"), TEXT("epicleaderboard.com"), *ID, *PlayerName, bAroundPlayer);
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &UMVLeaderboardHelper::OnHighScoresResponseReceived);

	Request->SetURL(url);
	Request->SetVerb("GET");
	Request->SetHeader(TEXT("User-Agent"), "X-UnrealEngine-EpicLeaderboard");
	Request->SetHeader("Content-Type", TEXT("application/x-www-form-urlencoded"));
	Request->SetHeader("Accept", TEXT("application/vnd.epicleaderboard.v2+json"));

	bObtainingHighScores = true;
	Request->ProcessRequest();
}

void UMVLeaderboardHelper::DeserializeMap(FString json, TMap<FString, FString>& map)
{
	map.Empty();

	TSharedPtr<FJsonValue> JsonParsed;
	TSharedRef< TJsonReader<> > JsonReader = TJsonReaderFactory<>::Create(json);
	if (FJsonSerializer::Deserialize(JsonReader, JsonParsed) && JsonParsed.IsValid())
	{
		for (const auto& JsonEntry : JsonParsed->AsObject()->Values)
		{
			map.Add(JsonEntry.Key, JsonEntry.Value->AsString());
		}
	}
}

void UMVLeaderboardHelper::OnHighScoresResponseReceived(FHttpRequestPtr Reuest, FHttpResponsePtr Response, bool bSuccess)
{
	check(bObtainingHighScores);

	bObtainingHighScores = false;


	UE_LOG(LogTemp, Log, TEXT("%s"), *Response->GetContentAsString());

	if (Response->GetResponseCode() == EHttpResponseCodes::Ok)
	{
		FString data = Response->GetContentAsString();

		//parse json response
		TSharedPtr<FJsonValue> JsonParsed;
		TSharedRef< TJsonReader<> > JsonReader = TJsonReaderFactory<>::Create(data);
		if (FJsonSerializer::Deserialize(JsonReader, JsonParsed) && JsonParsed.IsValid())
		{
			//parse top score list 
			TSharedPtr<FJsonValue>* topScores = JsonParsed->AsObject()->Values.Find(TEXT("scores"));

			if (topScores != nullptr && topScores->IsValid())
			{
				//clear entries
				LeaderboardEntries.Empty();

				FJsonObjectConverter::JsonArrayToUStruct<FEpicLeaderboardEntry>(topScores->Get()->AsArray(), &LeaderboardEntries, 0, 0);

				//deserialize metadata
				for (auto& Entry : LeaderboardEntries)
				{
					DeserializeMap(Entry.meta, Entry.Metadata);
				}
			}

			//clear struct
			PlayerEntry = FEpicLeaderboardEntry();
			if (UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(GetWorld()))
			{
				if (ULocalPlayer* Player = GameInstance->GetLocalPlayerByIndex(0))
				{					
					const FString& PlayerName = Player->GetName();

					for (const FEpicLeaderboardEntry& Entry : LeaderboardEntries)
					{
						if (Entry.username.Equals(PlayerName))
						{
							PlayerEntry = Entry;
							break;
						}
					}
				}
			}
		}
	}

	OnHighScoresObtained.Broadcast();
}