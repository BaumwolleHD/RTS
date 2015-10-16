// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Building.h"
#include "StorageBuilding.generated.h"

UCLASS()
class RTS_API AStorageBuilding : public ABuilding
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AStorageBuilding();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	
	
};
