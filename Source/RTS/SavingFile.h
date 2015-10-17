// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/SaveGame.h"
#include "SavingFile.generated.h"

/**
 * 
 */
UCLASS()
class RTS_API USavingFile : public USaveGame
{
	GENERATED_BODY()
public:
	USavingFile();

	UPROPERTY(VisibleAnywhere, Category = Basic)
		FString PlayerName;

	UPROPERTY(VisibleAnywhere, Category = Basic)
		FString SaveSlotName;

	UPROPERTY(VisibleAnywhere, Category = Basic)
		uint32 UserIndex;

	UPROPERTY(EditAnywhere, Category = Networking)
		FString UserName;
	UPROPERTY(EditAnywhere, Category = Networking)
		FString Password;
	UPROPERTY(EditAnywhere, Category = Networking)
		FString Token;
};
