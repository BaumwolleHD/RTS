// Fill out your copyright notice in the Description page of Project Settings.

#include "RTS.h"
#include "UnrealNetwork.h"
#include "DefaultPlayerstate.h"
#include "PlayerCharacter.h"
#include "Building.h"
#include "ResourceBuilding.h"
#include "BuildingPreview.h"
#include "StorageBlock.h"
#include "StorageStack.h"
#include "NpcCharacter.h"
#include "NpcController.h"
#include "BuildingExtension.h"
#include "Defaultgamemode.h"
#include "DefaultGamestate.h"
#include "DefaultPlayerstate.h"
#include "Engine.h"
#include "Blueprint/UserWidget.h"


void ADefaultPlayerstate::BeginPlay()
{
	Super::BeginPlay();

}

inline static bool SortNpcPredicate(const int32& TaskCount1, int32& TaskCount2)
{
	return (TaskCount1 > TaskCount2);
}

void ADefaultPlayerstate::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	ANpcController* NpcController;

	if (NpcSortTimer <= 0) 
	{
		TaskPriorities.Empty();
		NpcSortTimer = NpcSortFrequency;
		
		for (int32 NpcSortIndex = 0; NpcSortIndex < OwnedNpcs.Num(); NpcSortIndex++)
		{
			if (OwnedNpcs[NpcSortIndex]->IsValidLowLevel())
			{
				NpcController = Cast<ANpcController>(OwnedNpcs[NpcSortIndex]->GetController());
				if (NpcController && NpcController->Job == ENpcJob::StorageWorker)
				{
					TaskPriorities.Add(NpcSortIndex + NpcController->Tasks.Num() * 1000);
				}
			}
		}
		TaskPriorities.Sort();

		for (int32 TaskPriority = 0; TaskPriority<TaskPriorities.Num(); TaskPriority++)
		{
			TaskPriorities[TaskPriority] -= FMath::Floor(TaskPriorities[TaskPriority]/1000)*1000;
			UE_LOG(LogTemp, Warning, TEXT("TaskPriority: %d"), TaskPriorities[TaskPriority]);
		}
			
	}
	else
	{
		NpcSortTimer -= DeltaTime;
	}

	//int32 End;

	int32 StartNpc = 0;
	int32 EndNpc = 0;
	int32 PawnCount = OwnedNpcs.Num();
	int32 NpcsToIterate = 0;

	if (NpcProgression >= PawnCount) {
		NpcProgression = 0;
	}

	
	if (PawnCount >= 1 && PawnCount <= 100)
	{
		if (PawnCount < 20) {
			NpcsToIterate = PawnCount;
		}
		else {
			NpcsToIterate = 20;
		}
	}
	else if (PawnCount > 100)
	{
		NpcsToIterate = FMath::Min(PawnCount / 5, 50);
	}

	
	StartNpc = FMath::Min(NpcProgression, OwnedNpcs.Num());
	EndNpc = FMath::Min(StartNpc + NpcsToIterate, OwnedNpcs.Num());
	NpcProgression += NpcsToIterate + 1;

	//UE_LOG(LogTemp, Warning, TEXT("%d Npc: %d - %d, %d"), NpcsToIterate, StartNpc, EndNpc, int(1 / DeltaTime));

	for (int32 NpcIndex = StartNpc; NpcIndex < EndNpc; NpcIndex++)
	{

		NpcController = Cast<ANpcController>(OwnedNpcs[NpcIndex]->GetController());
		if (NpcController)
		{
			if (FVector::Dist(NpcController->TargetLocation, NpcController->GetPawn()->GetActorLocation()) < 250.f || NpcController->Tasks.Num() > 0 && NpcController->Tasks[0] == ENpcTask::Free && NpcController->Tasks.Num() > 1)
			{

				NpcController->bTargetReached = true;
				NpcController->bMoving = false;

				APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(NpcController->GetPawn()->GetOwner());
				AResourceBuilding* ResourceBuilding;
				switch (NpcController->Tasks[0])
				{
				case ENpcTask::Free:
					break;

				case ENpcTask::PickupItemsFromBuilding:


					ResourceBuilding = Cast<AResourceBuilding>(NpcController->TargetActors[0]);
					if (ResourceBuilding)
					{
						NpcController->CarriedItemID = ResourceBuilding->ProductionID;
						NpcController->CarriedItemQuantity = FMath::Min(ResourceBuilding->CurrentProductionQuantity, NpcController->MaxCarriedItemQuantity);
						ResourceBuilding->CurrentProductionQuantity -= NpcController->CarriedItemQuantity;
						ResourceBuilding->GrowProgression = 0.f;
						ResourceBuilding->GrowProgressionState = 0;
						ResourceBuilding->CalledNpc = false;


					}
					break;



				case ENpcTask::DropItemsToStorage:
					if (PlayerCharacter){ PlayerCharacter->ChangeItem(NpcController->CarriedItemID, NpcController->CarriedItemQuantity); }
					NpcController->CarriedItemID = NpcController->CarriedItemQuantity = 0;
					//Task = ENpcTask::Free;
					break;

				case ENpcTask::PickupItemsFromStorage:
					NpcController->CarriedItemID = NpcController->NeededItemID;
					NpcController->CarriedItemQuantity = FMath::Min(FMath::Min(NpcController->NeededItemQuantity, PlayerCharacter->CheckForQuantity(NpcController->NeededItemID)), NpcController->MaxCarriedItemQuantity);
					NpcController->NeededItemID = NpcController->NeededItemQuantity = 0;
					if (PlayerCharacter){ PlayerCharacter->ChangeItem(NpcController->CarriedItemID, -NpcController->CarriedItemQuantity); }
					break;


				case ENpcTask::DropItemsToBuilding:
					ResourceBuilding = Cast<AResourceBuilding>(NpcController->TargetActors[0]);
					if (ResourceBuilding)
					{

						int32 OldConsumption;
						OldConsumption = ResourceBuilding->CurrentConsumptionQuantity;
						//ResourceBuilding->CurrentConsumptionQuantity = FMath::Min(ResourceBuilding->ConsumptionQuantity, ResourceBuilding->CurrentConsumptionQuantity + CarriedItemQuantity);


						if (Role == ROLE_Authority)
						{
							ResourceBuilding->SendConsumeStateUpdateToClients(float(FMath::Min(ResourceBuilding->ConsumptionQuantity, ResourceBuilding->CurrentConsumptionQuantity + NpcController->CarriedItemQuantity) - 1) / ResourceBuilding->ConsumptionQuantity);
							ResourceBuilding->CalledNpc = false;
						}
						ResourceBuilding->GrowProgressionState = 0;
						
						NpcController->CarriedItemQuantity -= ResourceBuilding->CurrentConsumptionQuantity - OldConsumption;
						if (NpcController->CarriedItemQuantity <= 0)
						{
							NpcController->CarriedItemID = NpcController->CarriedItemQuantity = 0;
						}
						else
						{
							NpcController->AddTask(ENpcTask::DropItemsToStorage, PlayerCharacter->OwnedStorageBuilding, ENpcTaskPriority::Urgent);
							UE_LOG(LogTemp, Warning, TEXT("fuck dis shiat"));
						}
					}
					break;



				}

				NpcController->Tasks.RemoveAt(0);
				if (NpcController->Tasks.Num() == 0)
				{
					NpcController->Tasks.Add(ENpcTask::Free);
				}
				else
				{


					if (NpcController->TargetActors.Num() == 0)
					{
						NpcController->bTargetReached = true;
						return;
					}
					else
					{
						NpcController->TargetActors.RemoveAt(0);
					}


					NpcController->bTargetReached = false;
					switch (NpcController->Tasks[0])
					{
					case ENpcTask::Free:
						NpcController->bTargetReached = true;
						return;

					case ENpcTask::DropItemsToBuilding:
						NpcController->TargetLocation = NpcController->TargetActors[0]->GetActorLocation();
						break;

					case ENpcTask::DropItemsToStorage:
						NpcController->TargetLocation = FVector(0, 0, 0);
						if (NpcController->TargetActors[0] != NULL)
						{
							NpcController->TargetLocation = NpcController->TargetActors[0]->GetActorLocation();
						}
						break;

					case ENpcTask::PickupItemsFromBuilding:
						NpcController->TargetLocation = NpcController->TargetActors[0]->GetActorLocation();
						break;

					case ENpcTask::PickupItemsFromStorage:
						NpcController->TargetLocation = FVector(0, 0, 0);
						if (NpcController->TargetActors[0] != NULL)
						{
							NpcController->TargetLocation = NpcController->TargetActors[0]->GetActorLocation();
						}
						break;


					case ENpcTask::GetBuildingConsumption:
						ResourceBuilding = Cast<AResourceBuilding>(NpcController->TargetActors[0]);
						if (ResourceBuilding)
						{

							NpcController->NeededItemID = ResourceBuilding->ConsumptionID;
							NpcController->NeededItemQuantity = ResourceBuilding->ConsumptionQuantity - ResourceBuilding->CurrentConsumptionQuantity;


							//TargetActor = PlayerCharacter->OwnedStorageBuilding;
							NpcController->TargetLocation = NpcController->GetPawn()->GetActorLocation();
						}

						break;



					}

				}


			}
			else if (!NpcController->bTargetReached && !NpcController->bMoving)
			{
				APawn* const Pawn = NpcController->GetPawn();
				if (Pawn)
				{

					UNavigationSystem* const NavSys = GetWorld()->GetNavigationSystem();
					float const Distance = FVector::Dist(NpcController->TargetLocation, Pawn->GetActorLocation());

					if (NavSys && Distance > 150.f)
					{
						NpcController->MoveToLocation(NpcController->TargetLocation, 150.f);
						NpcController->bMoving = true;
					}
				}
			}
		}
	}
}


