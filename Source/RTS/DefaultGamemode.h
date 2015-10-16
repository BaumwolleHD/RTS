// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameMode.h"
#include "DefaultGamemode.generated.h"

/**
 * 
 */
UCLASS()
class RTS_API ADefaultGamemode : public AGameMode
{
	GENERATED_BODY()
public:
	ADefaultGamemode();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Hud)
		TSubclassOf<class UUserWidget> HudWidgetClass;

	UPROPERTY()
	class UUserWidget* CurrentWidget;
	
};
