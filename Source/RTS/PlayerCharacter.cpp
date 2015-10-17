// Fill out your copyright notice in the Description page of Project Settings.

#include "RTS.h"
#include "PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Building.h"
#include "BuildingPreview.h"
#include "StorageBlock.h"
#include "StorageStack.h"
#include "NpcCharacter.h"
#include "NpcController.h"
#include "BuildingExtension.h"
#include "Engine.h"


// Sets default values
APlayerCharacter::APlayerCharacter()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Mode = "Build";
	CameraMovementMargin = 0.1f;
	CameraMaxSpeed = 100.f;
	CameraMinSpeed = 50.f;
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	UGameplayStatics::GetPlayerController(this, 0)->bShowMouseCursor = true;

}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UGameplayStatics::GetPlayerController(this, 0)->GetMousePosition(MouseLocation.X, MouseLocation.Y);

	if (!Paused)
	{
		UGameplayStatics::GetPlayerController(this, 0)->GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_Visibility), true, MouseHitResult);
		MouseMovement(DeltaTime);
		if (Mode == "Build" || Mode == "BuildExtension")
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
	InputComponent->BindAction("LeftClick", IE_Released, this, &APlayerCharacter::OnLeftClick);
	InputComponent->BindAction("Pause", IE_Pressed, this, &APlayerCharacter::Pause);

}
void APlayerCharacter::OnLeftClick()
{
	if (Mode == "Build" || Mode == "BuildExtension")
	{
		PlaceBuilding(MouseHitResult, this);
	}
	else
	{
		SelectActor();
	}
}

void APlayerCharacter::SelectActor()
{
	if (MouseHitResult.bBlockingHit == true)
	{
		AActor* HitActor = MouseHitResult.GetActor();
		if (HitActor->GetRootComponent()->ComponentHasTag(FName("SelectAble")))
		{
			SelectedActor = HitActor;
			UE_LOG(LogTemp, Warning, TEXT("User %s Selected Actor: %s"), *GetName(), *SelectedActor->GetName());
		}
	}
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
				ABuilding* const SpawnedBuilding = World->SpawnActor<ABuilding>(SelectedBuilding, FVector(Grid2DPosition.X * 100, Grid2DPosition.Y * 100, 20.f), FRotator(0.f, 0.f, 0.f), SpawnParameters);
				UpdateBlockToServer(Grid2DPosition, SpawnedBuilding->Size, FString::FromInt(SpawnedBuilding->ID));


				if (PlayerCharacter->Mode == "BuildExtension")
				{
					ABuilding* const Building = Cast<ABuilding>(PlayerCharacter->SelectedActor);
					ABuildingExtension* const BuildingExtension = Cast<ABuildingExtension>(SpawnedBuilding);
					if (Building && BuildingExtension)
					{
						Building->ExtensionBuildings.Add(SpawnedBuilding);
						BuildingExtension->SetMasterBuilding(Building);
					}
				}
			}
		}
	}
}

bool APlayerCharacter::CanPlaceBuilding(FVector2D Position, FVector2D Size)
{
	FVector2D Offset = FVector2D(FMath::Abs(0.5f * int(!(int(Size.X) % 2))), FMath::Abs(0.5f * int(!(int(Size.Y) % 2))));
	for (int32 X = -Size.X / 2 + Position.X - Offset.X - (0.5f - Offset.X); X <= Size.X / 2 + Position.X - Offset.X - 3 * (0.5f - Offset.X) - 2 * Offset.X; X++)
	{
		for (int32 Y = -Size.Y / 2 + Position.Y - Offset.Y - (0.5f - Offset.Y); Y <= Size.Y / 2 + Position.Y - Offset.Y - 3 * (0.5f - Offset.Y) - 2 * Offset.Y; Y++)
		{
			if (PP_BlockID.Contains(SerializeFVector2D(FVector2D(X, Y))))
			{
				return false;
			}


		}
	}
	return true;
}

