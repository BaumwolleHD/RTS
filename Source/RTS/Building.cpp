// Fill out your copyright notice in the Description page of Project Settings.

#include "RTS.h"
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
	RootComponent->ComponentTags.Add(FName("SelectAble"));
}


// Called when the game starts or when spawned
void ABuilding::BeginPlay()
{
	Super::BeginPlay();
	//float Scaling = Size.X*0.1;
	//SetActorScale3D(FVector(Scaling, Scaling, Scaling));
	if (Role == ROLE_Authority) {

		if (BuildProgressionState == 0) {
			BuildProgressionState = 0;
			SendBuildStateUpdateToClients(BuildProgression);
		}
	}


}

// Called every frame
void ABuilding::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Role == ROLE_Authority) {

		if (BuildProgressionState != BuildMeshes.Num())
		{
			BuildProgression = FMath::Min( BuildProgression+(1 / BuildTime) * DeltaTime, 1.f);
			int32 CurrentBuildProgressionState = CalculateState(BuildProgression, BuildMeshes.Num());
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
	return FMath::Max(1, FMath::CeilToInt((Prog + 0.01f)*(Steps - 1)));
}

void ABuilding::SendBuildStateUpdateToClients_Implementation(float Prog)
{
	if (Role != ROLE_Authority) {
		BuildProgression = Prog;
		BuildProgressionState = CalculateState(BuildProgression, BuildMeshes.Num());
	}
	if (BuildProgressionState == 1)
	{
		float Scaling = Size.X*0.1;
		SetActorScale3D(FVector(Scaling, Scaling, Scaling));
		SetActorRotation(FRotator(0.f, Rotation, 0.f));

	}
	if (BuildProgressionState > 0 && BuildProgressionState <= BuildMeshes.Num()) 
	{ 
		BaseMesh->SetStaticMesh(BuildMeshes[BuildProgressionState - 1]); 
		if (BuildProgressionState == BuildMeshes.Num()){ BuildingComplete(); }
		
	}
}

