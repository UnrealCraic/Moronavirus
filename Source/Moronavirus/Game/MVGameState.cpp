// Copyright Epic Games, Inc. All Rights Reserved.

#include "MVGameState.h"

//////////////////////////////////////////////////////////////////////////
// AMVGameState

AMVGameState::AMVGameState()
{
	
}


void AMVGameState::OnAllHospitalsDestroyed()
{

}

void AMVGameState::HandleMatchHasStarted()
{
	Super::HandleMatchHasEnded();

	OnGameStarted.Broadcast();
}

void AMVGameState::HandleMatchHasEnded()
{
	Super::HandleMatchHasEnded();

	OnGameFinished.Broadcast();
}