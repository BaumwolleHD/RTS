// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/HUD.h"
#include "PlayerHUD.generated.h"

UCLASS()
class RTSMEDIVALTEST_API APlayerHUD : public AHUD
{
	GENERATED_BODY()
public:
	APlayerHUD();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MainMenu)
		UTexture2D* DefaultTexture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MainMenu)
		UFont* DefaultFont;
	
	virtual void DrawHUD() override;

	void DrawButton(UTexture2D* Texture, FVector2D Location, FName HitboxName, FString Text);
	
	
	
	
};
