// Fill out your copyright notice in the Description page of Project Settings.

#include "RTS.h"
#include <string>
#include "MenuHud.h"
#include "Http.h"
#include "Json.h"
#include "SavingFile.h"
#include "PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"

//USavingFile* LoadGameInstance = Cast<USavingFile>(UGameplayStatics::CreateSaveGameObject(USavingFile::StaticClass()));
//LoadGameInstance = Cast<USavingFile>(UGameplayStatics::LoadGameFromSlot(LoadGameInstance->SaveSlotName, LoadGameInstance->UserIndex));
//FString CurrentToken = LoadGameInstance->Token;


void UMenuHud::GetServerData()
{
	//APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
	UMenuHud* Widget = Cast<UMenuHud>(Cast<APlayerCharacter>(UGameplayStatics::GetPlayerPawn(this, 0))->CurrentWidget);
	Ip = Widget->Ip;
	Salt = Widget->Salt;
}

void UMenuHud::GetUserData()
{
	UMenuHud* Widget = Cast<UMenuHud>(Cast<APlayerCharacter>(UGameplayStatics::GetPlayerPawn(this, 0))->CurrentWidget);
	CurrentUserName = Widget->CurrentUserName;
	Token = Widget->Token;
}

void UMenuHud::SetUserData()
{
	UMenuHud* Widget = Cast<UMenuHud>(Cast<APlayerCharacter>(UGameplayStatics::GetPlayerPawn(this, 0))->CurrentWidget);
	Widget->CurrentUserName = CurrentUserName;
	Widget->Token = Token;
}

FString UMenuHud::StringFromBinaryArray(const TArray<uint8>& BinaryArray)
{
	//Create a string from a byte array!
	const std::string cstr(reinterpret_cast<const char*>(BinaryArray.GetData()), BinaryArray.Num());

	//FString can take in the c_str() of a std::string
	return FString(cstr.c_str());
}

void UMenuHud::Construct()
{
	Super::Construct();

	GetServerData();

}

