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
	void GetUserData();
	void SetUserData();




	virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime) override;
	virtual void Construct();




	UFUNCTION(BlueprintCallable, Category = Login)
	void UserRegister(const FString& UserName, const FString& Password);
	void OnRegistrationResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	UFUNCTION(BlueprintCallable, Category = Login)
	void UserLogin(const FString& UserName, const FString& Password);
	void OnLoginResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	UFUNCTION(BlueprintImplementableEvent, Category = Login, meta = (DisplayName = "LoginCompleteEvent"))
		void LoginCompleteEvent(bool bSuccess, const FString& Note);





	UFUNCTION(BlueprintCallable, Category = Login)
		void UserLogout(const FString& UserName, const FString& Password);

	UFUNCTION(BlueprintImplementableEvent, Category = Login, meta = (DisplayName = "LogoutCompleteEvent"))
	void LogoutCompleteEvent(bool bSuccess, const FString& Note);





	UFUNCTION(BlueprintCallable, Category = Lobby)
		void CreateLobby(const FString& LobbyName, const FString& Password);
	void OnCreateLobbyResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	UFUNCTION(BlueprintImplementableEvent, Category = LobbyManegment, meta = (DisplayName = "LobbycreationCompleteEvent"))
		void LobbycreationCompleteEvent(bool bSuccess, const FString& Note);





	UFUNCTION(BlueprintCallable, Category = Lobby)
		void RefreshLobbylist();
	void RefreshLobbylistResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	UFUNCTION(BlueprintImplementableEvent, Category = LobbyManegment, meta = (DisplayName = "RefreshLobbylistEvent"))
		void RefreshLobbylistEvent(bool bSuccess, const FString& Note);

	UFUNCTION(BlueprintImplementableEvent, Category = LobbyManegment, meta = (DisplayName = "AddLobbytoLobbylist"))
		void AddLobbytoLobbylist(const FString& Name, const FString& Password, const FString& Map);

	UFUNCTION(BlueprintImplementableEvent, Category = LobbyManegment, meta = (DisplayName = "ClearLobbylistEvent"))
		void ClearLobbylistEvent();





	UFUNCTION(BlueprintCallable, Category = Lobby)
		void JoinLobby(const FString& LobbyName, const FString& Password);
	void JoinLobbyResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	UFUNCTION(BlueprintImplementableEvent, Category = LobbyManegment, meta = (DisplayName = "JoinLobbyEvent"))
		void JoinLobbyEvent(bool bSuccess, const FString& Note, const FString& Lobbyname, const FString& Map);

	UFUNCTION(BlueprintImplementableEvent, Category = LobbyManegment, meta = (DisplayName = "AddUsertoUserlist"))
		void AddUsertoUserlist(const FString& Name, int32 PermissionLevel);

	UFUNCTION(BlueprintImplementableEvent, Category = LobbyManegment, meta = (DisplayName = "ClearUserlistEvent"))
		void ClearUserlistEvent();





	UFUNCTION(BlueprintImplementableEvent, Category = LobbyManegment, meta = (DisplayName = "NoteEvent"))
		void NoteEvent(const FString& Mode, const FString& Note);

	float HeartbeatTimer;

	bool IsConnected;

	UPROPERTY()
		FString Token;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Server)
		FString Ip;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Server)
		FString Salt;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = User)
		FString CurrentUserName;

	FSocket* CurrentSocket;


	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Server)
		bool ListenToServer;


	
	FSocket* CreateSocket(const FString& Ip, int32 Port);

	FString StringFromBinaryArray(const TArray<uint8>& BinaryArray);
	
	FHttpModule* Http;

	
};
