#pragma once

#include "GameFramework/Actor.h"
#include "StorageStack.generated.h"

UCLASS()
class RTS_API AStorageStack : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AStorageStack();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Storage)
		int32 Quantity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Storage)
		int32 MaxQuantity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Storage)
		int32 Type;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = BaseSetup)
		USceneComponent* Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = BaseSetup)
		UStaticMeshComponent* Mesh0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = BaseSetup)
		UStaticMeshComponent* Mesh1;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = BaseSetup)
		UStaticMeshComponent* Mesh2;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = BaseSetup)
		UStaticMeshComponent* Mesh3;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = BaseSetup)
		UStaticMeshComponent* Mesh4;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = BaseSetup)
		UStaticMeshComponent* Mesh5;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = BaseSetup)
		UStaticMeshComponent* Mesh6;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = BaseSetup)
		UStaticMeshComponent* Mesh7;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = BaseSetup)
		UStaticMeshComponent* Mesh8;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = BaseSetup)
		UStaticMeshComponent* Mesh9;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = BaseSetup)
		TArray<UStaticMeshComponent*> Meshes;


	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Visuals)
		TArray<UStaticMesh*> VisualMeshes;
	//UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Visuals)
	//	TArray<UStaticMesh*> VisualMeshOdd;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Structure)
		int32 ItemsPerLayer;


	void Sort();

	UFUNCTION(Reliable, NetMulticast)
		void SendStackUpdateToClients(int32 ServerQuantity);
	void SendStackUpdateToClients_Implementation(int32 ServerQuantity);


	//Construct Meshes
	UFUNCTION(BlueprintCallable, Category = PreSorting)
		void ConstructMeshes(UStaticMesh* Mesh);

	//Construct Location
	UFUNCTION(BlueprintCallable, Category = PreSorting)
		void ConstructLocationByConstantOffset(FVector Offset);

	UFUNCTION(BlueprintCallable, Category = PreSorting)
		void ConstructLocationByOffsetPerLayer(FVector Offset);

	UFUNCTION(BlueprintCallable, Category = PreSorting)
		void ConstructLocationByPyramide(FVector ItemSize, FVector ItemSizeDeviation, int32 FloorItems, int32 ItemSubstractionPerLayer, FRotator RotationOffsetPerLayer, FRotator RotationOffsetDeviation);

	//Construct Rotation
	UFUNCTION(BlueprintCallable, Category = PreSorting)
		void ConstructRotationByRotator(FRotator Offset);

	//Construct Scale
	UFUNCTION(BlueprintCallable, Category = PreSorting)
		void ConstructScaleByVector(FVector Scale);

	UFUNCTION(BlueprintCallable, Category = PreSorting)
		void ConstructScaleByFloat(float Scale);

private:
	UStaticMesh* GetMeshByLayerItems(int32 LayerItems, int32 Layer);



};
