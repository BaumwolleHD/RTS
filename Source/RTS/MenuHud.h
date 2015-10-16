// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "Http.h"
#include "MenuHud.generated.h"


UCLASS()
class RTS_API UMenuHud : public UUserWidget
{
	GENERATED_BODY()
public:


	UFUNCTION(BlueprintCallable, Category = Login)
	bool UserRegister(const FString& UserName, const FString& Password);

	void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

private:
	
	//UPROPERTY()
	FHttpModule* Http;

	
};
