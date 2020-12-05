// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "FPSGameState.generated.h"

/**
 * Container for everything to be replicated in game mode
 */
UCLASS()
class FPSGAME_API AFPSGameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	UFUNCTION(NetMulticast, Reliable) // if this function is called on server, it is ran on every client
	void MulticastOnMissionComplete(APawn *InstigatorPawn, bool bMissionSuccess);
};
