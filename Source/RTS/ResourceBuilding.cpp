// Fill out your copyright notice in the Description page of Project Settings.

#include "RTS.h"
#include "ResourceBuilding.h"
#include "Core.h"
#include "Building.h"
#include "PlayerCharacter.h"
#include "NpcController.h"
#include "ResourceBuildingExtension.h"
#include "UnrealNetwork.h"

AResourceBuilding::AResourceBuilding()
{
	GrowMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GrowMesh"));
	GrowMesh->AttachTo(BaseMesh);

	ConsumeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ConsumeMesh"));
	ConsumeMesh->AttachTo(BaseMesh);

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
	
	if (Role == ROLE_Authority && BuildProgressionState == 5) 
	{
		APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetOwner());

		if (IsGrowing && GrowProgressionState < GrowMeshes.Num())
		{
			GrowProgression = FMath::Min(GrowProgression + (1 / GrowTime) * DeltaTime, 1.f);

			int32 CurrentGrowProgressionState = CalculateState(GrowProgression, GrowMeshes.Num());
			if (GrowProgressionState != CurrentGrowProgressionState) 
			{
				SendGrowStateUpdateToClients(GrowProgression);
				GrowProgressionState = CurrentGrowProgressionState;
			}


		

			int32 CurrentConsumeProgressionState = CalculateState(1 - GrowProgression, ConsumeMeshes.Num());
			if (GrowProgressionState != CurrentConsumeProgressionState)
			{
				SendGrowStateUpdateToClients(1 - GrowProgression);
				//ConsumeProgressionState = CurrentConsumeProgressionState;
			}


		}
		else if (GrowProgressionState == 0 && (ConsumptionQuantity == 0 || CurrentConsumptionQuantity == ConsumptionQuantity ))
		{
			UE_LOG(LogTemp, Warning, TEXT("Start"));
			IsGrowing = true;
		}
		else if (GrowProgressionState != GrowMeshes.Num() + 1 && (GrowProgressionState == GrowMeshes.Num() || GrowProgressionState == 0 && CurrentConsumptionQuantity < ConsumptionQuantity && PlayerCharacter->CheckForQuantity(ConsumptionID) >= ConsumptionQuantity - CurrentConsumptionQuantity))
		{
			if (PlayerCharacter)
			{
				TArray<APawn*> FreeNpcs = PlayerCharacter->GetNpcsByState(PlayerCharacter->StorageNpcs, ENpcJob::StorageWorker, ENpcTask::Free);
				if (FreeNpcs.Num() > 0)
				{
					ANpcController* const Npc = Cast<ANpcController>(FreeNpcs[0]->GetController());
					if (Npc)
					{
						if (GrowProgressionState == GrowMeshes.Num())
						{
							Npc->AddTask(ENpcTask::PickupItemsFromBuilding, this);
							Npc->AddTask(ENpcTask::DropItemsToStorage, PlayerCharacter->OwnedStorageBuilding);
						}
						else
						{
							Npc->AddTask(ENpcTask::PickupItemsFromStorage, PlayerCharacter->OwnedStorageBuilding);
							Npc->AddTask(ENpcTask::DropItemsToBuilding, this);

							Npc->NeededItemID = ConsumptionID;
							Npc->NeededItemQuantity = ConsumptionQuantity - CurrentConsumptionQuantity;
						}
						IsGrowing = false;
						GrowProgressionState = GrowMeshes.Num() + 1;
					}
				}
			}
		}
	}
}

void AResourceBuilding::SendConsumeStateUpdateToClients_Implementation(float Prog)
{
	ConsumeProgressionState = CalculateState(Prog, ConsumeMeshes.Num());
	if (ConsumeProgressionState >= 0 && ConsumeProgressionState < ConsumeMeshes.Num() && BuildProgressionState == 5)
	{
		ConsumeMesh->SetStaticMesh(ConsumeMeshes[ConsumeProgressionState]);
	}
}
void AResourceBuilding::SendGrowStateUpdateToClients_Implementation(float Prog)
{
	if (Role != ROLE_Authority) {
		GrowProgression = Prog;
		GrowProgressionState = CalculateState(GrowProgression, GrowMeshes.Num());
	}
	if (GrowProgressionState >= 0 && GrowProgressionState < GrowMeshes.Num() && BuildProgressionState == 5)
	{ 
		GrowMesh->SetStaticMesh(GrowMeshes[GrowProgressionState]);
		for (int32 Index = 0; Index < ExtensionBuildings.Num(); Index++)
		{
			AResourceBuildingExtension* const Extension = Cast<AResourceBuildingExtension>(ExtensionBuildings[Index]);
			if (Extension)
			{
				if (Extension->IsGrowing)
				{
					Extension->GrowMesh->SetStaticMesh(Extension->GrowMeshes[GrowProgressionState]);
				}
				else if (GrowProgressionState == 0 && Extension->BuildProgressionState == 5)
				{
					Extension->IsGrowing = true;
					Extension->SetMasterBuilding(this);
				}
			}

		}
	}

}