void UMenuHud::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	Super::NativeTick(MyGeometry, DeltaTime);
	if (ListenToServer && CurrentSocket != NULL)
	{

		//Heartbeat
		HeartbeatTimer -= DeltaTime;
		if (HeartbeatTimer <= 0)
		{
			FString Serialized = TEXT("Heartbeat|");
			TCHAR *SerializedChar = Serialized.GetCharArray().GetData();
			int32 Size = FCString::Strlen(SerializedChar);
			int32 Sent = 0;
			bool SendSuccessful = CurrentSocket->Send((uint8*)TCHAR_TO_UTF8(SerializedChar), Size, Sent);
			if (SendSuccessful)
			{
				if (!IsConnected) 
				{
					NoteEvent("Success", "Reconnected to server");
				}
				IsConnected = true;
				GetUserData();
				HeartbeatTimer = 5;
				UE_LOG(LogTemp, Warning, TEXT("Heartbeat"))
			}
			else if (IsConnected)
			{
				IsConnected = false;
				HeartbeatTimer = 1.5f;
				NoteEvent("Fail", "Lost connection to server\nError Code: Tcp001");
			}
			else
			{
				HeartbeatTimer = 1.5f;
				NoteEvent("Blink", "Trying to reconnect...");
			}
			

		}
		
		//Listener
		uint32 PendingDataSize;
		while (CurrentSocket != NULL && CurrentSocket->HasPendingData(PendingDataSize))
		{
			TArray<uint8> Data;
			Data.SetNumUninitialized(FMath::Min(PendingDataSize, 65507u));
			int32 BytesRead;
			bool ReceivingSuccesful = CurrentSocket->Recv(Data.GetData(), 1024, BytesRead, ESocketReceiveFlags::None);

			if (ReceivingSuccesful && BytesRead > 0)
			{
				FString ReceivedString = StringFromBinaryArray(Data);
				if (ReceivedString != "0")
				{
					UE_LOG(LogTemp, Warning, TEXT("Received String: %s"), *ReceivedString);
					if (ReceivedString.Left(4) == "Join")
					{
						FString LeftString = ReceivedString.RightChop(5) + ";";
						FString CurrentData;
						int32 CurrentIndex = 0;
						int32 Success = -1;
						FString PlayerList;
						FString LobbyName;
						FString LobbyMap;
						while (LeftString.Split(";", &CurrentData, &LeftString))
						{
							if (CurrentIndex > 10)
							{
								break;
							}
							
							switch (CurrentIndex)
							{
							case 0:
								Success = FCString::Atoi(*CurrentData);
								break;
							case 1:
								PlayerList = CurrentData;
								break;
							case 2:
								LobbyName = CurrentData;
								break;
							case 3:
								LobbyMap = CurrentData;
								break;
							}
							CurrentIndex += 1;
						}

						
						FString CurrentAttribute = "*"; FString LeftAttributes; FString UserName; int32 PermissionLevel = 0;
						switch (Success)
						{
						case -1:
							JoinLobbyEvent(false, "Unable to read incomming LobbyData\nError Code: Tcp004","","");
							CurrentSocket = NULL;
							break;
						case 0:
							JoinLobbyEvent(false, "Lobby does not exist\nError Code: Tcp005", "", "");
							CurrentSocket = NULL;
							break;
						case 1:
							JoinLobbyEvent(false, "Wrong password\nError Code: Tcp006", "", "");
							CurrentSocket = NULL;
							break;
						case 2:
							NoteEvent("Success", "Player joined your lobby");
							CurrentIndex = 0;
							LeftAttributes = CurrentData + ":";

							while (CurrentIndex < 2 && LeftAttributes.Split(":", &CurrentAttribute, &LeftAttributes))
							{
								UE_LOG(LogTemp, Warning, TEXT("PlayerData: %s"), *CurrentAttribute);

								switch (CurrentIndex)
								{
								case 0:
									UserName = CurrentAttribute;
									break;
								case 1:
									PermissionLevel = FCString::Atoi(*CurrentAttribute);
									break;
								}
								CurrentIndex += 1;
							}
							AddUsertoUserlist(UserName, PermissionLevel);
							break;
						case 3:
							ClearUserlistEvent();
							JoinLobbyEvent(true, "Successfully joined lobby", LobbyName, LobbyMap);

							LeftString = PlayerList + ",";

							while (CurrentData != "" && LeftString.Split(",", &CurrentData, &LeftString))
							{
								UE_LOG(LogTemp, Warning, TEXT("Player: %s"), *CurrentData);

								CurrentIndex = 0;
								LeftAttributes = CurrentData + ":";

								while (CurrentIndex < 2 && LeftAttributes.Split(":", &CurrentAttribute, &LeftAttributes))
								{
									UE_LOG(LogTemp, Warning, TEXT("PlayerData: %s"), *CurrentAttribute);

									switch (CurrentIndex)
									{
									case 0:
										UserName = CurrentAttribute;
										break;
									case 1:
										PermissionLevel = FCString::Atoi(*CurrentAttribute);
										break;
									}
									

									CurrentIndex += 1;
								}


								
								AddUsertoUserlist(UserName, PermissionLevel);
							}






							break;
						case 4:
							NoteEvent("Success", "Player left your lobby");
							break;


						}
					}
				}
			}
		}
	}
}

