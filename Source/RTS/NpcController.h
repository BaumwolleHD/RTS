#pragma once

#include "AIController.h"
#include "NpcController.generated.h"


UENUM(BlueprintType)
enum class ENpcJob : uint8
{
	Empty 	UMETA(DisplayName = "Empty"),
	StorageWorker 	UMETA(DisplayName = "StorageWorker"),
	Begger 	UMETA(DisplayName = "Begger"),
};

UENUM(BlueprintType)
enum class ENpcTask : uint8
{
	Empty 	UMETA(DisplayName = "Empty"),
	Free 	UMETA(DisplayName = "Free"),
	PickupItemsFromStorage 	UMETA(DisplayName = "PickupItemsFromStorage"),
	DropItemsToStorage 	UMETA(DisplayName = "DropItemsToStorage"),
	PickupItemsFromBuilding 	UMETA(DisplayName = "PickupItemsFromBuilding"),
	DropItemsToBuilding 	UMETA(DisplayName = "DropItemsToBuilding"),

	GetBuildingConsumption 	UMETA(DisplayName = "GetBuildingConsumption"),



};

UENUM(BlueprintType)
enum class ENpcTaskPriority : uint8
{
	Low 	UMETA(DisplayName = "Low"),
	Urgent 	UMETA(DisplayName = "Urgent"),

};


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
		TArray<ENpcTask> Tasks;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = State)
		ENpcJob Job;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = State)
		bool Moving;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = State)
		FVector TargetLocation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = State)
		TArray<AActor*> TargetActors;


	//Items
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Items)
		int32 CarriedItemID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Items)
		int32 CarriedItemQuantity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Items)
		int32 MaxCarriedItemQuantity;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Items)
		int32 NeededItemID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Items)
		int32 NeededItemQuantity;


	//Public Targets
	void AddTask(ENpcTask Task, AActor* TargetActor, ENpcTaskPriority Priority = ENpcTaskPriority::Low);


private:
	//Checks
	bool IsTargetReached();

	//Tasks
	void MoveToTarget();

	//Services
	void FindNextTask();

};