APawn* ADefaultPlayerstate::GetStorageWorker()
{
	if (TaskPriorities.Num() > 0)
	{
		NpcSortTimer = 0;
		return OwnedNpcs[TaskPriorities[0]];

	}

	return NULL;

}

void ADefaultPlayerstate::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to everyone
	DOREPLIFETIME(ADefaultPlayerstate, CurrentUserIndex);
	DOREPLIFETIME(ADefaultPlayerstate, OwnedBuildings);
	DOREPLIFETIME(ADefaultPlayerstate, OwnedItems);
	DOREPLIFETIME(ADefaultPlayerstate, OwnedNpcs);



}

TArray<APawn*> ADefaultPlayerstate::GetNpcsByState(TArray<APawn*> NpcArray, ENpcJob Job, ENpcTask Task)
{
	TArray<APawn*> ReturnedActors;
	for (int32 Index = 0; Index < NpcArray.Num(); Index++)
	{
		ANpcController* const Npc = Cast<ANpcController>(NpcArray[Index]->GetController());
		if (Npc)
		{
			if (Task == ENpcTask::Empty && Npc->Tasks[0] == Task || Job == ENpcJob::Empty && Npc->Job == Job || Npc->Tasks[0] == Task && Npc->Job == Job)
			{
				ReturnedActors.Add(NpcArray[Index]);
			}
		}

	}
	return ReturnedActors;
}