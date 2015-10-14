// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AIController.h"
#include "NPC.generated.h"

UCLASS()
class RTS_API ANPC : public AAIController
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ANPC();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Task)
		FString Task;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Task)
		bool Moving;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Task)
		FVector TargetLocation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Task)
		AActor* TargetActor;

	
	//Items
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Items)
		int32 ItemID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Items)
		int32 ItemQuantity;


	//Public Targets
	void SetTarget(AActor* Target);
	void SetTaskToPickup(AActor* TargetResourceBuilding);

private:
	//Checks
	bool IsTargetReached();

	//Tasks
	void MoveToTarget();
	int32 PickupItemsFromResorceBuilding();

};
