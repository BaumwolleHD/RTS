// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "BuildingPreview.generated.h"

UCLASS()
class RTSMEDIVALTEST_API ABuildingPreview : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABuildingPreview();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Visuals)
		UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Visuals)
		UMaterialInstanceDynamic* Material;

};
