// Fill out your copyright notice in the Description page of Project Settings.

#include "RTS.h"
#include "Building.h"
#include "BuildingPreview.h"
#include "Kismet/GameplayStatics.h"
#include "DefaultGamestate.h"


void ADefaultGamestate::BeginPlay()
{
	Super::BeginPlay();

	
	//UWorld* World = UGameplayStatics::GetPlayerController(this, 0)->GetWorld();
	UWorld* World = GetWorld();
	if (World && BuildingPreviewObject != nullptr)
	{
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.Owner = this;
		SpawnParameters.Instigator = Instigator;
		CurrentPreview = World->SpawnActor<ABuildingPreview>(BuildingPreviewObject, FVector(0.f, 0.f, 0.f), FRotator(0.f, 0.f, 0.f), SpawnParameters);
	}

}

