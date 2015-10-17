// Fill out your copyright notice in the Description page of Project Settings.

#include "RTS.h"
#include "MenuHud.h"
#include "Http.h"
#include "Json.h"
#include "SavingFile.h"

#define SALT "joxR9yjVXXR4D6Rb"

void UMenuHud::UserRegister(const FString& UserName, const FString& Password)
{

	if (UserName.Len() >= 4 && UserName.Len() <= 16 && Password.Len() >= 4 && Password.Len() <= 64)
	{
		Http = &FHttpModule::Get();
		if (!Http) return;
		if (!Http->IsHttpEnabled()) return;

		FString JsonString;
		//FString Salt = "joxR9yjVXXR4D6Rb";
		TSharedRef<TJsonWriter<TCHAR>> JsonWriter = TJsonWriterFactory<TCHAR>::Create(&JsonString);

		JsonWriter->WriteObjectStart();
		JsonWriter->WriteValue("User", UserName);
		JsonWriter->WriteValue("Pass", Password);
		JsonWriter->WriteValue("Secret", SALT);
		JsonWriter->WriteObjectEnd();
		JsonWriter->Close();

		TSharedRef < IHttpRequest > Request = Http->CreateRequest();
		Request->OnProcessRequestComplete().BindUObject(this, &UMenuHud::OnRegistrationResponseReceived);
		Request->SetURL("http://10.0.0.100/api/users/register.php");
		Request->SetVerb("POST");
		Request->SetHeader("Content-Type", "application/x-www-form-urlencoded");
		Request->SetContentAsString(JsonString);

		if (!Request->ProcessRequest())
		{
			LoginCompleteEvent(false, "Connection timed out!\nError Code: Http001");
		}
		return;
	}
	else
	{
		LoginCompleteEvent(false, "Entered Username/Password is to short/to long\nError Code: Http005");
		return;
	}
}
void UMenuHud::OnRegistrationResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		FString Content = Response->GetContentAsString();
		TSharedRef< TJsonReader<TCHAR> > Reader = TJsonReaderFactory<TCHAR>::Create(Content);
		TSharedPtr<FJsonObject> JsonParsed;
		if (FJsonSerializer::Deserialize(Reader, JsonParsed))
		{
			int32 Succesful = JsonParsed->GetIntegerField("bSuccesful");
			switch (Succesful)
			{
			case 0:
				LoginCompleteEvent(false, "An internal error occored!\nError Code: Http002");
				return;
			case 1:
				LoginCompleteEvent(false, "Username already in use!\nError Code: Http003");
				return;
			case 2:
				Token = JsonParsed->GetStringField("Token");
				LoginCompleteEvent(true, "Registration completed!\nYour Token is "+Token);
				USavingFile* SaveGameInstance = Cast<USavingFile>(UGameplayStatics::CreateSaveGameObject(USavingFile::StaticClass()));
				SaveGameInstance->PlayerName = "DefaultPlayer";
				SaveGameInstance->Token = Token;
				UGameplayStatics::SaveGameToSlot(SaveGameInstance, SaveGameInstance->SaveSlotName, SaveGameInstance->UserIndex);
				break;
			}
			
		}
		else 
		{
			LoginCompleteEvent(false, "An internal error occored!\nError Code: Http004");
		}
	}
}

