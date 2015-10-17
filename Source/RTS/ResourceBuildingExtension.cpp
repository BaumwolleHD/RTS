// Fill out your copyright notice in the Description page of Project Settings.

#include "RTS.h"
#include "Building.h"
#include "ResourceBuilding.h"
#include "ResourceBuildingExtension.h"




AResourceBuildingExtension::AResourceBuildingExtension()
{
	GrowMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GrowMesh"));
	GrowMesh->AttachTo(BaseMesh);

	bReplicates = true;
	GrowMeshes.SetNum(5);
	IsGrowing = false;
}

void AResourceBuildingExtension::BeginPlay()
{
	Super::BeginPlay();

}

void AResourceBuildingExtension::SetMasterBuilding(AActor* Building)
{
	//Super::SetMasterBuilding(Building);
	AResourceBuilding* const ResourceBuilding = Cast<AResourceBuilding>(Building);
	if (ResourceBuilding)
	{
		ResourceBuilding->ProductionQuantity += ProductionEnhancement;
	}
}