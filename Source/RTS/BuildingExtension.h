// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Building.h"
#include "BuildingExtension.generated.h"


UCLASS()
class RTS_API ABuildingExtension : public ABuilding
{
	GENERATED_BODY()
	
public:
	ABuildingExtension();

	virtual void BeginPlay();

	void SetMasterBuilding(AActor* Building);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BaseSetup)
		AActor* MasterBuilding;
	
	
};
