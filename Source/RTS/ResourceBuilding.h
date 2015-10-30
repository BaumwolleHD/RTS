// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Core.h"
#include "Engine.h"
#include "UnrealNetwork.h"
#include "Building.h"
#include "ResourceBuilding.generated.h"

/**
*
*/
UCLASS()
class RTS_API AResourceBuilding : public ABuilding
{
	GENERATED_BODY()
public:
	AResourceBuilding();

	virtual void BeginPlay();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BaseSetup)
		UStaticMeshComponent* GrowMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BaseSetup)
		UStaticMeshComponent* ConsumeMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Production)
		float GrowProgression;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Production)
		float GrowTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Production)
		bool IsGrowing;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Production)
		int32 ProductionQuantity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Production)
		int32 ProductionID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Consumption)
		int32 ConsumptionQuantity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Consumption)
		int32 ConsumptionID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Consumption)
		int32 CurrentConsumptionQuantity;


	int32 GrowProgressionState;

	int32 ConsumeProgressionState;


	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Production)
		TArray<UStaticMesh*> GrowMeshes;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Consumption)
		TArray<UStaticMesh*> ConsumeMeshes;

	UFUNCTION(Reliable, NetMulticast)
		void SendGrowStateUpdateToClients(float Prog);
	void SendGrowStateUpdateToClients_Implementation(float Prog);
	UFUNCTION(Reliable, NetMulticast)
		void SetIsGrowingToClients(bool b);
	void SetIsGrowingToClients_Implementation(bool b);

	UFUNCTION(Reliable, NetMulticast)
		void SendConsumeStateUpdateToClients(float Prog);
	void SendConsumeStateUpdateToClients_Implementation(float Prog);

private:

	virtual void Tick(float DeltaSeconds);
	//	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps);
};
