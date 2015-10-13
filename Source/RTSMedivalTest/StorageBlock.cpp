// Fill out your copyright notice in the Description page of Project Settings.

#include "RTSMedivalTest.h"
#include "StorageStack.h"
#include "Building.h"
#include "StorageBlock.h"

#include "Kismet/GameplayStatics.h"
#include "PlayerCharacter.h"

AStorageBlock::AStorageBlock()
{
	//Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	//RootComponent = Mesh;
	
	bReplicates = true;
}

void AStorageBlock::BeginPlay()
{
	Super::BeginPlay();
	StorageStacks.SetNum(SizeX*SizeY);

	//insert to character list
	//UE_LOG(LogTemp, Warning, TEXT("Owner: %s"), *this->GetOwner()->GetName());
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(this->GetOwner());
	if (PlayerCharacter && Role == ROLE_Authority)
	{
		
		PlayerCharacter->OwnedStorageBlocks.Add(this);

	}

}

//bool AStorageBlock::AddStorageStack(int32 Type, int32 Quantity)
//{
//	if (Role == ROLE_Authority)
//	{
//		for (int32 Index = 0; Index < StorageStacks.Num(); Index++)
//		{
//			AStorageStack* const StorageStack = Cast<AStorageStack>(StorageStacks[Index]);
//			if (StorageStack && StorageStack->Type == Type)
//			{
//				if (Quantity + StorageStack->Quantity <= StorageStack->MaxQuantity)
//				{
//					StorageStack->Quantity += Quantity;
//					StorageStack->Sort();
//					return true;
//				}
//				else
//				{
//					Quantity -= StorageStack->MaxQuantity - StorageStack->Quantity;
//					StorageStack->Quantity = StorageStack->MaxQuantity;
//					StorageStack->Sort();
//				}
//
//			}
//
//		}
//		for (int32 Index = 0; Index < StorageStacks.Num(); Index++)
//		{
//			AStorageStack* const StorageStack = Cast<AStorageStack>(StorageStacks[Index]);
//			if (!StorageStack)
//			{
//				UWorld* World = GetWorld();
//				if (World)
//				{
//					FActorSpawnParameters SpawnParameters;
//					SpawnParameters.Owner = this;
//					SpawnParameters.Instigator = Instigator;
//					const FVector Location = GetItemPosition(Index);
//					APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetOwner());
//					AStorageStack* const SpawnedItem = World->SpawnActor<AStorageStack>(PlayerCharacter->ItemClasses[Type], Location, FRotator(0.f, 0.f, 0.f), SpawnParameters);
//					StorageStacks[Index] = SpawnedItem;
//					SpawnedItem->SetActorRotation(FRotator(0, int(FMath::RandBool())*90.f, 0));
//					if (Quantity + SpawnedItem->Quantity <= SpawnedItem->MaxQuantity)
//					{
//						SpawnedItem->Quantity = Quantity;
//						SpawnedItem->Sort();
//						return true;
//					}
//					else
//					{
//						Quantity -= SpawnedItem->MaxQuantity - SpawnedItem->Quantity;
//						SpawnedItem->Quantity = SpawnedItem->MaxQuantity;
//						SpawnedItem->Sort();
//					}
//
//
//				}
//			}
//		}
//	}
//
//	return false;
//}

FVector AStorageBlock::GetItemPosition(int32 Index)
{
	FVector Location = GetActorLocation();
	Location.X += (Index % SizeX + 1) * 1000 - 2000;
	Location.Y += (Index / SizeY + 1) * 1000 - 2000;
	return Location;
}