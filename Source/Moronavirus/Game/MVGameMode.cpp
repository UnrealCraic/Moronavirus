// Copyright Epic Games, Inc. All Rights Reserved.

#include "MVGameMode.h"

#include "Moronavirus/Buildings/MVHospital.h"
#include "Moronavirus/Turrets/MVTurret.h"
#include "EngineUtils.h"
#include "MVGameState.h"

//////////////////////////////////////////////////////////////////////////
// AMVGameMode

AMVGameMode::AMVGameMode()
{
	bDelayedStart = true;
	bStartPlayersAsSpectators = true;
}

void AMVGameMode::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();

	for (TActorIterator<AMVHospital> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		Hospitals.Add(*ActorItr);
		ActorItr->GetOnHealthDepleted().AddUObject(this, &ThisClass::OnHospitalHealthDepleted);
	}

	for (TActorIterator<AMVTurret> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		ActorItr->Activate();
	}
}

void AMVGameMode::OnHospitalHealthDepleted()
{
	UE_LOG(LogTemp, Log, TEXT("Hospital destroyed!"));
	Hospitals.RemoveAllSwap([](AMVHospital* PotentialHospital) { return PotentialHospital->IsHealthDepleted(); });

	if (Hospitals.Num() < 1)
	{
		if (AMVGameState* MVGameState = GetGameState<AMVGameState>())
		{
			MVGameState->OnAllHospitalsDestroyed();
		}

		for (TActorIterator<AMVTurret> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			ActorItr->Deactivate();
		}

		EndMatch();
	}
	/*if (AMVGameState* GameState = GetGameState<AMVGameState>())
	{
		GameState->
	}*/
}

