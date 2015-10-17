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
			LoginSuccessEvent(false, "Connection timed out!\nError Code: Http001");
		}
		return;
	}
	else
	{
		LoginSuccessEvent(false, "Entered Username/Password is to short/to long\nError Code: Http005");
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
				LoginSuccessEvent(false, "An internal error occored!\nError Code: Http002");
				return;
			case 1:
				LoginSuccessEvent(false, "Username already in use!\nError Code: Http003");
				return;
			case 2:
				Token = JsonParsed->GetStringField("Token");
				LoginSuccessEvent(true, "Registration completed!\nYour Token is "+Token);
				break;
			}
			
		}
		else 
		{
			LoginSuccessEvent(false, "An internal error occored!\nError Code: Http004");
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
			LoginSuccessEvent(false, "Connection timed out!\nError Code: Http009");
		}
		return;
	}
	else
	{
		LoginSuccessEvent(false, "Entered Username/Password is to short/to long:\nError Code: Http010");
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
				LoginSuccessEvent(false, "An internal error occored!\nError Code: Http006");
				return;
			case 1:
				LoginSuccessEvent(false, "Wrong Username/Password!\nError Code: Http007");
				return;
			case 2:
				Token = JsonParsed->GetStringField("Token");
				LoginSuccessEvent(true, "Login completed!\nYour Token is " + Token);


				USavingFile* SaveGameInstance = Cast<USavingFile>(UGameplayStatics::CreateSaveGameObject(USavingFile::StaticClass()));
				SaveGameInstance->PlayerName = "DefaultPlayer";
				SaveGameInstance->Token = Token;
				UGameplayStatics::SaveGameToSlot(SaveGameInstance, SaveGameInstance->SaveSlotName, SaveGameInstance->UserIndex);

				break;
			}

		}
		else
		{
			LoginSuccessEvent(false, "An internal error occored!\nError Code: Http008");
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
		Request->OnProcessRequestComplete().BindUObject(this, &UMenuHud::OnLoginResponseReceived);
		Request->SetURL("http://10.0.0.100/api/users/login.php");
		Request->SetVerb("POST");
		Request->SetHeader("Content-Type", "application/x-www-form-urlencoded");
		Request->SetContentAsString(JsonString);

		if (!Request->ProcessRequest())
		{
			//LoginSuccessEvent(false, "Connection timed out!\nError Code: Http009");
		}
		return;
	}
}