// Fill out your copyright notice in the Description page of Project Settings.

#include "RTS.h"
#include "BuildingExtension.h"


ABuildingExtension::ABuildingExtension()
{

}

void ABuildingExtension::BeginPlay()
{
	Super::BeginPlay();
}

void ABuildingExtension::SetMasterBuilding(AActor* Building)
{
	MasterBuilding = Building;
}