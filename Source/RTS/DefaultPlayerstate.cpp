// Fill out your copyright notice in the Description page of Project Settings.

#include "RTS.h"
#include "UnrealNetwork.h"
#include "DefaultPlayerstate.h"
#include "Building.h"
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