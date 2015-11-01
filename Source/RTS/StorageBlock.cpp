// Fill out your copyright notice in the Description page of Project Settings.

#include "RTS.h"
#include "StorageStack.h"
#include "Building.h"
#include "StorageBlock.h"
#include "DefaultPlayerstate.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerCharacter.h"

AStorageBlock::AStorageBlock()
{
	//Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	//RootComponent = Mesh;

	bReplicates = true;
}

void AStorageBlock::BeginPlay()
{
	Super::BeginPlay();
	StorageStacks.SetNum(SizeX*SizeY);

	//insert to character list
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(this->GetOwner());
	if (PlayerCharacter && (Role == ROLE_Authority || PlayerCharacter->IsRelevant))
	{
		PlayerCharacter->CurrentPlayerstate->OwnedStorageBlocks.Add(this);

	}

}


FVector AStorageBlock::GetItemPosition(int32 Index)
{
	FVector Location = GetActorLocation();
	Location.X += (Index % SizeX + 1) * 200 - 400;
	Location.Y += (Index / SizeY + 1) * 200 - 400;
	Location.Z += 5;
	return Location;
}