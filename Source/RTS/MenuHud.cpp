// Fill out your copyright notice in the Description page of Project Settings.

#include "RTS.h"
#include "MenuHud.h"
#include "Http.h"
#include "Json.h"

bool UMenuHud::UserRegister(const FString& UserName, const FString& Password)
{

	if (UserName.Len() >= 4 && UserName.Len() <= 16 && Password.Len() >= 4 && Password.Len() <= 64)
	{
		Http = &FHttpModule::Get();
		if (!Http) return false;
		if (!Http->IsHttpEnabled()) return false;

		FString JsonString;
		FString Salt = "joxR9yjVXXR4D6Rb";
		TSharedRef<TJsonWriter<TCHAR>> JsonWriter = TJsonWriterFactory<TCHAR>::Create(&JsonString);

		JsonWriter->WriteObjectStart();
		JsonWriter->WriteValue("User", UserName);
		JsonWriter->WriteValue("Pass", Password);
		JsonWriter->WriteValue("Secret", Salt);
		JsonWriter->WriteObjectEnd();
		JsonWriter->Close();

		TSharedRef < IHttpRequest > Request = Http->CreateRequest();
		Request->OnProcessRequestComplete().BindUObject(this, &UMenuHud::OnRegistrationResponseReceived);
		Request->SetURL("http://10.0.0.100/api/users/register.php");
		Request->SetVerb("POST");
		Request->SetHeader("Content-Type", "application/x-www-form-urlencoded");
		Request->SetContentAsString(JsonString);
		//Is registering animation start

		if (!Request->ProcessRequest())
		{
			UE_LOG(LogTemp, Error, TEXT("Connection timed out! Error Code: Http001"));
			//Is registering animation stop
		}
		return true;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Entered Username/Password is to short/to long: Http005"));
		return false;
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
				UE_LOG(LogTemp, Error, TEXT("An internal error occored! Error Code: Http002"));
				return;
			case 1:
				UE_LOG(LogTemp, Error, TEXT("Username already in use! Error Code: Http003"));
				return;
			case 2:
				Token = JsonParsed->GetStringField("Token");
				UE_LOG(LogTemp, Warning, TEXT("Registration completed! Your Token is: %s"),*Token);
				break;
			}
			
		}
		else 
		{
			UE_LOG(LogTemp, Error, TEXT("An internal error occored! Error Code: Http004"));
		}
	}
	
	//Is registering animation stop
}

bool UMenuHud::UserLogin(const FString& UserName, const FString& Password)
{

	if (UserName.Len() >= 4 && UserName.Len() <= 16 && Password.Len() >= 4 && Password.Len() <= 64)
	{
		Http = &FHttpModule::Get();
		if (!Http) return false;
		if (!Http->IsHttpEnabled()) return false;

		FString JsonString;
		FString Salt = "joxR9yjVXXR4D6Rb";
		TSharedRef<TJsonWriter<TCHAR>> JsonWriter = TJsonWriterFactory<TCHAR>::Create(&JsonString);

		JsonWriter->WriteObjectStart();
		JsonWriter->WriteValue("User", UserName);
		JsonWriter->WriteValue("Pass", Password);
		JsonWriter->WriteValue("Secret", Salt);
		JsonWriter->WriteObjectEnd();
		JsonWriter->Close();

		TSharedRef < IHttpRequest > Request = Http->CreateRequest();
		Request->OnProcessRequestComplete().BindUObject(this, &UMenuHud::OnLoginResponseReceived);
		Request->SetURL("http://10.0.0.100/api/users/login.php");
		Request->SetVerb("POST");
		Request->SetHeader("Content-Type", "application/x-www-form-urlencoded");
		Request->SetContentAsString(JsonString);
		//Is registering animation start

		if (!Request->ProcessRequest())
		{
			UE_LOG(LogTemp, Error, TEXT("Connection timed out! Error Code: Http001"));
			//Is registering animation stop
		}
		return true;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Entered Username/Password is to short/to long: Http005"));
		return false;
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
				UE_LOG(LogTemp, Error, TEXT("An internal error occored! Error Code: Http006"));
				return;
			case 1:
				UE_LOG(LogTemp, Error, TEXT("Username already in use! Error Code: Http007"));
				return;
			case 2:
				Token = JsonParsed->GetStringField("Token");
				UE_LOG(LogTemp, Warning, TEXT("Login completed! Your Token is: %s"), *Token);
				break;
			}

		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("An internal error occored! Error Code: Http008"));
		}
	}

	//Is registering animation stop
}