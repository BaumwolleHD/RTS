// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "DefaultWidget.generated.h"


UENUM(BlueprintType)
enum class ECurrentContextWidget : uint8
{
	Empty 	UMETA(DisplayName = "Empty"),
	BuildingOutliner 	UMETA(DisplayName = "BuildingOutliner"),
	BuildMode 	UMETA(DisplayName = "BuildMode"),
};


UCLASS()
class RTS_API UDefaultWidget : public UUserWidget
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable, Category = Items)
		void UpdateItems(TArray<int32> Items);

	UFUNCTION(BlueprintCallable, Category = Items)
		AActor* BPGetDefaultObject(TSubclassOf<AActor> Class);

	UFUNCTION(BlueprintImplementableEvent, Category = Items, meta = (Displayname = "OnUpdateItems"))
		void OnUpdateItems(const TArray<int32>& Items);

};
