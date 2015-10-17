// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BuildingExtension.h"
#include "ResourceBuildingExtension.generated.h"


UCLASS()
class RTS_API AResourceBuildingExtension : public ABuildingExtension
{
	GENERATED_BODY()

public:
	AResourceBuildingExtension();

	virtual void BeginPlay();

	void SetMasterBuilding(AActor* Building);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BaseSetup)
		UStaticMeshComponent* GrowMesh;


	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Production)
		TArray<UStaticMesh*> GrowMeshes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Production)
		bool IsGrowing;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Production)
		int32 ProductionEnhancement;
	
	
};
