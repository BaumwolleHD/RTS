// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Pawn.h"
#include "NpcController.h"
#include "DefaultGamestate.h"
#include "DefaultPlayerstate.h"
#include "PlayerCharacter.generated.h"


UENUM(BlueprintType)
enum class EPlayerMode : uint8
{
	Select 	UMETA(DisplayName = "Select"),
	Build 	UMETA(DisplayName = "Build"),
	BuildExtension 	UMETA(DisplayName = "BuildExtension"),
};


UCLASS()
class RTS_API APlayerCharacter : public APawn
{
	GENERATED_BODY()
public:
	// Sets default values for this pawn's properties
	APlayerCharacter();

	FString SerializeFVector2D(FVector2D Vec2D);
	bool CanPlaceBuilding(FVector2D Position, FVector2D Size);
	bool CanPlaceBuildingExtension(FVector2D Position, AActor* MasterBuilding);

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


	//Id
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Identity)
		bool IsRelevant;

	//Buildings
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BuildMode)
		TSubclassOf<class ABuilding> SelectedBuilding;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BuildMode)
		TSubclassOf<class ABuildingPreview> BuildingPreviewObject;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BuildMode)
		TSubclassOf<class AStorageBlock> StorageBlockClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BuildMode)
		AActor* CurrentPreview;



	//Items
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = ItemClasses)
		TArray<TSubclassOf<class AStorageStack>> ItemClasses;



	//Player specific
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlayerInput)
		EPlayerMode Mode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlayerInput)
		AActor* SelectedActor;


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
	TArray<APawn*> GetNpcsByState(TArray<APawn*> NpcArray, ENpcJob Job, ENpcTask Task);


	//Synced stuff
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gamemode)
		ADefaultGamestate* CurrentGamestate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gamemode)
		ADefaultPlayerstate* CurrentPlayerstate;


	/*UPROPERTY()
		TMap<FString, FString> PP_BlockID;*/

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlayerInput)
		bool Paused;


	//Build Mode
	UFUNCTION(Reliable, Server, WithValidation) //Runs on Server
		void PlaceBuilding(FHitResult HitResult, TSubclassOf<ABuilding> Building);
	void PlaceBuilding_Implementation(FHitResult HitResult, TSubclassOf<ABuilding> Building);
	bool PlaceBuilding_Validate(FHitResult HitResult, TSubclassOf<ABuilding> Building);


	UFUNCTION(Reliable, NetMulticast) // Sends block update to clients
	void UpdateBlocks(FVector2D Position, FVector2D Size, const FString& ID);
	void UpdateBlocks_Implementation(FVector2D Position, FVector2D Size, const FString& ID);


	

	void BuildingPreview();



	//Resorces
	UFUNCTION(Reliable, Server, WithValidation) //Runs on Server
		void ChangeItem(int32 ID, int32 Quantity);
	void ChangeItem_Implementation(int32 ID, int32 Quantity);
	bool ChangeItem_Validate(int32 ID, int32 Quantity);

	void UpdateQuantity();



	int32 CheckForQuantity(int32 ID);


	//Mode Changes (Hud)
	UFUNCTION(BlueprintCallable, Category = PlayerModes)
	void SetModeToBuildExtend();

	//Hud
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Hud)
		TSubclassOf<class UUserWidget> HudWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Hud)
	class UUserWidget* CurrentWidget;

protected:
	void MouseMovement(float DeltaTime);
	float ScaleToViewportFloat(float in, float factor);
	void Pause();
	void OnLeftClick();
	void OnMouseWheelUp();
	void OnMouseWheelDown();
	FVector2D ApplyGrid(FVector2D Location, FVector2D Size);
	void SelectActor();






};
