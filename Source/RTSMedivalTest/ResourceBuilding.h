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
class RTSMEDIVALTEST_API AResourceBuilding : public ABuilding
{
	GENERATED_BODY()
public:
	AResourceBuilding();

	virtual void BeginPlay();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BaseSetup)
		UStaticMeshComponent* GrowMesh;

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

	uint32 GrowProgressionState;


	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Production)
		TArray<UStaticMesh*> GrowMeshes;

	UFUNCTION(Reliable, NetMulticast)
		void SendGrowStateUpdateToClients(float Prog);
	void SendGrowStateUpdateToClients_Implementation(float Prog);
	UFUNCTION(Reliable, NetMulticast)
		void SetIsGrowingToClients(bool b);
	void SetIsGrowingToClients_Implementation(bool b);
private:
	
	virtual void Tick(float DeltaSeconds);
//	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps);
};
