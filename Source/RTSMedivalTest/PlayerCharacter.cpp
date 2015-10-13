// Fill out your copyright notice in the Description page of Project Settings.

#include "RTSMedivalTest.h"
#include "PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Building.h"
#include "BuildingPreview.h"
#include "StorageBlock.h"
#include "StorageStack.h"
#include "Engine.h"


// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Mode = "Building";
	CameraMovementMargin = 0.1f;
	CameraMaxSpeed = 100.f;
	CameraMinSpeed = 50.f;
	//TimerCount = 0.f;
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	UGameplayStatics::GetPlayerController(this,0)-> bShowMouseCursor = true;
	
}

// Called every frame
void APlayerCharacter::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
	UGameplayStatics::GetPlayerController(this, 0)->GetMousePosition(MouseLocation.X, MouseLocation.Y);
	
	if (!Paused)
	{
		UGameplayStatics::GetPlayerController(this, 0)->GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_Visibility), true, MouseHitResult);
		MouseMovement(DeltaTime);
		if (Mode == "Building")
		{
			BuildingPreview();
		}
	}
	else
	{

	}
}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);
	InputComponent->BindAction("LeftClick", IE_Released, this, &APlayerCharacter::PlaceBuildingBind);
	InputComponent->BindAction("Pause", IE_Pressed, this, &APlayerCharacter::Pause);

}
void APlayerCharacter::PlaceBuildingBind()
{

	PlaceBuilding(MouseHitResult, this);
}
void APlayerCharacter::PlaceBuilding_Implementation(FHitResult HitResult, APlayerCharacter* PlayerCharacter)
{
	if (HitResult.bBlockingHit == true)
	{
		UWorld* World = GetWorld();
	
		if (World)
		{

			ABuilding* const TestBuilding = SelectedBuilding.GetDefaultObject();
			FVector2D Grid2DPosition = ApplyGrid(FVector2D(HitResult.ImpactPoint.X, HitResult.ImpactPoint.Y), TestBuilding->Size);
			if (CanPlaceBuilding(Grid2DPosition, TestBuilding->Size))
			{
				FActorSpawnParameters SpawnParameters;
				SpawnParameters.Owner = PlayerCharacter;
				SpawnParameters.Instigator = Instigator;
				UE_LOG(LogTemp, Warning, TEXT("SpawnOwner: %s"), *PlayerCharacter->GetName());
				ABuilding* const SpawnedBuilding = World->SpawnActor<ABuilding>(SelectedBuilding, FVector(Grid2DPosition.X * 1000.f, Grid2DPosition.Y * 1000.f, 20.f), FRotator(0.f, 0.f, 0.f), SpawnParameters);


				UpdateBlockToServer(Grid2DPosition, SpawnedBuilding->Size, FString::FromInt(SpawnedBuilding->ID));
			}
		}
	}
}

bool APlayerCharacter::CanPlaceBuilding(FVector2D Position, FVector2D Size)
{
	//Position = ApplyGrid(Position, SelectedBuilding.GetDefaultObject()->Size);
	for (int32 X = -Size.X/2 + Position.X; X <= Size.X/2 + Position.X; X++)
	{
		for (int32 Y = -Size.Y/2 + Position.Y; Y <= Size.Y/2 + Position.Y; Y++)
		{
			if (PP_BlockID.Contains(SerializeFVector2D(FVector2D(X,Y))))
			{
				return false;
			}


		}
	}
	return true;
}

FVector2D APlayerCharacter::ApplyGrid(FVector2D Location, FVector2D Size)
{
	Location.X = float(FMath::RoundToInt(Location.X / 1000) + 0.5*int(!(int(Size.X)%2)));
	Location.Y = float(FMath::RoundToInt(Location.Y / 1000) + 0.5*int(!(int(Size.Y)%2)));
	return Location;
}

bool APlayerCharacter::PlaceBuilding_Validate(FHitResult HitResult, APlayerCharacter* PlayerCharacter) { return true; } //Anti-Cheat

//bool APlayerCharacter::UpdateBlockToClients_Validate(const FString& Position, int32 ID) { return true; } //Anti-Cheat

void APlayerCharacter::UpdateBlockToServer_Implementation(FVector2D Position, FVector2D Size, const FString& ID)
{
	if (Role == ROLE_Authority)
	{
		APlayerCharacter::UpdateBlockToClients(Position, Size, ID);
	}
}

bool APlayerCharacter::UpdateBlockToServer_Validate(FVector2D Position, FVector2D Size, const FString& ID) { return true; } //Anti-Cheat

