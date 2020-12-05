// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSObjectiveActor.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "FPSCharacter.h"
#include "GameFramework/Actor.h"

// Sets default values
AFPSObjectiveActor::AFPSObjectiveActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent = MeshComp;

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	SphereComp->SetupAttachment(MeshComp);

	SetReplicates(true);
}

// Called when the game starts or when spawned
void AFPSObjectiveActor::BeginPlay()
{
	Super::BeginPlay();

	PlayEffects();
	
}

void AFPSObjectiveActor::PlayEffects() {
	UGameplayStatics::SpawnEmitterAtLocation(this, PickupFX, GetActorLocation());
}


void AFPSObjectiveActor::NotifyActorBeginOverlap(AActor *OtherActor) {
	Super::NotifyActorBeginOverlap(OtherActor);

	UE_LOG(LogTemp, Log, TEXT("Collieded"));

	PlayEffects();

	AFPSCharacter *myCharacter = Cast<AFPSCharacter>(OtherActor);
	if (myCharacter) {
		myCharacter->bIsCarryingObjective = true; // this is only on the server (without replication)
		Destroy();
	}
}

