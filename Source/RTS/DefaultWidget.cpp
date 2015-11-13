// Fill out your copyright notice in the Description page of Project Settings.

#include "RTS.h"
#include "DefaultWidget.h"


void UDefaultWidget::UpdateItems(TArray<int32> Items)
{
	Items.Sort();
	OnUpdateItems(Items, Items);

}

AActor* UDefaultWidget::BPGetDefaultObject(TSubclassOf<AActor> Class)
{

	return Class.GetDefaultObject();

}
