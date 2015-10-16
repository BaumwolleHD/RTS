// Fill out your copyright notice in the Description page of Project Settings.

#include "RTS.h"
#include "MenuHud.h"
#include "Http.h"

bool UMenuHud::UserRegister(const FString& UserName, const FString& Password)
{
	

	Http = &FHttpModule::Get();
	if (!Http) return false;
	if (!Http->IsHttpEnabled()) return false;

	TSharedRef < IHttpRequest > Request = Http->CreateRequest();
	Request->SetVerb("POST");
	Request->SetURL("http://10.0.0.100/api/users/register.php");
	Request->SetHeader("User-Agent", "RTSClient/1.0");
	Request->SetHeader("Content-Type", "application/json");
	Request->SetContentAsString("{'0':'"+UserName+"','1':'"+Password+"','2':'joxR9yjVXXR4D6Rb'}");
	//Is registering animation start
	Request->OnProcessRequestComplete().BindUObject(this, &UMenuHud::OnResponseReceived);
	UE_LOG(LogTemp, Warning, TEXT("Request complete"));
	if (!Request->ProcessRequest())
	{
		UE_LOG(LogTemp, Warning, TEXT("Sucks ass"));
		//Is registering animation stop
	}
	return true;
}

void UMenuHud::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	UE_LOG(LogTemp, Warning, TEXT("Succses: %s"), *Response->GetContentAsString());
	
	//Is registering animation stop
}
