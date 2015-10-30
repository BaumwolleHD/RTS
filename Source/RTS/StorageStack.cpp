// Fill out your copyright notice in the Description page of Project Settings.

#include "RTS.h"
#include "StorageStack.h"


// Sets defaultvalues
AStorageStack::AStorageStack()
{
	RootComponent = Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	Mesh0 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh0"));
	Mesh1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh1"));
	Mesh2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh2"));
	Mesh3 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh3"));
	Mesh4 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh4"));
	Mesh5 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh5"));
	Mesh6 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh6"));
	Mesh7 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh7"));
	Mesh8 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh8"));
	Mesh9 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh9"));


	Mesh0->AttachTo(Root);
	Mesh1->AttachTo(Root);
	Mesh2->AttachTo(Root);
	Mesh3->AttachTo(Root);
	Mesh4->AttachTo(Root);
	Mesh5->AttachTo(Root);
	Mesh6->AttachTo(Root);
	Mesh7->AttachTo(Root);
	Mesh8->AttachTo(Root);
	Mesh9->AttachTo(Root);

	




	bReplicates = true;

}

void AStorageStack::ConstructMeshes(UStaticMesh* Mesh)
{
	Meshes.SetNum(0);
	Meshes.Add(Mesh0);
	Meshes.Add(Mesh1);
	Meshes.Add(Mesh2);
	Meshes.Add(Mesh3);
	Meshes.Add(Mesh4);
	Meshes.Add(Mesh5);
	Meshes.Add(Mesh6);
	Meshes.Add(Mesh7);
	Meshes.Add(Mesh8);
	Meshes.Add(Mesh9);

	for (int32 Index = 0; Index < Meshes.Num(); Index++)
	{
		Meshes[Index]->SetStaticMesh(Mesh);
	}

}

void AStorageStack::ConstructLocationByConstantOffset(FVector Offset)
{
	for (int32 Index = 0; Index < Meshes.Num(); Index++)
	{
		Meshes[Index]->AddRelativeLocation(FVector(Offset));
	}
}

void AStorageStack::ConstructLocationByOffsetPerLayer(FVector Offset)
{
	for (int32 Index = 0; Index < Meshes.Num(); Index++)
	{
		Meshes[Index]->SetRelativeLocation(FVector(Offset.X * Index, Offset.Y * Index, Offset.Z * Index));
	}
}

void AStorageStack::ConstructLocationByPyramide(FVector ItemSize, FVector ItemSizeDeviation, int32 FloorItems, int32 ItemSubstractionPerLayer, FRotator RotationOffsetPerLayer, FRotator RotationOffsetDeviation)
{
	int32 CurrentLayerItems = FloorItems;
	int32 CurrentLayer = 0;
	FVector SizeDeviation;
	FRotator RotationDevaition;
	int32 CurrentItems = 0;


	for (int32 Index = 0; Index < Meshes.Num(); Index++)
	{

		SizeDeviation.X = FMath::FRandRange(-ItemSizeDeviation.X / 2, ItemSizeDeviation.X / 2);
		SizeDeviation.Y = FMath::FRandRange(-ItemSizeDeviation.Y / 2, ItemSizeDeviation.Y / 2);
		SizeDeviation.Z = FMath::FRandRange(-ItemSizeDeviation.Z / 2, ItemSizeDeviation.Z / 2);

		Meshes[Index]->SetRelativeLocation(SizeDeviation + FVector((CurrentLayerItems - FloorItems * 0.5f - 0.5f) * ItemSize.X, (CurrentLayerItems - FloorItems * 0.5f - 0.5f) * ItemSize.Y, ItemSize.Z * CurrentLayer));
		
		RotationDevaition.Roll = FMath::FRandRange(-RotationOffsetDeviation.Roll / 2, RotationOffsetDeviation.Roll / 2);
		RotationDevaition.Pitch = FMath::FRandRange(-RotationOffsetDeviation.Pitch / 2, RotationOffsetDeviation.Pitch / 2);
		RotationDevaition.Yaw = FMath::FRandRange(-RotationOffsetDeviation.Yaw / 2, RotationOffsetDeviation.Yaw / 2);

		Meshes[Index]->AddRelativeRotation(RotationOffsetPerLayer*CurrentLayer + RotationDevaition);


		CurrentLayerItems--;
		CurrentItems++;
		if (CurrentItems == MaxQuantity)
		{
			break;
		}
		else if (CurrentLayerItems == 0)
		{
			CurrentLayer++;
			FloorItems -= ItemSubstractionPerLayer;
			CurrentLayerItems = FloorItems;
			
		}
	}

}


void AStorageStack::ConstructRotationByRotator(FRotator Offset)
{
	for (int32 Index = 0; Index < Meshes.Num(); Index++)
	{
		Meshes[Index]->SetRelativeRotation(Offset);
	}
}

void AStorageStack::ConstructScaleByVector(FVector Scale)
{
	for (int32 Index = 0; Index < Meshes.Num(); Index++)
	{
		Meshes[Index]->SetRelativeScale3D(Scale);
	}
}

void AStorageStack::ConstructScaleByFloat(float Scale)
{
	for (int32 Index = 0; Index < Meshes.Num(); Index++)
	{
		Meshes[Index]->SetRelativeScale3D(FVector(Scale,Scale,Scale));
	}
}

void AStorageStack::BeginPlay()
{
	Super::BeginPlay();
}

void AStorageStack::Sort()
{
	if (Role == ROLE_Authority)
	{
		SendStackUpdateToClients(Quantity);
	}
}

void AStorageStack::SendStackUpdateToClients_Implementation(int32 ServerQuantity)
{
	if (Role != ROLE_Authority)
	{
		Quantity = ServerQuantity;

	}
	int32 CurrentQuantity = 0;
	int32 LayerItems = 0;
//	for (int32 Index = 0; Index <= FMath::CeilToInt(Quantity) + ItemsPerLayer; Index += ItemsPerLayer)
	for (int32 LayerIndex = 0; LayerIndex < 10; LayerIndex++)
	{

		LayerItems = FMath::Min(ItemsPerLayer, Quantity - CurrentQuantity);
		CurrentQuantity += LayerItems;
		if (LayerIndex >= 0 && LayerIndex < Meshes.Num()){ Meshes[LayerIndex]->SetStaticMesh(GetMeshByLayerItems(LayerItems, LayerIndex)); }
	}
}

UStaticMesh* AStorageStack::GetMeshByLayerItems(int32 LayerItems, int32 Layer)
{
	if (LayerItems > 0)
	{
		return VisualMeshes[LayerItems - 1];
	}
	
	//UE_LOG(LogTemp, Warning, TEXT("Layer: %d"), LayerItems, Layer);
	return NULL;
}