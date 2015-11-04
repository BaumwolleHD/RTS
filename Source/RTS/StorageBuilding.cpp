// Fill out your copyright notice in the Description page of Project Settings.

#include "RTS.h"
#include "StorageBuilding.h"
#include "PlayerCharacter.h"


// Sets default values
AStorageBuilding::AStorageBuilding()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AStorageBuilding::BeginPlay()
{
	Super::BeginPlay();

	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(this->GetOwner());
	if (PlayerCharacter && (Role == ROLE_Authority || PlayerCharacter->IsRelevant))
	{
		PlayerCharacter->CurrentPlayerstate->OwnedStorageBuilding = this;

	}
	
}

// Called every frame
void AStorageBuilding::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

