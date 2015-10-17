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
	void UserRegister(const FString& UserName, const FString& Password);
	void OnRegistrationResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	UFUNCTION(BlueprintCallable, Category = Login)
	void UserLogin(const FString& UserName, const FString& Password);
	void OnLoginResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	UFUNCTION(BlueprintImplementableEvent, Category = LobbyManegment, meta = (Friendlyname = "LoginSuccessEvent"))
		void LoginSuccessEvent(bool bSuccess, const FString& Note);

	UFUNCTION(BlueprintCallable, Category = Lobby)
		void CreateLobby(const FString& LobbyName, const FString& Password);


	UPROPERTY()
		FString Token;

private:
	
	FHttpModule* Http;

	
};