FVector2D APlayerCharacter::ApplyGrid(FVector2D Location, FVector2D Size)
{
	Location.X = float(FMath::RoundToInt(Location.X / 100) + 0.5*int(!(int(Size.X) % 2)));
	Location.Y = float(FMath::RoundToInt(Location.Y / 100) + 0.5*int(!(int(Size.Y) % 2)));
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

	FVector2D Offset = FVector2D(FMath::Abs(0.5f * int(!(int(Size.X) % 2))), FMath::Abs(0.5f * int(!(int(Size.Y) % 2))));
	for (int32 X = -Size.X / 2 + Position.X - Offset.X - (0.5f - Offset.X); X <= Size.X / 2 + Position.X - Offset.X - 3*(0.5f - Offset.X) - 2 * Offset.X; X++)
	{
		for (int32 Y = -Size.Y / 2 + Position.Y - Offset.Y - (0.5f - Offset.Y); Y <= Size.Y / 2 + Position.Y - Offset.Y - 3*(0.5f - Offset.Y) - 2 * Offset.Y; Y++)
		{

			if (FVector2D(X, Y) != Position)
			{
				SerializedPosition = SerializeFVector2D(FVector2D(X, Y));
				//UE_LOG(LogTemp, Warning, TEXT("%s, Offset: %f|%f, Min: %f|%f, Max: %f|%f"), *SerializedPosition, Offset.X, Offset.Y, -Size.X / 2 + Position.X - Offset.X + (0.5f - Offset.X), -Size.Y / 2 + Position.Y - Offset.Y, Size.X / 2 + Position.X - Offset.X - (0.5f - Offset.X), Size.Y / 2 + Position.Y - Offset.Y -  (0.5f - Offset.Y) - 2 * Offset.Y);
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
	FVector CurrentLocation = GetActorLocation();


	if (Factor.X > 1 - CameraMovementMargin&& CurrentLocation.Y <= MapSize.Y * 100 / 2)
	{
		CurrentLocation += (FVector(0.f, CameraMaxSpeed, 0.f)*FMath::Pow(CameraMaxSpeed - CameraMinSpeed, (CameraMovementMargin - (1 - Factor.X)) / CameraMovementMargin));


	}
	else if (Factor.X < CameraMovementMargin &&CurrentLocation.Y >= -MapSize.Y * 100 / 2)
	{
		CurrentLocation += (FVector(0.f, -CameraMaxSpeed, 0.f)*FMath::Pow(CameraMaxSpeed - CameraMinSpeed, (CameraMovementMargin - Factor.X) / CameraMovementMargin));

	}

	if (Factor.Y>1 - CameraMovementMargin && CurrentLocation.X >= -MapSize.X * 100 / 2)
	{
		CurrentLocation += (FVector(-CameraMaxSpeed, 0.f, 0.f)*FMath::Pow(CameraMaxSpeed - CameraMinSpeed, (CameraMovementMargin - (1 - Factor.Y)) / CameraMovementMargin));
	}
	else if (Factor.Y < CameraMovementMargin && CurrentLocation.X <= MapSize.X * 100 / 2)
	{
		CurrentLocation += (FVector(CameraMaxSpeed, 0.f, 0.f)*FMath::Pow(CameraMaxSpeed - CameraMinSpeed, (CameraMovementMargin - Factor.Y) / CameraMovementMargin));
	}
	SetActorLocation(CurrentLocation);
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
			CurrentPreviewBuilding->SetActorLocation(FVector(GridLocation.X * 100, GridLocation.Y * 100, MouseHitResult.ImpactPoint.Z));

			if (CurrentPreviewBuilding->Mesh->StaticMesh != TestBuilding->BuildMeshes[4])
			{
				CurrentPreviewBuilding->Mesh->SetStaticMesh(TestBuilding->BuildMeshes[4]);
				CurrentPreviewBuilding->Mesh->SetMaterial(0, TestBuilding->BuildMeshes[4]->GetMaterial(0));
				CurrentPreviewBuilding->Material = CurrentPreviewBuilding->Mesh->CreateAndSetMaterialInstanceDynamic(0);
				float Scaling = TestBuilding->Size.X*0.1;
				CurrentPreview->SetActorScale3D(FVector(Scaling, Scaling, Scaling));
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
				CurrentPreview = World->SpawnActor<ABuildingPreview>(BuildingPreviewObject, FVector(GridLocation.X * 100, GridLocation.Y * 100, MouseHitResult.ImpactPoint.Z), FRotator(0.f, 0.f, 0.f), SpawnParameters);
				ABuildingPreview* const CurrentPreviewBuilding = Cast<ABuildingPreview>(CurrentPreview);
				CurrentPreviewBuilding->Mesh->SetStaticMesh(TestBuilding->BuildMeshes[4]);
				CurrentPreviewBuilding->Material = CurrentPreviewBuilding->Mesh->CreateAndSetMaterialInstanceDynamic(0);
				float Scaling = TestBuilding->Size.X*0.1;
				CurrentPreview->SetActorScale3D(FVector(Scaling, Scaling, Scaling));
			}
			else
			{
				CurrentPreview->Destroy();
			}
		}
	}
}

TArray<APawn*> APlayerCharacter::GetFreeNpcsByState(TArray<APawn*> NpcArray, FString Job, FString Task)
{
	TArray<APawn*> ReturnedActors;
	for (int32 Index = 0; Index < NpcArray.Num(); Index++)
	{
		ANpcController* const Npc = Cast<ANpcController>(NpcArray[Index]->GetController());
		if (Npc)
		{
			if (Task == "" && Npc->Task == Task || Job == "" && Npc->Job == Job || Npc->Task == Task && Npc->Job == Job)
			{
				ReturnedActors.Add(NpcArray[Index]);
			}
		}

	}
	return ReturnedActors;
}

void APlayerCharacter::ChangeItem(int32 Quantity, int32 ID)
{
	UE_LOG(LogTemp, Warning, TEXT("%d"), OwnedStorageBlocks.Num());
	for (int32 BlockIndex = 0; BlockIndex < OwnedStorageBlocks.Num(); BlockIndex++)
	{
		AStorageBlock* const Block = Cast<AStorageBlock>(OwnedStorageBlocks[BlockIndex]);
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
		AStorageBlock* const Block = Cast<AStorageBlock>(OwnedStorageBlocks[BlockIndex]);
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
						Stack->SetActorScale3D(FVector(0.1f,0.1f,0.1f));



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

void APlayerCharacter::SetModeToBuildExtend()
{
	UE_LOG(LogTemp, Warning, TEXT("SetModeToBuildExtend called"));
	ABuilding* const Building = Cast<ABuilding>(SelectedActor);
	if (Building)
	{
		UE_LOG(LogTemp, Warning, TEXT("SetModeToBuildExtend success"));
		Mode = "BuildExtension";
		SelectedBuilding = Building->ExtensionBuildingClass;
	}
}