void APlayerCharacter::UpdateBlockToClients_Implementation(FVector2D Position, FVector2D Size, FString ID)
{
	FString SerializedPosition = "";
	FString CenterSerializedPosition = SerializeFVector2D(Position);

	if (PP_BlockID.Contains(CenterSerializedPosition))
	{
		PP_BlockID[CenterSerializedPosition] = ID;
	}
	else
	{
		PP_BlockID.Add(CenterSerializedPosition, ID);
	}
	for (int32 X = -Size.X/2+1 + Position.X; X <= Size.X/2 + Position.X-1; X++)
	{
		for (int32 Y = -Size.Y/2+1 + Position.Y; Y <= Size.Y/2 + Position.Y-1; Y++)
		{
			
			if (FVector2D(X, Y) != Position)
			{
				SerializedPosition = SerializeFVector2D(FVector2D(X, Y));
				if (PP_BlockID.Contains(SerializedPosition))
				{
					PP_BlockID[SerializedPosition] = CenterSerializedPosition;
				}
				else
				{
					PP_BlockID.Add(SerializedPosition, CenterSerializedPosition);
				}
			}



		}


	}
}

FString APlayerCharacter::SerializeFVector2D(FVector2D Vec2D)
{
	return FString::FromInt(int(Vec2D.X)) + "|" + FString::FromInt(int(Vec2D.Y));
}

void APlayerCharacter::Pause()
{
	/*Paused = !Paused;
	if (Paused)
	{}
	else
	{}*/
	//GetItemStack(0);
}
void APlayerCharacter::MouseMovement(float DeltaTime)
{
	int32 ViewportSizeX;
	int32 ViewportSizeY;
	APlayerController* PlayerControl = UGameplayStatics::GetPlayerController(this, 0);

	PlayerControl->GetViewportSize(ViewportSizeX, ViewportSizeY);



	FVector2D Factor = FVector2D(MouseLocation.X / ViewportSizeX, MouseLocation.Y / ViewportSizeY);
	FVector CurrentLocation = this -> GetActorLocation();


	if (Factor.X > 1 - CameraMovementMargin&& CurrentLocation.Y <= MapSize.Y * 1000 /2)
	{
		SetActorLocation(CurrentLocation + (FVector(0.f, CameraMaxSpeed, 0.f)*FMath::Pow(Factor.X, CameraMaxSpeed - CameraMinSpeed)));

	}
	else if (Factor.X < CameraMovementMargin &&CurrentLocation.Y >= -MapSize.Y * 1000 / 2)
	{
		SetActorLocation(CurrentLocation + (FVector(0.f, -CameraMaxSpeed, 0.f)*FMath::Pow((1 - Factor.X), CameraMaxSpeed - CameraMinSpeed)));


	}

	if (Factor.Y>1 - CameraMovementMargin && CurrentLocation.X >= -MapSize.X * 1000 / 2)
	{
		SetActorLocation(CurrentLocation + (FVector(-CameraMaxSpeed, 0.f, 0.f)*FMath::Pow(Factor.Y, CameraMaxSpeed - CameraMinSpeed)));
	}
	else if (Factor.Y < CameraMovementMargin && CurrentLocation.X <= MapSize.X * 1000 / 2)
	{
		SetActorLocation(CurrentLocation + (FVector(CameraMaxSpeed, 0.f, 0.f)*FMath::Pow((1 - Factor.Y), CameraMaxSpeed - CameraMinSpeed)));
	}

}

float APlayerCharacter::ScaleToViewportFloat(float in, float factor)
{
	return factor / 1920 * in;
}