void UMenuHud::UserRegister(const FString& UserName, const FString& Password)
{

	if (UserName.Len() >= 4 && UserName.Len() <= 16 && Password.Len() >= 4 && Password.Len() <= 64)
	{
		Http = &FHttpModule::Get();
		if (!Http) return;
		if (!Http->IsHttpEnabled()) return;

		FString ActualPassword = FMD5::HashAnsiString(*Password);

		FString JsonString;
		TSharedRef<TJsonWriter<TCHAR>> JsonWriter = TJsonWriterFactory<TCHAR>::Create(&JsonString);

		CurrentUserName = UserName;

		JsonWriter->WriteObjectStart();
		JsonWriter->WriteValue("User", UserName);
		JsonWriter->WriteValue("Pass", ActualPassword);
		JsonWriter->WriteValue("Secret", Salt);
		JsonWriter->WriteObjectEnd();
		JsonWriter->Close();

		TSharedRef < IHttpRequest > Request = Http->CreateRequest();
		Request->OnProcessRequestComplete().BindUObject(this, &UMenuHud::OnRegistrationResponseReceived);
		Request->SetURL("http://"+Ip+"/api/users/register.php");
		Request->SetVerb("POST");
		Request->SetHeader("Content-Type", "application/x-www-form-urlencoded");
		Request->SetContentAsString(JsonString);

		if (!Request->ProcessRequest())
		{
			LoginCompleteEvent(false, "Connection timed out\nError Code: Http001");
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
				LoginCompleteEvent(false, "An internal error occored\nError Code: Http002");
				return;
			case 1:
				LoginCompleteEvent(false, "Username already in use\nError Code: Http003");
				return;
			case 2:
				Token = JsonParsed->GetStringField("Token");
				LoginCompleteEvent(true, "Registration completed\nYour Token is "+Token);

				SetUserData();

				USavingFile* SaveGameInstance = Cast<USavingFile>(UGameplayStatics::CreateSaveGameObject(USavingFile::StaticClass()));
				SaveGameInstance->PlayerName = CurrentUserName;
				SaveGameInstance->Token = Token;
				UGameplayStatics::SaveGameToSlot(SaveGameInstance, SaveGameInstance->SaveSlotName, SaveGameInstance->UserIndex);

				break;
			}
			
		}
		else 
		{
			LoginCompleteEvent(false, "Failed to deserialize returns\nError Code: Http004");
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

		FString ActualPassword = FMD5::HashAnsiString(*Password);


		JsonWriter->WriteObjectStart();
		JsonWriter->WriteValue("User", UserName);
		JsonWriter->WriteValue("Pass", ActualPassword);
		//const FString& Salt = "joxR9yjVXXR4D6Rb";
		JsonWriter->WriteValue("Secret", Salt);
		JsonWriter->WriteObjectEnd();
		JsonWriter->Close();

		CurrentUserName = UserName;


		TSharedRef < IHttpRequest > Request = Http->CreateRequest();
		Request->OnProcessRequestComplete().BindUObject(this, &UMenuHud::OnLoginResponseReceived);
		Request->SetURL("http://" + Ip + "/api/users/login.php");
		Request->SetVerb("POST");
		Request->SetHeader("Content-Type", "application/x-www-form-urlencoded");
		Request->SetContentAsString(JsonString);

		if (!Request->ProcessRequest())
		{
			LoginCompleteEvent(false, "Connection timed out\nError Code: Http009");
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
		UE_LOG(LogTemp, Warning, TEXT("%s"),*Content);
		TSharedRef< TJsonReader<TCHAR> > Reader = TJsonReaderFactory<TCHAR>::Create(Content);
		TSharedPtr<FJsonObject> JsonParsed;
		if (FJsonSerializer::Deserialize(Reader, JsonParsed))
		{
			int32 Succesful = JsonParsed->GetIntegerField("bSuccesful");
			switch (Succesful)
			{
			case 0:
				LoginCompleteEvent(false, "An internal error occored\nError Code: Http006");
				return;
			case 1:
				LoginCompleteEvent(false, "Invalid Username/Password\nError Code: Http007");
				return;
			case 2:
				Token = JsonParsed->GetStringField("Token");
				LoginCompleteEvent(true, "Login completed\nYour Token is " + Token);

				SetUserData();

				USavingFile* SaveGameInstance = Cast<USavingFile>(UGameplayStatics::CreateSaveGameObject(USavingFile::StaticClass()));
				SaveGameInstance->PlayerName = CurrentUserName;
				SaveGameInstance->Token = Token;
				UGameplayStatics::SaveGameToSlot(SaveGameInstance, SaveGameInstance->SaveSlotName, SaveGameInstance->UserIndex);

				break;
			}

		}
		else
		{
			LoginCompleteEvent(false, "Failed to deserialize returns\nError Code: Http008");
		}
	}

	//Is registering animation stop
}


void UMenuHud::UserLogout(const FString& UserName, const FString& Password)
{

	USavingFile* SaveGameInstance = Cast<USavingFile>(UGameplayStatics::CreateSaveGameObject(USavingFile::StaticClass()));
	SaveGameInstance->PlayerName = "DefaultPlayer";
	SaveGameInstance->Token = "";
	UGameplayStatics::SaveGameToSlot(SaveGameInstance, SaveGameInstance->SaveSlotName, SaveGameInstance->UserIndex);

	CurrentUserName = "";

}

void UMenuHud::CreateLobby(const FString& LobbyName, const FString& Password)
{
	FString ActualPassword = Password;
	if (LobbyName.Len() >= 4 && LobbyName.Len() <= 16 && Password.Len() <= 64 && Password != "NULL")
	{
		if (Password == "")
		{
			ActualPassword = "NULL";

		}
		else {
			ActualPassword = FMD5::HashAnsiString(*Password);
		}
		Http = &FHttpModule::Get();
		if (!Http) return;
		if (!Http->IsHttpEnabled()) return;

		FString JsonString;
		TSharedRef<TJsonWriter<TCHAR>> JsonWriter = TJsonWriterFactory<TCHAR>::Create(&JsonString);

		GetUserData();


		JsonWriter->WriteObjectStart();
		JsonWriter->WriteValue("GameName", LobbyName);
		JsonWriter->WriteValue("Password", ActualPassword);
		JsonWriter->WriteValue("Secret", Salt);
		JsonWriter->WriteValue("Token", Token);
		JsonWriter->WriteObjectEnd();
		JsonWriter->Close();

		TSharedRef < IHttpRequest > Request = Http->CreateRequest();
		Request->OnProcessRequestComplete().BindUObject(this, &UMenuHud::OnCreateLobbyResponseReceived);
		Request->SetURL("http://"+Ip+"/api/serverlist/creategame.php");
		Request->SetVerb("POST");
		Request->SetHeader("Content-Type", "application/x-www-form-urlencoded");
		Request->SetContentAsString(JsonString);

		if (!Request->ProcessRequest())
		{
			LobbycreationCompleteEvent(false, "Connection timed out\nError Code: Http013");
		}
		return;
	}
	else
	{
		LobbycreationCompleteEvent(false, "Connection timed out\nError Code: Http013");
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
				LobbycreationCompleteEvent(false, "An internal error occored\nError Code: Http011");
				return;
			case 1:
				LobbycreationCompleteEvent(false, "Lobby name already in use\nError Code: Http012");
				return;
			case 2:
				LobbycreationCompleteEvent(true, "Lobby successfully created");
				RefreshLobbylist();
				break;
			case 3:
				LobbycreationCompleteEvent(false, "You are already in a Lobby. \nPlease restart your Client.\nError Code: Http014");
				return;
			}

		}
		else
		{
			LobbycreationCompleteEvent(false, "Failed to deserialize returns\nError Code: Http023");
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

	GetUserData();

	JsonWriter->WriteObjectStart();
	JsonWriter->WriteValue("Secret", Salt);
	JsonWriter->WriteValue("Token", Token);
	JsonWriter->WriteObjectEnd();
	JsonWriter->Close();


	TSharedRef < IHttpRequest > Request = Http->CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &UMenuHud::RefreshLobbylistResponseReceived);
	Request->SetURL("http://"+Ip+"/api/serverlist/GetGameList.php");
	Request->SetVerb("POST");
	Request->SetHeader("Content-Type", "application/x-www-form-urlencoded");
	Request->SetContentAsString(JsonString);

	if (!Request->ProcessRequest())
	{
		RefreshLobbylistEvent(false, "Connection timed out\nError Code: Http021");
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
		if (FJsonSerializer::Deserialize(Reader, JsonParsed))
		{
			
			int32 Succesful = JsonParsed->GetIntegerField("bSuccesful");
			switch (Succesful)
			{
			case 0:
				RefreshLobbylistEvent(false, "An internal error occored\nError Code: Http016");
				return;
			case 1:
				RefreshLobbylistEvent(true, "No available Lobbies");
				ClearLobbylistEvent();
				break;
			case 2:
				RefreshLobbylistEvent(true, "Lobby list refreshed");
				FString ServerField = JsonParsed->GetStringField("Servers");
				FString CurrentServer;
				FString LeftServers = ServerField;

				FString CurrentValue;
				FString LeftValues = CurrentServer;

				FString Name;
				FString Password;
				FString Map;

				ClearLobbylistEvent();

				while (LeftServers.Split(";", &CurrentServer, &LeftServers))
				{
					if (CurrentServer == "" || CurrentServer == ","){ break; }

					CurrentValue = "";
					LeftValues = CurrentServer;
					Name = "";
					Password = "";
					Map = "";

					int32 CurrentIndex = 0;
					while (LeftValues.Split(",", &CurrentValue, &LeftValues))
					{
						if (CurrentValue == "" ||  CurrentValue == ",")
						{ 
							break; 
						}
						switch (CurrentIndex)
						{
						case 0:
							Name = CurrentValue;
						case 1:
							Password = CurrentValue;
						case 2:
							Map = CurrentValue;
						}
						CurrentIndex += 1;
					}
					AddLobbytoLobbylist(Name, Password, Map);
				}
				
				//ClearLobbylistEvent();
				return;
			}
		}
		else
		{
			RefreshLobbylistEvent(false, "Failed to deserialize returns\nError Code: Http020");
		}
	}

}


