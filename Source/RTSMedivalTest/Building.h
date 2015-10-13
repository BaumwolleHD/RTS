// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Core.h"
#include "Engine.h"
#include "UnrealNetwork.h"
#include "Building.generated.h"




UCLASS()
class RTSMEDIVALTEST_API ABuilding : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABuilding();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

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
		uint32 BuildProgressionState;



	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Building)
		TArray<UStaticMesh*> BuildMeshes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BaseSetup)
		float Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BaseSetup)
		FVector2D Size;

	UFUNCTION(Reliable, NetMulticast)
		void SendBuildStateUpdateToClients(float Prog);
	void SendBuildStateUpdateToClients_Implementation(float Prog);

	int32 CalculateState(float Prog, int32 Steps);
};
