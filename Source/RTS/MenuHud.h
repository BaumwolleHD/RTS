// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "Http.h"
#include "Networking.h"
#include "MenuHud.generated.h"


UCLASS()
class RTS_API UMenuHud : public UUserWidget
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable, Category = Login)
	void GetServerData();


	UFUNCTION(BlueprintCallable, Category = Login)
	void UserRegister(const FString& UserName, const FString& Password);
	void OnRegistrationResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	UFUNCTION(BlueprintCallable, Category = Login)
	void UserLogin(const FString& UserName, const FString& Password);
	void OnLoginResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	UFUNCTION(BlueprintImplementableEvent, Category = Login, meta = (Friendlyname = "LoginCompleteEvent"))
		void LoginCompleteEvent(bool bSuccess, const FString& Note);


	UFUNCTION(BlueprintCallable, Category = Login)
		void UserLogout(const FString& UserName, const FString& Password);
	//void OnLoginResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	UFUNCTION(BlueprintImplementableEvent, Category = Login, meta = (Friendlyname = "LogoutCompleteEvent"))
	void LogoutCompleteEvent(bool bSuccess, const FString& Note);


	UFUNCTION(BlueprintCallable, Category = Lobby)
		void CreateLobby(const FString& LobbyName, const FString& Password);
	void OnCreateLobbyResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	UFUNCTION(BlueprintImplementableEvent, Category = LobbyManegment, meta = (Friendlyname = "LobbycreationCompleteEvent"))
		void LobbycreationCompleteEvent(bool bSuccess, const FString& Note);



	UFUNCTION(BlueprintCallable, Category = Lobby)
		void RefreshLobbylist();
	void RefreshLobbylistResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	UFUNCTION(BlueprintImplementableEvent, Category = LobbyManegment, meta = (Friendlyname = "RefreshLobbylistEvent"))
		void RefreshLobbylistEvent(bool bSuccess, const FString& Note);

	UFUNCTION(BlueprintImplementableEvent, Category = LobbyManegment, meta = (Friendlyname = "AddLobbytoLobbylist"))
		void AddLobbytoLobbylist(const FString& Name, const FString& Password, const FString& Map);

	UFUNCTION(BlueprintImplementableEvent, Category = LobbyManegment, meta = (Friendlyname = "ClearLobbylistEvent"))
		void ClearLobbylistEvent();



	UFUNCTION(BlueprintCallable, Category = Lobby)
		void JoinLobby(const FString& LobbyName, const FString& Password);
	void JoinLobbyResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	UFUNCTION(BlueprintImplementableEvent, Category = LobbyManegment, meta = (Friendlyname = "JoinLobbyEvent"))
		void JoinLobbyEvent(bool bSuccess, const FString& Note, const FString& Lobbyname, const FString& Map);



	UPROPERTY()
		FString Token;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Server)
		FString Ip;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Server)
		FString Salt;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = User)
		FString CurrentUserName;





private:
	
	FHttpModule* Http;

	
};