void UMenuHud::JoinLobby(const FString& LobbyName, const FString& Password)
{

	FString ActualPassword = Password;
	if (Password.Len() <= 64)
	{
		if (Password == "" || Password == "NULL")
		{
			ActualPassword = "NULL";

		}
		else {
			ActualPassword = FMD5::HashAnsiString(*Password);
		}
		Http = &FHttpModule::Get();
		if (!Http) return;
		if (!Http->IsHttpEnabled()) return;

		FString JsonString;
		TSharedRef<TJsonWriter<TCHAR>> JsonWriter = TJsonWriterFactory<TCHAR>::Create(&JsonString);

		GetUserData();


		JsonWriter->WriteObjectStart();
		JsonWriter->WriteValue("GameName", LobbyName);
		JsonWriter->WriteValue("GamePassword", ActualPassword);
		JsonWriter->WriteValue("Secret", Salt);
		JsonWriter->WriteValue("Token", Token);
		JsonWriter->WriteObjectEnd();
		JsonWriter->Close();


		TSharedRef < IHttpRequest > Request = Http->CreateRequest();
		Request->OnProcessRequestComplete().BindUObject(this, &UMenuHud::JoinLobbyResponseReceived);
		Request->SetURL("http://" + Ip + "/api/users/JoinServer.php");
		Request->SetVerb("POST");
		Request->SetHeader("Content-Type", "application/x-www-form-urlencoded");
		Request->SetContentAsString(JsonString);
		if (!Request->ProcessRequest())
		{
			JoinLobbyEvent(false, "Connection timed out\nError Code: Http022", LobbyName, "");
		}
		return;
	}
	else
	{
		JoinLobbyEvent(false, "Connection timed out\nError Code: Http023", LobbyName, "");
	}
}


