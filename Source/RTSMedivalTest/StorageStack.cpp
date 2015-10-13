// Fill out your copyright notice in the Description page of Project Settings.

#include "RTSMedivalTest.h"
#include "StorageStack.h"


// Sets default values
AStorageStack::AStorageStack()
{
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

	RootComponent = Mesh0;
	Mesh1->AttachTo(Mesh0);
	Mesh2->AttachTo(Mesh0);
	Mesh3->AttachTo(Mesh0);
	Mesh4->AttachTo(Mesh0);
	Mesh5->AttachTo(Mesh0);
	Mesh6->AttachTo(Mesh0);
	Mesh7->AttachTo(Mesh0);
	Mesh8->AttachTo(Mesh0);
	Mesh9->AttachTo(Mesh0);


	bReplicates = true;

}

// Called when the game starts or when spawned
void AStorageStack::BeginPlay()
{
	Super::BeginPlay();
	Mesh1->SetRelativeLocation(FVector(0.f, 0.f, ItemHeight * 1));
	Mesh2->SetRelativeLocation(FVector(0.f, 0.f, ItemHeight * 2));
	Mesh3->SetRelativeLocation(FVector(0.f, 0.f, ItemHeight * 3));
	Mesh4->SetRelativeLocation(FVector(0.f, 0.f, ItemHeight * 4));
	Mesh5->SetRelativeLocation(FVector(0.f, 0.f, ItemHeight * 5));
	Mesh6->SetRelativeLocation(FVector(0.f, 0.f, ItemHeight * 6));
	Mesh7->SetRelativeLocation(FVector(0.f, 0.f, ItemHeight * 7));
	Mesh8->SetRelativeLocation(FVector(0.f, 0.f, ItemHeight * 8));
	Mesh9->SetRelativeLocation(FVector(0.f, 0.f, ItemHeight * 9));

	//insert to character list
	//APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(this->GetOwner()->GetOwner());
	//if (PlayerCharacter)
	//{
	//	PlayerCharacter->OwnedStorageStacks.Add(this);

	//}

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
	for (int32 Index = 0; Index <= FMath::CeilToInt(Quantity) + ItemsPerLayer; Index += ItemsPerLayer)
	{
		if (CurrentQuantity < Quantity)
		{

			LayerItems = FMath::Min(ItemsPerLayer, Quantity - CurrentQuantity);
			CurrentQuantity += LayerItems;


			switch (Index / ItemsPerLayer-1)
			{
			case 0:
				Mesh0->SetStaticMesh(GetMeshByLayerItems(LayerItems, 0));
				break;
			case 1:
				Mesh1->SetStaticMesh(GetMeshByLayerItems(LayerItems, 1));
				break;
			case 2:
				Mesh2->SetStaticMesh(GetMeshByLayerItems(LayerItems, 2));
				break;
			case 3:
				Mesh3->SetStaticMesh(GetMeshByLayerItems(LayerItems, 3));
				break;
			case 4:
				Mesh4->SetStaticMesh(GetMeshByLayerItems(LayerItems, 4));
				break;
			case 5:
				Mesh5->SetStaticMesh(GetMeshByLayerItems(LayerItems, 5));
				break;
			case 6:
				Mesh6->SetStaticMesh(GetMeshByLayerItems(LayerItems, 6));
				break;
			case 7:
				Mesh7->SetStaticMesh(GetMeshByLayerItems(LayerItems, 7));
				break;
			case 8:
				Mesh8->SetStaticMesh(GetMeshByLayerItems(LayerItems, 8));
				break;
			case 9:
				Mesh9->SetStaticMesh(GetMeshByLayerItems(LayerItems, 9));
				break;

			}
		}
	}
}

UStaticMesh* AStorageStack::GetMeshByLayerItems(int32 LayerItems, int32 Layer)
{
	if (Layer % 2) {
		return VisualMeshEven[LayerItems - 1];

	}
	else {

		return VisualMeshOdd[LayerItems - 1];
	}
}