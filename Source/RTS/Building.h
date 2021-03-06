// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Core.h"
#include "Engine.h"
#include "UnrealNetwork.h"
#include "Building.generated.h"




UCLASS()
class RTS_API ABuilding : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABuilding();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	bool CheckIfActorHasStaticMesh(AActor* Actor, UStaticMesh*) const;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = BaseSetup)
		UStaticMeshComponent* BaseMesh;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Building)
		float BuildProgression;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Building)
		float BuildTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Building)
		int32 ID;
	UPROPERTY()
		int32 BuildProgressionState;


	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Interface)
		FName Name;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Interface)
		UTexture2D* Icon;


	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Building)
		TArray<UStaticMesh*> BuildMeshes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BaseSetup)
		float Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BaseSetup)
		FVector2D Size;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Transforms)
		int32 Rotation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Extension)
		TSubclassOf<class ABuilding> ExtensionBuildingClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Extension)
		TArray<AActor*> ExtensionBuildings;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Extension)
		int32 MaxExtensionDistance;

	UFUNCTION(Reliable, NetMulticast)
		void SendBuildStateUpdateToClients(float Prog);
	void SendBuildStateUpdateToClients_Implementation(float Prog);

	int32 CalculateState(float Prog, int32 Steps);

	UFUNCTION(BlueprintImplementableEvent, Category = Building)
		void BuildingComplete();
};
