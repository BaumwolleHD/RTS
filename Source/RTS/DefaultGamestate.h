// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameState.h"
#include "DefaultGamestate.generated.h"

/**
 * 
 */
UCLASS()
class RTS_API ADefaultGamestate : public AGameState
{
	GENERATED_BODY()
public:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;



	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Users)
	TArray<AActor*> PlayerStateList;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Users)
		TArray<AActor*> PlayerCharacterList;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Buildings)
		TArray<TSubclassOf<class ABuilding>> BuildingList;

	UPROPERTY()
		TMap<FString, FString> PP_BlockID;



	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Buildings)
		AActor* CurrentPreview;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Buildings)
		TSubclassOf<class ABuildingPreview> BuildingPreviewObject;
	
	
};
