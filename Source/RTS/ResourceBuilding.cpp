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
	
	if (Role == ROLE_Authority && BuildProgressionState == BuildMeshes.Num())
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
			if (GrowProgressionState == GrowMeshes.Num())
			{
				CurrentProductionQuantity = ProductionQuantity;
				IsGrowing = false;
			}


			if (ConsumptionQuantity > 0)
			{
				int32 CurrentConsumeProgressionState = CalculateState(1 - GrowProgression, ConsumeMeshes.Num()) - 1;
				if (ConsumeProgressionState != CurrentConsumeProgressionState)
				{
					SendConsumeStateUpdateToClients(1 - GrowProgression);

				}
				if (CurrentGrowProgressionState == GrowMeshes.Num())
				{
					CurrentConsumptionQuantity = 0;
				}

			}

		}
		else if (GrowProgressionState == 0 && CurrentProductionQuantity == 0 && (ConsumptionQuantity == 0 || CurrentConsumptionQuantity == ConsumptionQuantity))
		{
			IsGrowing = true;
			SendGrowStateUpdateToClients(0.f);
		}
		else if (!CalledNpc)
		{
			if (GrowProgressionState == GrowMeshes.Num() || GrowProgressionState == 0 && CurrentConsumptionQuantity < ConsumptionQuantity && PlayerCharacter->CheckForQuantity(ConsumptionID) >= ConsumptionQuantity - CurrentConsumptionQuantity || CurrentProductionQuantity > 0)
			{

				if (PlayerCharacter)
				{
					APawn* FreeNpc = PlayerCharacter->CurrentPlayerstate->GetStorageWorker();
					if (FreeNpc != NULL)
					{
						ANpcController* const Npc = Cast<ANpcController>(FreeNpc->GetController());
						if (Npc)
						{
							if (CurrentProductionQuantity > 0)
							{
								if (ConsumptionQuantity > 0 && CurrentConsumptionQuantity < ConsumptionQuantity)
								{
									Npc->AddTask(ENpcTask::GetBuildingConsumption, this);
									Npc->AddTask(ENpcTask::PickupItemsFromStorage, PlayerCharacter->CurrentPlayerstate->OwnedStorageBuilding);
									Npc->AddTask(ENpcTask::DropItemsToBuilding, this);
								}
								Npc->AddTask(ENpcTask::PickupItemsFromBuilding, this);
								Npc->AddTask(ENpcTask::DropItemsToStorage, PlayerCharacter->CurrentPlayerstate->OwnedStorageBuilding);
							}
							else
							{
								Npc->AddTask(ENpcTask::GetBuildingConsumption, this);
								Npc->AddTask(ENpcTask::PickupItemsFromStorage, PlayerCharacter->CurrentPlayerstate->OwnedStorageBuilding);
								Npc->AddTask(ENpcTask::DropItemsToBuilding, this);
								UE_LOG(LogTemp, Warning, TEXT("npc called"));

							}
							IsGrowing = false;
							CalledNpc = true;

						}
					}
				}
			}	
		}
	}
}

void AResourceBuilding::SendConsumeStateUpdateToClients_Implementation(float Prog)
{
	ConsumeProgressionState = CalculateState(Prog, ConsumeMeshes.Num()) - 1;
	CurrentConsumptionQuantity = FMath::Min(CalculateState(Prog, ConsumptionQuantity+1), ConsumptionQuantity);
	if (ConsumeProgressionState >= 0 && ConsumeProgressionState < ConsumeMeshes.Num() && BuildProgressionState == BuildMeshes.Num())
	{
		ConsumeMesh->SetStaticMesh(ConsumeMeshes[ConsumeProgressionState]);
		
	}
}
void AResourceBuilding::SendGrowStateUpdateToClients_Implementation(float Prog)
{
	UE_LOG(LogTemp, Warning, TEXT("lol %d %f"), GrowProgressionState, Prog);
	if (GrowProgressionState == GrowMeshes.Num() - 1)
	{
		GrowingComplete();
	}
	else if ((GrowProgressionState == 0 && Role == ROLE_Authority || GrowProgressionState == 1) && Prog > 0.f)
	{
		UE_LOG(LogTemp, Warning, TEXT("Hey"));
		GrowingStarted();
	}
	if (Role != ROLE_Authority) {
		GrowProgression = Prog;
		GrowProgressionState = CalculateState(GrowProgression, GrowMeshes.Num());
	}

	if (GrowProgressionState >= 0 && GrowProgressionState < GrowMeshes.Num() && BuildProgressionState == BuildMeshes.Num())
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
				else if (GrowProgressionState == 0 && Extension->BuildProgressionState == BuildMeshes.Num())
				{
					Extension->IsGrowing = true;
					Extension->SetMasterBuilding(this);
				}
			}

		}
	}

}