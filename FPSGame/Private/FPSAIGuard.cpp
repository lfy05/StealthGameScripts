// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSAIGuard.h"
#include "AIModule/Classes/Perception/PawnSensingComponent.h"
#include "DrawDebugHelpers.h"
#include "RotationMatrix.h"
#include "FPSGameMode.h"
#include "AI/NavigationSystemBase.h"
#include "NavigationSystem/Public/NavigationSystem.h"
#include "AIModule/Classes/Blueprint/AIBlueprintHelperLibrary.h"
#include "UnrealNetwork.h"

// Sets default values
AFPSAIGuard::AFPSAIGuard()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComp"));
	PawnSensingComp->OnSeePawn.AddDynamic(this, &AFPSAIGuard::OnPawnSeen);

	GuardState = EAIState::IDLE;
}

// Called when the game starts or when spawned
void AFPSAIGuard::BeginPlay()
{
	Super::BeginPlay();
	
	// Always OnHearNoise.AddDynamic in BeginPlay
	
	PawnSensingComp->OnHearNoise.AddDynamic(this, &AFPSAIGuard::OnNoiseHeard);

	OriginalRotation = GetActorRotation();

	if (bPatrol) {
		MoveToNextPatrolPoint();
	}
}

void AFPSAIGuard::OnPawnSeen(APawn *SeenPawn) {
	if (SeenPawn == nullptr) {
		return;
	}

	AFPSGameMode *GM = Cast<AFPSGameMode>(GetWorld()->GetAuthGameMode());
	if (GM) {
		GM->CompleteMission(SeenPawn, false);
	}

	DrawDebugSphere(GetWorld(), SeenPawn->GetActorLocation(), 32.0f, 12, FColor::Red, false, 10.0f);
	UE_LOG(LogTemp, Log, TEXT("Seen Pawn"));

	SetGuardState(EAIState::ALERTED);

	AController *controller = GetController();
	if (controller) {
		controller->StopMovement();
	}
}

void AFPSAIGuard::OnNoiseHeard(APawn *NoiseInstigator, const FVector &Location, float Volume) {
	if (GuardState == EAIState::ALERTED) {
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("Heard Noise"));
	DrawDebugSphere(GetWorld(), Location, 32.0f, 12, FColor::Green, false, 10.0f);
	
	// rotate based on projectile noise
	FVector Direction = Location - GetActorLocation();
	Direction.Normalize();

	FRotator NewLookAt = FRotationMatrix::MakeFromX(Direction).Rotator();
	NewLookAt.Pitch = 0.0f;
	NewLookAt.Roll = 0.0f;
	SetActorRotation(NewLookAt);

	// set timer to return to original rotation after timeout
	GetWorldTimerManager().ClearTimer(TimerHandle_ResetOrientation);
	GetWorldTimerManager().SetTimer(TimerHandle_ResetOrientation, this, &AFPSAIGuard::ResetOrientation, 3.0f);

	SetGuardState(EAIState::SUSPICIOUS);

	AController *controller = GetController();
	if (controller) {
		controller->StopMovement();
	}
}

void AFPSAIGuard::ResetOrientation() {
	if (GuardState == EAIState::ALERTED) {
		return;
	}
	SetActorRotation(OriginalRotation);
	SetGuardState(EAIState::IDLE);

	if (bPatrol) {
		MoveToNextPatrolPoint();
	}
}

void AFPSAIGuard::OnRep_GuardState() {
	OnStateChanged(GuardState);
}

void AFPSAIGuard::SetGuardState(EAIState NewState) {
	if (GuardState == NewState) {
		return;
	}

	GuardState = NewState; // this is only on server
	// OnStateChanged(GuardState);	// this is set on server. It is called in the OnRep_GuardState;
	OnRep_GuardState();	// this is set on client
}

void AFPSAIGuard::MoveToNextPatrolPoint() {
	if (CurrentPatrolPoint == nullptr || CurrentPatrolPoint == SecondPatrolPoint) {
		CurrentPatrolPoint = FirstPatrolPoint;
	} else {
		CurrentPatrolPoint = SecondPatrolPoint;
	}

	UAIBlueprintHelperLibrary::SimpleMoveToActor(GetController(), CurrentPatrolPoint);
	UE_LOG(LogTemp, Log, TEXT("Moving to Next Patrol Point"));
}

// Called every frame
void AFPSAIGuard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CurrentPatrolPoint) {
		FVector Delta = GetActorLocation() - CurrentPatrolPoint->GetActorLocation();
		float DistanceToGoal = Delta.Size();

		// check if we are within 50 units of our goal, if so, pick a new patrol point
		if (DistanceToGoal < 0) {
			MoveToNextPatrolPoint();
		}
	}
}

void AFPSAIGuard::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AFPSAIGuard, GuardState);
}