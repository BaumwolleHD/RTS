// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AIController.h"
#include "NpcController.generated.h"

UCLASS()
class RTS_API ANpcController : public AAIController
{
	GENERATED_BODY()
public:
	ANpcController();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = State)
		FString Task;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = State)
		FString Job;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = State)
		bool Moving;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = State)
		FVector TargetLocation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = State)
		AActor* TargetActor;


	//Items
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Items)
		int32 ItemID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Items)
		int32 ItemQuantity;


	//Public Targets
	void SetTarget(AActor* Target);
	void SetTaskToPickup(AActor* TargetResourceBuilding);
	void SetTaskToStorage();


private:
	//Checks
	bool IsTargetReached();

	//Tasks
	void MoveToTarget();
	int32 PickupItemsFromResorceBuilding();
	int32 PlaceItemsInStorage();

	//Services
	void FindNextTask();

};
