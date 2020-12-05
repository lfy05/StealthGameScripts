// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Rotator.h"
#include "FPSAIGuard.generated.h"

class UPawnSensingComponent;

UENUM(BlueprintType)
enum class EAIState: uint8 {
	IDLE,
	SUSPICIOUS,
	ALERTED
};

UCLASS()
class FPSGAME_API AFPSAIGuard : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AFPSAIGuard();

protected:
	UPROPERTY(EditInstanceOnly, Category = "AI")
	bool bPatrol;

	UPROPERTY(EditInstanceOnly, Category = "AI", meta = (EditCondition="bPatrol"))
	AActor *FirstPatrolPoint;

	UPROPERTY(EditInstanceOnly, Category = "AI", meta = (EditCondition = "bPatrol"))
	AActor *SecondPatrolPoint;

	AActor *CurrentPatrolPoint;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Componenets")
	UPawnSensingComponent *PawnSensingComp;

	UFUNCTION()
	void OnPawnSeen(APawn *SeenPawn);

	UFUNCTION()
	void OnNoiseHeard(APawn* NoiseInstigator, const FVector& Location, float Volume);

	FRotator OriginalRotation;

	FTimerHandle TimerHandle_ResetOrientation;

	UFUNCTION()
	void ResetOrientation();

	UPROPERTY(ReplicatedUsing=OnRep_GuardState)
	EAIState GuardState;

	UFUNCTION()
	void OnRep_GuardState(); // called only on clients when GuardState changes.

	void SetGuardState(EAIState NewState);

	UFUNCTION(BlueprintImplementableEvent)
	void OnStateChanged(EAIState NewState);

	void MoveToNextPatrolPoint();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
