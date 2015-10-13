#pragma once

#include "GameFramework/Actor.h"
#include "StorageBlock.generated.h"



UCLASS()
class RTSMEDIVALTEST_API AStorageBlock : public ABuilding
{
	GENERATED_BODY()
public:
	AStorageBlock();

	virtual void BeginPlay() override;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = BaseSetup)
	//	UStaticMeshComponent* Mesh;


	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Size)
		int32 SizeX;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Size)
		int32 SizeY;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = ItemClasses)
		TArray<AActor*> StorageStacks;

	UPROPERTY()
		TMap<FString, int32> StorageIndexes;

	//UFUNCTION(BlueprintCallable, Category = Items)
	//	bool AddStorageStack(int32 Type, int32 Quantity);

	FVector GetItemPosition(int32 Index);
};