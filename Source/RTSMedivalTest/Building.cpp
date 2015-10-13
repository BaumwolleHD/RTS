// Fill out your copyright notice in the Description page of Project Settings.

#include "RTSMedivalTest.h"
#include "Building.h"
#include "Core.h"
#include "Engine.h"
#include "UnrealNetwork.h"

// Sets default values
ABuilding::ABuilding()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	
	//BaseMesh->SetMobility(EComponentMobility::Movable);
	Health = 100.f;
	BuildTime = 0.f;
	BuildMeshes.SetNum(5);
	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
	RootComponent = BaseMesh;
}


// Called when the game starts or when spawned
void ABuilding::BeginPlay()
{
	Super::BeginPlay();
	SetActorScale3D(FVector(Size.X, Size.X, Size.X));
	if (Role == ROLE_Authority) {

		if (BuildProgressionState == 0) {
			BuildProgressionState = 0;
			SendBuildStateUpdateToClients(BuildProgression);
		}	
	}

	
}

// Called every frame
void ABuilding::Tick( float DeltaTime )
{
	Super::Tick(DeltaTime);

	if (Role == ROLE_Authority) {
		
		if (BuildProgressionState != 5)
		{
			BuildProgression += (1 / BuildTime) * DeltaTime;
			int32 CurrentBuildProgressionState = CalculateState(BuildProgression, 5);
			if (BuildProgressionState != CurrentBuildProgressionState) {
				BuildProgressionState = CurrentBuildProgressionState;
				SendBuildStateUpdateToClients(BuildProgression);
			}

		}


	}

}

bool ABuilding::CheckIfActorHasStaticMesh(AActor* Actor, UStaticMesh* StaticMesh) const
{
	if (Actor == nullptr || StaticMesh == nullptr) { return false; }

	TArray<UStaticMeshComponent*>  StaticMeshComponents;
	Actor->GetComponents(StaticMeshComponents);

	if (StaticMeshComponents.Num() > 0)
	{
		for (UStaticMeshComponent* smc : StaticMeshComponents)
		{
			if (smc->StaticMesh == StaticMesh)
			{
				return true;
			}
		}
	}

	return false;
}

int32 ABuilding::CalculateState(float Prog, int32 Steps)
{
	return FMath::Max(1, FMath::CeilToInt((Prog+0.001f)*(Steps-1))); //To beoptimised
}

void ABuilding::SendBuildStateUpdateToClients_Implementation(float Prog)
{
	if (Role != ROLE_Authority) {
		BuildProgression = Prog;
		BuildProgressionState = CalculateState(BuildProgression, 5);
	}
	if (BuildProgressionState > 0 && BuildProgressionState <= 5) { BaseMesh->SetStaticMesh(BuildMeshes[BuildProgressionState - 1]); }
}

