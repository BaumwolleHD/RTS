// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Pawn.h"
#include "PlayerCharacter.generated.h"

UCLASS()
class RTS_API APlayerCharacter : public APawn
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
	virtual void Tick(float DeltaSeconds) override;

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


	//Buildings
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlayerInput)
		TSubclassOf<class ABuilding> SelectedBuilding;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlayerInput)
		TSubclassOf<class ABuildingPreview> BuildingPreviewObject;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlayerInput)
		TSubclassOf<class AStorageBlock> StorageBlockClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlayerInput)
		AActor* CurrentPreview;


	//Items
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = ItemClasses)
		TArray<TSubclassOf<class AStorageStack>> ItemClasses;



	//Player specific
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlayerInput)
		FString Mode;


	//Owned stuff
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Storage)
		TArray<AActor*> OwnedStorageBlocks;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Storage)
		TArray<AActor*> OwnedStorageBlock;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Storage)
		AActor* OwnedStorageBuilding;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = NPCs)
		TArray<APawn*> OwnedNpcs;

	//Npc Arrays
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = NPCs)
		TArray<APawn*> StorageNpcs;

	//Npc Networking
	TArray<APawn*> GetFreeNpcsByState(TArray<APawn*> NpcArray, FString Job, FString Task);


	//Synced stuff
	UPROPERTY()
		TMap<FString, FString> PP_BlockID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlayerInput)
		bool Paused;


	//Build Mode
	UFUNCTION(Reliable, Server, WithValidation) //Runs on Server
		void PlaceBuilding(FHitResult HitResult, APlayerCharacter* PlayerCharacter);
	void PlaceBuilding_Implementation(FHitResult HitResult, APlayerCharacter* PlayerCharacter);
	bool PlaceBuilding_Validate(FHitResult HitResult, APlayerCharacter* PlayerCharacter);

	UFUNCTION(Reliable, Server, WithValidation) // Sends block update request to server
		void UpdateBlockToServer(FVector2D Position, FVector2D Size, const FString& ID);
	void UpdateBlockToServer_Implementation(FVector2D Position, FVector2D Size, const FString& ID);
	bool UpdateBlockToServer_Validate(FVector2D Position, FVector2D Size, const FString& ID);

	UFUNCTION(Reliable, NetMulticast) // Sends block update to clients
		void UpdateBlockToClients(FVector2D Position, FVector2D Size, const FString& ID);
	void UpdateBlockToClients_Implementation(FVector2D Position, FVector2D Size, FString ID);

	void BuildingPreview();



	//Resorces
	void ChangeItem(int32 Quantity, int32 ID);


	//Mode Changes (Hud)
	UFUNCTION(BlueprintCallable, Category = PlayerModes)
	void SetModeToBuildExtend();

protected:
	void MouseMovement(float DeltaTime);
	float ScaleToViewportFloat(float in, float factor);
	void Pause();
	void OnLeftClick();
	FVector2D ApplyGrid(FVector2D Location, FVector2D Size);



};