void APlayerCharacter::BuildingPreview()
{
	if (MouseHitResult.bBlockingHit)
	{
		ABuilding* const TestBuilding = SelectedBuilding.GetDefaultObject();
		FVector2D GridLocation = ApplyGrid(FVector2D(MouseHitResult.ImpactPoint.X, MouseHitResult.ImpactPoint.Y), TestBuilding->Size);
		const FString PassedString = FString::FromInt(GridLocation.X) + "|" + FString::FromInt(GridLocation.Y);
		ABuildingPreview* const CurrentPreviewBuilding = Cast<ABuildingPreview>(CurrentPreview);
		if (CurrentPreviewBuilding)
		{
			CurrentPreviewBuilding->SetActorLocation(FVector(GridLocation.X * 1000, GridLocation.Y * 1000, MouseHitResult.ImpactPoint.Z));
			
			if (CurrentPreviewBuilding->Mesh->StaticMesh != TestBuilding->BuildMeshes[4])
			{	
				CurrentPreviewBuilding->Mesh->SetStaticMesh(TestBuilding->BuildMeshes[4]);
				CurrentPreviewBuilding->Mesh->SetMaterial(0,TestBuilding->BuildMeshes[4]->GetMaterial(0));
				CurrentPreviewBuilding->Material = CurrentPreviewBuilding->Mesh->CreateAndSetMaterialInstanceDynamic(0);
				CurrentPreview->SetActorScale3D(FVector(TestBuilding->Size.X, TestBuilding->Size.X, TestBuilding->Size.X));
			}
			
			if (CanPlaceBuilding(GridLocation, TestBuilding->Size))
			{
				CurrentPreviewBuilding->Material->SetVectorParameterValue(FName("PreviewColor"), FLinearColor(0, 0.8f, 0));
			}
			else
			{
				CurrentPreviewBuilding->Material->SetVectorParameterValue(FName("PreviewColor"), FLinearColor(1, 0, 0));
			}
		}
		else if (CanPlaceBuilding(GridLocation, TestBuilding->Size))
		{
			UWorld* World = UGameplayStatics::GetPlayerController(this, 0)->GetWorld();
			if (World)
			{
				FActorSpawnParameters SpawnParameters;
				SpawnParameters.Owner = this;
				SpawnParameters.Instigator = Instigator;
				CurrentPreview = World->SpawnActor<ABuildingPreview>(BuildingPreviewObject, FVector(GridLocation.X * 1000, GridLocation.Y * 1000, MouseHitResult.ImpactPoint.Z), FRotator(0.f, 0.f, 0.f), SpawnParameters);
				ABuildingPreview* const CurrentPreviewBuilding = Cast<ABuildingPreview>(CurrentPreview);
				CurrentPreviewBuilding->Mesh->SetStaticMesh(TestBuilding->BuildMeshes[4]);
				CurrentPreviewBuilding->Material = CurrentPreviewBuilding->Mesh->CreateAndSetMaterialInstanceDynamic(0);
				CurrentPreview->SetActorScale3D(FVector(TestBuilding->Size.X, TestBuilding->Size.X, TestBuilding->Size.X));
			}
			else
			{
				CurrentPreview->Destroy();
			}
		}
	}
}


void APlayerCharacter::ChangeItem(int32 Quantity, int32 ID)
{
	for (int32 BlockIndex = 0; BlockIndex < OwnedStorageBlocks.Num(); BlockIndex++)
	{
		AStorageBlock* const Block = OwnedStorageBlocks[BlockIndex];
		if (Block && Block->BuildProgressionState == 5)
		{
			for (int32 StackIndex = 0; StackIndex < Block->StorageStacks.Num(); StackIndex++)
			{
				AStorageStack* const Stack = Cast<AStorageStack>(Block->StorageStacks[StackIndex]);
				if (Stack)
				{
					if (Stack->GetClass() == ItemClasses[ID])
					{	
						if (Quantity + Stack->Quantity <= Stack->MaxQuantity)
						{
							Stack->Quantity += Quantity;
							Stack->Sort();
							Quantity = 0;
							return;
						}
						else
						{
							Quantity -= Stack->MaxQuantity - Stack->Quantity;
							Stack->Quantity = Stack->MaxQuantity;
							Stack->Sort();
						}
					}
				}
			}
		}
	}

	for (int32 BlockIndex = 0; BlockIndex < OwnedStorageBlocks.Num(); BlockIndex++)
	{
		AStorageBlock* const Block = OwnedStorageBlocks[BlockIndex];
		if (Block && Block->BuildProgressionState == 5)
		{
			for (int32 StackIndex = 0; StackIndex < Block->StorageStacks.Num(); StackIndex++)
			{
				AStorageStack* const Stack = Cast<AStorageStack>(Block->StorageStacks[StackIndex]);
				if (!Stack)
				{
					UWorld* World = GetWorld();
					if (World)
					{

						FActorSpawnParameters SpawnParameters;
						SpawnParameters.Owner = Block;
						SpawnParameters.Instigator = Instigator;
						const FVector Location = Block->GetItemPosition(StackIndex);
						AStorageStack* const Stack = World->SpawnActor<AStorageStack>(ItemClasses[ID], Location, FRotator(0.f, 0.f, 0.f), SpawnParameters);
						Block->StorageStacks[StackIndex] = Stack;
						Stack->SetActorRotation(FRotator(0, int(FMath::RandBool())*90.f, 0));



						if (Block->StorageStacks[StackIndex]->GetClass() == ItemClasses[ID])
						{
							if (Quantity <= Stack->MaxQuantity)
							{
								Stack->Quantity = Quantity;
								Stack->Sort();
								Quantity = 0;
								return;
							}
							else
							{
								Quantity -= Stack->MaxQuantity - Stack->Quantity;
								Stack->Quantity = Stack->MaxQuantity;
								Stack->Sort();
							}
						}
					}
				}
			}
		}
	}
}