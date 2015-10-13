// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Pawn.h"
#include "PlayerCharacter.generated.h"

UCLASS()
class RTSMEDIVALTEST_API APlayerCharacter : public APawn
{
	GENERATED_BODY()
public:
	// Sets default values for this pawn's properties
	APlayerCharacter();

	FString SerializeFVector2D(FVector2D Vec2D);
	bool CanPlaceBuilding(FVector2D Position, FVector2D Size);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = PlayerInput)
		FHitResult MouseHitResult;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = PlayerInput)
		FVector2D MouseLocation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlayerSettings)
		float CameraMovementMargin;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlayerSettings)
		float CameraMaxSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlayerSettings)
		float CameraMinSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Map)
		FVector2D MapSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlayerInput)
		TSubclassOf<class ABuilding> SelectedBuilding;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlayerInput)
		TSubclassOf<class ABuildingPreview> BuildingPreviewObject;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlayerInput)
		TSubclassOf<class AStorageBlock> StorageBlockClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = ItemClasses)
		TArray<TSubclassOf<class AStorageStack>> ItemClasses;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlayerInput)
		AActor* CurrentPreview;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlayerInput)
		FString Mode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlayerInput)
		bool Paused;

	UPROPERTY() //Replicated)
		TMap<FString, FString> PP_BlockID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Storage)
		TArray<AStorageBlock*> OwnedStorageBlocks;


	UFUNCTION(Reliable, Server, WithValidation) //Runs on Server
		void PlaceBuilding(FHitResult HitResult, APlayerCharacter* PlayerCharacter);
	void PlaceBuilding_Implementation(FHitResult HitResult, APlayerCharacter* PlayerCharacter);
	bool PlaceBuilding_Validate(FHitResult HitResult, APlayerCharacter* PlayerCharacter);

	UFUNCTION(Reliable, Server, WithValidation) // Sends block update request to server
		void UpdateBlockToServer(FVector2D Position, FVector2D Size,const FString& ID);
	void UpdateBlockToServer_Implementation(FVector2D Position, FVector2D Size, const FString& ID);
	bool UpdateBlockToServer_Validate(FVector2D Position, FVector2D Size, const FString& ID);

	UFUNCTION(Reliable, NetMulticast) // Sends block update to clients
		void UpdateBlockToClients(FVector2D Position, FVector2D Size, const FString& ID);
	void UpdateBlockToClients_Implementation(FVector2D Position, FVector2D Size,FString ID);

	void BuildingPreview();


	void ChangeItem(int32 Quantity, int32 ID);



protected:
	//void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps);
	void MouseMovement(float DeltaTime);
	float ScaleToViewportFloat(float in, float factor);
	void Pause();
	void PlaceBuildingBind();
	FVector2D ApplyGrid(FVector2D Location, FVector2D Size);
};
