#pragma once

#include "GameFramework/Actor.h"
#include "StorageStack.generated.h"

UCLASS()
class RTSMEDIVALTEST_API AStorageStack : public AActor
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

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Visuals)
		TArray<UStaticMesh*> VisualMeshEven;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Visuals)
		TArray<UStaticMesh*> VisualMeshOdd;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Structure)
		int32 ItemsPerLayer;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Structure)
		float ItemHeight;

	void Sort();

	UFUNCTION(Reliable, NetMulticast)
		void SendStackUpdateToClients(int32 ServerQuantity);
	void SendStackUpdateToClients_Implementation(int32 ServerQuantity);

private:
	UStaticMesh* GetMeshByLayerItems(int32 LayerItems, int32 Layer);

};
