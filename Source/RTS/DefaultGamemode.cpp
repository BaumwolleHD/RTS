// Fill out your copyright notice in the Description page of Project Settings.

#include "RTS.h"
#include "DefaultGamemode.h"
#include "Blueprint/UserWidget.h"


ADefaultGamemode::ADefaultGamemode()
{

}

void ADefaultGamemode::BeginPlay()
{
	Super::BeginPlay();
	if (HudWidgetClass != nullptr)
	{
		CurrentWidget = CreateWidget<UUserWidget>(GetWorld(), HudWidgetClass);
		if (CurrentWidget)
		{
			CurrentWidget->AddToViewport();
		}
	}


}

void ADefaultGamemode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
