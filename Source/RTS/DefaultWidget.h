// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "DefaultWidget.generated.h"


UCLASS()
class RTS_API UDefaultWidget : public UUserWidget
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable, Category = Items)
		void UpdateItems(TArray<int32> Items);

	UFUNCTION(BlueprintImplementableEvent, Category = Items, meta = (Displayname = "OnUpdateItems"))
		void OnUpdateItems(const TArray<int32>& Items);

};
