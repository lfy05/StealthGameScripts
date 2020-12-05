// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSGameState.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Actor.h"
#include "FPSPlayerController.h"

void AFPSGameState::MulticastOnMissionComplete_Implementation(APawn *InstigatorPawn, bool MissionSuccess) {
	for (FConstPlayerControllerIterator it = GetWorld()->GetPlayerControllerIterator(); it; it++) {
		AFPSPlayerController *PC = Cast<AFPSPlayerController>(it->Get());
		if (PC && PC->IsLocalController()) {	
			PC->OnMissionCompleted(GetInstigator(), MissionSuccess);
			APawn *myPawn = PC->GetPawn();
			if (myPawn) {
				myPawn->DisableInput(nullptr);
			}
		}
	}
}	