void UMenuHud::UserLogin(const FString& UserName, const FString& Password)
{

	if (UserName.Len() >= 4 && UserName.Len() <= 16 && Password.Len() >= 4 && Password.Len() <= 64)
	{
		Http = &FHttpModule::Get();
		if (!Http) return;
		if (!Http->IsHttpEnabled()) return;

		FString JsonString;
		//FString Salt = "joxR9yjVXXR4D6Rb";
		TSharedRef<TJsonWriter<TCHAR>> JsonWriter = TJsonWriterFactory<TCHAR>::Create(&JsonString);

		JsonWriter->WriteObjectStart();
		JsonWriter->WriteValue("User", UserName);
		JsonWriter->WriteValue("Pass", Password);
		JsonWriter->WriteValue("Secret", SALT);
		JsonWriter->WriteObjectEnd();
		JsonWriter->Close();

		TSharedRef < IHttpRequest > Request = Http->CreateRequest();
		Request->OnProcessRequestComplete().BindUObject(this, &UMenuHud::OnLoginResponseReceived);
		Request->SetURL("http://10.0.0.100/api/users/login.php");
		Request->SetVerb("POST");
		Request->SetHeader("Content-Type", "application/x-www-form-urlencoded");
		Request->SetContentAsString(JsonString);

		if (!Request->ProcessRequest())
		{
			LoginCompleteEvent(false, "Connection timed out!\nError Code: Http009");
		}
		return;
	}
	else
	{
		LoginCompleteEvent(false, "Entered Username/Password is to short/to long:\nError Code: Http010");
		return;
	}
}
void UMenuHud::OnLoginResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		FString Content = Response->GetContentAsString();
		TSharedRef< TJsonReader<TCHAR> > Reader = TJsonReaderFactory<TCHAR>::Create(Content);
		TSharedPtr<FJsonObject> JsonParsed;
		if (FJsonSerializer::Deserialize(Reader, JsonParsed))
		{
			int32 Succesful = JsonParsed->GetIntegerField("bSuccesful");
			switch (Succesful)
			{
			case 0:
				LoginCompleteEvent(false, "An internal error occored!\nError Code: Http006");
				return;
			case 1:
				LoginCompleteEvent(false, "Invalid Username/Password!\nError Code: Http007");
				return;
			case 2:
				Token = JsonParsed->GetStringField("Token");
				LoginCompleteEvent(true, "Login completed!\nYour Token is " + Token);


				USavingFile* SaveGameInstance = Cast<USavingFile>(UGameplayStatics::CreateSaveGameObject(USavingFile::StaticClass()));
				SaveGameInstance->PlayerName = "DefaultPlayer";
				SaveGameInstance->Token = Token;
				UGameplayStatics::SaveGameToSlot(SaveGameInstance, SaveGameInstance->SaveSlotName, SaveGameInstance->UserIndex);

				break;
			}

		}
		else
		{
			LoginCompleteEvent(false, "An internal error occored!\nError Code: Http008");
		}
	}

	//Is registering animation stop
}

void UMenuHud::CreateLobby(const FString& LobbyName, const FString& Password)
{
	if (LobbyName.Len() >= 4 && LobbyName.Len() <= 16 && Password.Len() >= 4 && Password.Len() <= 64)
	{
		Http = &FHttpModule::Get();
		if (!Http) return;
		if (!Http->IsHttpEnabled()) return;

		FString JsonString;
		TSharedRef<TJsonWriter<TCHAR>> JsonWriter = TJsonWriterFactory<TCHAR>::Create(&JsonString);


		USavingFile* LoadGameInstance = Cast<USavingFile>(UGameplayStatics::CreateSaveGameObject(USavingFile::StaticClass()));
		LoadGameInstance = Cast<USavingFile>(UGameplayStatics::LoadGameFromSlot(LoadGameInstance->SaveSlotName, LoadGameInstance->UserIndex));
		FString CurrentToken = LoadGameInstance->Token;

		
		JsonWriter->WriteObjectStart();
		JsonWriter->WriteValue("GameName", LobbyName);
		JsonWriter->WriteValue("Password", Password);
		JsonWriter->WriteValue("Secret", SALT);
		JsonWriter->WriteValue("Token", CurrentToken);
		JsonWriter->WriteObjectEnd();
		JsonWriter->Close();

		TSharedRef < IHttpRequest > Request = Http->CreateRequest();
		Request->OnProcessRequestComplete().BindUObject(this, &UMenuHud::OnCreateLobbyResponseReceived);
		Request->SetURL("http://10.0.0.100/api/serverlist/creategame.php");
		Request->SetVerb("POST");
		Request->SetHeader("Content-Type", "application/x-www-form-urlencoded");
		Request->SetContentAsString(JsonString);

		if (!Request->ProcessRequest())
		{
			LoginCompleteEvent(false, "Connection timed out!\nError Code: Http013");
		}
		return;
	}
}

