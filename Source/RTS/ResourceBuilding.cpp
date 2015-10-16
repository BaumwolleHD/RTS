// Fill out your copyright notice in the Description page of Project Settings.

#include "RTS.h"
#include "ResourceBuilding.h"
#include "Core.h"
#include "Building.h"
#include "PlayerCharacter.h"
#include "NpcController.h"
#include "UnrealNetwork.h"

AResourceBuilding::AResourceBuilding()
{
	GrowMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GrowMesh"));
	GrowMesh->AttachTo(BaseMesh);

	bReplicates = true;
	GrowMeshes.SetNum(5);
	IsGrowing = false;
	PrimaryActorTick.bCanEverTick = true;
}

void AResourceBuilding::BeginPlay()
{
	Super::BeginPlay();
	if (Role == ROLE_Authority)
	{

		if (GrowProgressionState == 0) {
			GrowProgressionState = 0;
			SendGrowStateUpdateToClients(GrowProgression);
		}
	}
}

void AResourceBuilding::SetIsGrowingToClients_Implementation(bool b)
{
	IsGrowing = b;
}

void AResourceBuilding::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (IsGrowing && BuildProgressionState == 5 && Role == ROLE_Authority) {
		if (GrowProgressionState < 5)
		{
			GrowProgression += (1 / GrowTime) * DeltaTime;

			int32 CurrentGrowProgressionState = ABuilding::CalculateState(GrowProgression, 5);
			if (GrowProgressionState != CurrentGrowProgressionState) {
				SendGrowStateUpdateToClients(GrowProgression);
				
				GrowProgressionState = CurrentGrowProgressionState;
			}

		}
		else if (GrowProgressionState != 6)
		{
			APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetOwner());
			if (PlayerCharacter)
			{
				TArray<APawn*> FreeNpcs = PlayerCharacter->GetFreeNpcsByState(PlayerCharacter->StorageNpcs, "StorageWorker", "Free");
				if (FreeNpcs.Num() > 0)
				{
					ANpcController* const Npc = Cast<ANpcController>(FreeNpcs[0]->GetController());
					if (Npc)
					{		
						Npc->SetTaskToPickup(this);
						GrowProgressionState = 6;
					}
				}

			}
		}


	}
}

//
void AResourceBuilding::SendGrowStateUpdateToClients_Implementation(float Prog)
{
	if (Role != ROLE_Authority) {
		GrowProgression = Prog;
		GrowProgressionState = CalculateState(GrowProgression, 5);
	}
	if (GrowProgressionState >= 0 && GrowProgressionState < 5 && BuildProgressionState == 5){ GrowMesh->SetStaticMesh(GrowMeshes[GrowProgressionState]); }

}