void UMenuHud::JoinLobbyResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{

		FString Content = Response->GetContentAsString();
		TSharedRef< TJsonReader<TCHAR> > Reader = TJsonReaderFactory<TCHAR>::Create(Content);
		TSharedPtr<FJsonObject> JsonParsed;
		UE_LOG(LogTemp, Warning, TEXT("return: %s"), *Content);
		if (FJsonSerializer::Deserialize(Reader, JsonParsed))
		{

			int32 Succesful = JsonParsed->GetIntegerField("bSuccesful");
			switch (Succesful)
			{
			case 0:
				JoinLobbyEvent(false, "An internal error occored\nError Code: Http025", "", "");
				break;
			case 1:
				JoinLobbyEvent(false, "Wrong password\nError Code: Http026", "", "");
				break;
			case 2:
				FString Ip = JsonParsed->GetStringField("Ip");
				FString Port = JsonParsed->GetStringField("Port");
				FString Name = JsonParsed->GetStringField("Name");
				
				FString Password = ";"+JsonParsed->GetStringField("Password");

				if (Password == ";NULL")
				{
					Password = "";
				}

				int32 PortIndex = FCString::Atoi(*Port);

				FSocket* LobbySocket = CreateSocket(Ip, PortIndex);

				GetUserData();

				FString Serialized = TEXT("Join;" + Name + ";" + CurrentUserName + ";" + Token + Password + "|");

				UE_LOG(LogTemp, Warning, TEXT("Sending Data: %s"), *Serialized);

				TCHAR *SerializedChar = Serialized.GetCharArray().GetData();
				int32 Size = FCString::Strlen(SerializedChar);
				int32 Sent = 0;

				bool SendSuccessful = LobbySocket->Send((uint8*)TCHAR_TO_UTF8(SerializedChar), Size, Sent);

				if (!SendSuccessful)
				{
					JoinLobbyEvent(false, "Could not reach Server\nError Code: Tcp002", "", "");
					return;
				}

				CurrentSocket = LobbySocket;
				IsConnected = true;

				return;
			}
		}
		else
		{
			JoinLobbyEvent(false, "Failed to deserialize returns\nError Code: Tcp003", "", "");
		}
	}


}

FSocket* UMenuHud::CreateSocket(const FString& Ip, int32 PortIndex)
{

	FSocket* LobbySocket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(NAME_Stream, TEXT("default"), false);
	FIPv4Address Address;
	FIPv4Address::Parse(Ip, Address);
	TSharedRef<FInternetAddr> addr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	addr->SetIp(Address.GetValue());
	addr->SetPort(PortIndex);
	bool Connected = LobbySocket->Connect(*addr);

	return LobbySocket;

}