void UMenuHud::OnCreateLobbyResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		FString Content = Response->GetContentAsString();
		TSharedRef< TJsonReader<TCHAR> > Reader = TJsonReaderFactory<TCHAR>::Create(Content);
		TSharedPtr<FJsonObject> JsonParsed;
		if (FJsonSerializer::Deserialize(Reader, JsonParsed))
		{
			int32 Succesful = JsonParsed->GetIntegerField("bSuccesful");
			switch (Succesful)
			{
			case 0:
				LobbycreationCompleteEvent(false, "An internal error occored!\nError Code: Http011");
				return;
			case 1:
				LobbycreationCompleteEvent(false, "Lobby name already in use!\nError Code: Http012");
				return;
			case 2:
				LobbycreationCompleteEvent(true, "Lobby successfully created!");
				break;
			case 3:
				LobbycreationCompleteEvent(false, "You are already in a Lobby. \nPlease restart your Client.\nError Code: Http014");
				return;
			}

		}
		else
		{
			LobbycreationCompleteEvent(false, "An internal error occored!\nError Code: Http018");
		}

	}

}

void UMenuHud::RefreshLobbylist()
{
	Http = &FHttpModule::Get();
	if (!Http) return;
	if (!Http->IsHttpEnabled()) return;

	FString JsonString;
	TSharedRef<TJsonWriter<TCHAR>> JsonWriter = TJsonWriterFactory<TCHAR>::Create(&JsonString);


	USavingFile* LoadGameInstance = Cast<USavingFile>(UGameplayStatics::CreateSaveGameObject(USavingFile::StaticClass()));
	LoadGameInstance = Cast<USavingFile>(UGameplayStatics::LoadGameFromSlot(LoadGameInstance->SaveSlotName, LoadGameInstance->UserIndex));
	FString CurrentToken = LoadGameInstance->Token;

	JsonWriter->WriteObjectStart();
	JsonWriter->WriteValue("Secret", SALT);
	JsonWriter->WriteValue("Token", CurrentToken);
	JsonWriter->WriteObjectEnd();
	JsonWriter->Close();

	TSharedRef < IHttpRequest > Request = Http->CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &UMenuHud::RefreshLobbylistResponseReceived);
	Request->SetURL("http://10.0.0.100/api/serverlist/GetGameList.php");
	Request->SetVerb("POST");
	Request->SetHeader("Content-Type", "application/x-www-form-urlencoded");
	Request->SetContentAsString(JsonString);


	if (!Request->ProcessRequest())
	{
		RefreshLobbylistEvent(false, "Connection timed out!\nError Code: Http021");
	}
	return;
}

void UMenuHud::RefreshLobbylistResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		
		FString Content = Response->GetContentAsString();
		TSharedRef< TJsonReader<TCHAR> > Reader = TJsonReaderFactory<TCHAR>::Create(Content);
		TSharedPtr<FJsonObject> JsonParsed;
		UE_LOG(LogTemp, Warning, TEXT("Toke = %s"), *Content);
		if (FJsonSerializer::Deserialize(Reader, JsonParsed))
		{
			
			int32 Succesful = JsonParsed->GetIntegerField("bSuccesful");
			switch (Succesful)
			{
			case 0:
				RefreshLobbylistEvent(false, "An internal error occored!\nError Code: Http016");
				return;
			case 1:
				RefreshLobbylistEvent(false, "No available Lobbies!\nError Code: Http017");
				break;
			case 2:
				RefreshLobbylistEvent(true, "Lobby list refreshed: " + Content);
				//TSharedPtr<FJsonObject> Servers = JsonParsed->GetObjectField("Servers");
				TArray<TSharedPtr<FJsonValue>>*& Serverlist;
				JsonParsed->TryGetArrayField("Servers", Serverlist);
				
				for (int32 ServerIndex = 0; ServerIndex < Servers.Num(); ServerIndex++)
				{
					FString Name = Servers[ServerIndex]->AsString();
					UE_LOG(LogTemp, Warning, TEXT("%s"),*Name);


					/*FJsonObject* Server = Servers[ServerIndex]->AsObject().Get();
					FString Map = JsonParsed->GetStringField("Map");
					FString Password = JsonParsed->GetStringField("Password");
					FString Name = Servers[ServerIndex]->AsString();*/
					//UE_LOG(LogTemp, Warning, TEXT("%S %s %s"), *Map, *Password, *Name);
				}

				return;
			}
		}
		else
		{
			RefreshLobbylistEvent(false, "An internal error occored!\nError Code: Http020");
		}
	}

}