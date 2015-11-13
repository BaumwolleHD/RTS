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
#include "Defaultgamemode.h"
#include "DefaultGamestate.h"
#include "DefaultPlayerstate.h"
#include "Engine.h"
#include "DefaultWidget.h"
#include "Blueprint/UserWidget.h"


// Sets default values
APlayerCharacter::APlayerCharacter()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Mode = EPlayerMode::Build;
	CameraMovementMargin = 0.1f;
	CameraMaxSpeed = 100.f;
	CameraMinSpeed = 50.f;
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	UGameplayStatics::GetPlayerController(this, 0)->bShowMouseCursor = true;

	if (HudWidgetClass != nullptr)
	{
		CurrentWidget = CreateWidget<UUserWidget>(GetWorld(), HudWidgetClass);
		if (CurrentWidget)
		{
			CurrentWidget->AddToViewport();
		}
	}

	CurrentGamestate = Cast<ADefaultGamestate>(UGameplayStatics::GetGameState(this));
	CurrentPlayerstate = Cast<ADefaultPlayerstate>(PlayerState);
	//UE_LOG(LogTemp, Warning, TEXT("Spawn: %s"), *UGameplayStatics::GetPlayerPawn(this, 0)->GetName());

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
		if (Mode == EPlayerMode::Build || Mode == EPlayerMode::BuildExtension)
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
	InputComponent->BindAction("MouseScrollUp", IE_Pressed, this, &APlayerCharacter::OnMouseWheelUp);
	InputComponent->BindAction("MouseScrollDown", IE_Pressed, this, &APlayerCharacter::OnMouseWheelDown);


}
void APlayerCharacter::OnLeftClick()
{
	if (Mode == EPlayerMode::Build || Mode == EPlayerMode::BuildExtension)
	{

		PlaceBuilding(MouseHitResult, SelectedBuilding);
	}
	else
	{
		SelectActor();
	}
}

void APlayerCharacter::OnMouseWheelUp()
{
	if (Mode == EPlayerMode::Build || Mode == EPlayerMode::BuildExtension)
	{

		ABuildingPreview* const CurrentPreviewBuilding = Cast<ABuildingPreview>(CurrentGamestate->CurrentPreview);
		if (CurrentPreviewBuilding)
		{
			CurrentPreviewBuilding->Rotation -= 90;
			CurrentPreviewBuilding->SetActorRotation(FRotator(0.f, CurrentPreviewBuilding->Rotation, 0.f));
		}
	}

}
void APlayerCharacter::OnMouseWheelDown()
{
	if (Mode == EPlayerMode::Build || Mode == EPlayerMode::BuildExtension)
	{

		ABuildingPreview* const CurrentPreviewBuilding = Cast<ABuildingPreview>(CurrentGamestate->CurrentPreview);
		if (CurrentPreviewBuilding)
		{
			CurrentPreviewBuilding->Rotation += 90;
			CurrentPreviewBuilding->SetActorRotation(FRotator(0.f, CurrentPreviewBuilding->Rotation, 0.f));
		}
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

bool APlayerCharacter::PlaceBuilding_Validate(FHitResult HitResult, TSubclassOf<ABuilding> Building) { return true; } //Anti-Cheat

void APlayerCharacter::PlaceBuilding_Implementation(FHitResult HitResult, TSubclassOf<ABuilding> Building)
{
	if (HitResult.bBlockingHit == true)
	{
		UWorld* World = GetWorld();
		//APlayerCharacter* PossesedPlayerCharacter = Cast<APlayerCharacter>(Cast<ADefaultGamemode>(UGameplayStatics::GetGameMode(this))->PlayerList[UserIndex]);
		if (World)
		{
			FVector2D Size;
			ABuilding* const TestBuilding = Building.GetDefaultObject();
			ABuildingPreview* const CurrentPreviewBuilding = Cast<ABuildingPreview>(CurrentGamestate->CurrentPreview);
			if (CurrentPreviewBuilding)
			{
				
				if (FMath::Fmod(CurrentPreviewBuilding->Rotation, 180) == 0)
				{
					Size = TestBuilding->Size;

				}
				else
				{
					Size = FVector2D(TestBuilding->Size.Y, TestBuilding->Size.X);
				}
			}

			FVector2D Grid2DPosition = ApplyGrid(FVector2D(HitResult.ImpactPoint.X, HitResult.ImpactPoint.Y), Size);

			if (CanPlaceBuilding(Grid2DPosition, Size) && (Mode == EPlayerMode::Build || CanPlaceBuildingExtension(Grid2DPosition, SelectedActor)))
			{
				FActorSpawnParameters SpawnParameters;
				SpawnParameters.Owner = this;
				SpawnParameters.Instigator = Instigator;
				ABuilding* const SpawnedBuilding = World->SpawnActor<ABuilding>(Building, FVector(Grid2DPosition.X * 100, Grid2DPosition.Y * 100, 20.f), FRotator(0.f, 0.f, 0.f), SpawnParameters);

				CurrentPlayerstate->OwnedBuildings.Add(SpawnedBuilding);
				

				UpdateBlocks(Grid2DPosition, Size, FString::FromInt(TestBuilding->ID));
				SpawnedBuilding->Rotation = CurrentPreviewBuilding->Rotation;

				if (Mode == EPlayerMode::BuildExtension)
				{
					ABuilding* const Building = Cast<ABuilding>(SelectedActor);
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
			if (CurrentGamestate->PP_BlockID.Contains(SerializeFVector2D(FVector2D(X, Y))))
			{
				return false;
			}


		}
	}
	return true;
}

bool APlayerCharacter::CanPlaceBuildingExtension(FVector2D Position, AActor* MasterBuilding)
{
	ABuilding* const Building = Cast<ABuilding>(MasterBuilding);
	if (Building)
	{
		FVector MasterPosition = Building->GetActorLocation();
		float Distance = FVector2D::Distance(Position * 100, FVector2D(MasterPosition.X, MasterPosition.Y));
		if (Distance <= Building->MaxExtensionDistance*100)
		{
			return true;
		}
		return false;
	}
	return false;
}

FVector2D APlayerCharacter::ApplyGrid(FVector2D Location, FVector2D Size)
{
	Location.X = float(FMath::RoundToInt(Location.X / 100) + 0.5*int(!(int(Size.X) % 2)));
	Location.Y = float(FMath::RoundToInt(Location.Y / 100) + 0.5*int(!(int(Size.Y) % 2)));
	return Location;
}




void APlayerCharacter::UpdateBlocks_Implementation(FVector2D Position, FVector2D Size, const FString& ID)
{
	FString SerializedPosition = "";
		FString CenterSerializedPosition = SerializeFVector2D(Position);
	
		if (CurrentGamestate->PP_BlockID.Contains(CenterSerializedPosition))
		{
			CurrentGamestate->PP_BlockID[CenterSerializedPosition] = ID;
		}
		else
		{
			CurrentGamestate->PP_BlockID.Add(CenterSerializedPosition, ID);
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
					if (CurrentGamestate->PP_BlockID.Contains(SerializedPosition))
					{
						CurrentGamestate->PP_BlockID[SerializedPosition] = CenterSerializedPosition;
					}
					else
					{
						CurrentGamestate->PP_BlockID.Add(SerializedPosition, CenterSerializedPosition);
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
	if (MouseHitResult.bBlockingHit && CurrentPlayerstate)
	{
		FVector2D Size;
		ABuilding* const TestBuilding = SelectedBuilding.GetDefaultObject();
		ABuildingPreview* const CurrentPreviewBuilding = Cast<ABuildingPreview>(CurrentGamestate->CurrentPreview);
		if (CurrentPreviewBuilding)
		{

			if (FMath::Fmod(CurrentPreviewBuilding->Rotation, 180) == 0)
			{
				Size = TestBuilding->Size;

			}
			else
			{
				Size = FVector2D(TestBuilding->Size.Y, TestBuilding->Size.X);
			}
		}
		
		FVector2D GridLocation = ApplyGrid(FVector2D(MouseHitResult.ImpactPoint.X, MouseHitResult.ImpactPoint.Y), Size);
		const FString PassedString = FString::FromInt(GridLocation.X) + "|" + FString::FromInt(GridLocation.Y);
		//ADefaultGamestate* const Gamestate = Cast<ADefaultGamestate>(UGameplayStatics::GetGameState(this));
		if (CurrentGamestate && CurrentGamestate->CurrentPreview != nullptr && IsRelevant)
		{
			ABuildingPreview* const CurrentPreviewBuilding = Cast<ABuildingPreview>(CurrentGamestate->CurrentPreview);
			if (CurrentPreviewBuilding)
			{
				
				CurrentPreviewBuilding->SetActorLocation(FVector(GridLocation.X * 100, GridLocation.Y * 100, MouseHitResult.ImpactPoint.Z));

				if (CurrentPreviewBuilding->Mesh->StaticMesh != TestBuilding->BuildMeshes[TestBuilding->BuildMeshes.Num() - 1])
				{

					CurrentPreviewBuilding->Mesh->SetStaticMesh(TestBuilding->BuildMeshes[TestBuilding->BuildMeshes.Num() - 1]);
					CurrentPreviewBuilding->Mesh->SetMaterial(0, TestBuilding->BuildMeshes[TestBuilding->BuildMeshes.Num() - 1]->GetMaterial(0));
					CurrentPreviewBuilding->Material = CurrentPreviewBuilding->Mesh->CreateAndSetMaterialInstanceDynamic(0);
					float Scaling = TestBuilding->Size.X*0.1;
					CurrentPreviewBuilding->Mesh->SetRelativeScale3D(FVector(Scaling, Scaling, Scaling));
					//UE_LOG(LogTemp, Warning, TEXT("Self: %s, Building: %s"), *GetName(), *CurrentPlayerstate->SelectedBuilding->GetName());
				}

				if (CanPlaceBuilding(GridLocation, Size) && (Mode == EPlayerMode::Build || CanPlaceBuildingExtension(GridLocation, SelectedActor)))
				{
					CurrentPreviewBuilding->Material->SetVectorParameterValue(FName("PreviewColor"), FLinearColor(0, 0.8f, 0));
				}
				else
				{
					CurrentPreviewBuilding->Material->SetVectorParameterValue(FName("PreviewColor"), FLinearColor(1, 0, 0));
				}
			}
		}
	}
}

TArray<APawn*> APlayerCharacter::GetNpcsByState(TArray<APawn*> NpcArray, ENpcJob Job, ENpcTask Task)
{
	TArray<APawn*> ReturnedActors;
	for (int32 Index = 0; Index < NpcArray.Num(); Index++)
	{
		ANpcController* const Npc = Cast<ANpcController>(NpcArray[Index]->GetController());
		if (Npc)
		{
			if (Task == ENpcTask::Empty && Npc->Tasks[0] == Task || Job == ENpcJob::Empty && Npc->Job == Job || Npc->Tasks[0] == Task && Npc->Job == Job)
			{
				ReturnedActors.Add(NpcArray[Index]);
			}
		}

	}
	return ReturnedActors;
}


void APlayerCharacter::UpdateQuantity()
{
	int32 ItemIndex = 0;


	for (ItemIndex; ItemIndex < CurrentPlayerstate->OwnedItems.Num(); ItemIndex++)
	{
		CurrentPlayerstate->OwnedItems[ItemIndex] = 0;
	}

	for (int32 BlockIndex = 0; BlockIndex < CurrentPlayerstate->OwnedStorageBlocks.Num(); BlockIndex++)
	{
		AStorageBlock* const Block = Cast<AStorageBlock>(CurrentPlayerstate->OwnedStorageBlocks[BlockIndex]);
		if (Block && Block->BuildProgressionState == 5)
		{
			for (int32 StackIndex = 0; StackIndex < Block->StorageStacks.Num(); StackIndex++)
			{
				AStorageStack* const Stack = Cast<AStorageStack>(Block->StorageStacks[StackIndex]);
				if (Stack)
				{
					ItemClasses.Find(Stack->GetClass(), ItemIndex);
					CurrentPlayerstate->OwnedItems[ItemIndex] += Stack->Quantity;
				}
			}
		}
	}
	UDefaultWidget* Widget = Cast<UDefaultWidget>(CurrentWidget);
	if (Widget)
	{
		Widget->UpdateItems(CurrentPlayerstate->OwnedItems);

	}
}

int32 APlayerCharacter::CheckForQuantity(int32 ID)
{
	int32 Quantity = 0;
	for (int32 BlockIndex = 0; BlockIndex < CurrentPlayerstate->OwnedStorageBlocks.Num(); BlockIndex++)
	{
		AStorageBlock* const Block = Cast<AStorageBlock>(CurrentPlayerstate->OwnedStorageBlocks[BlockIndex]);
		if (Block && Block->BuildProgressionState == 5)
		{
			for (int32 StackIndex = 0; StackIndex < Block->StorageStacks.Num(); StackIndex++)
			{
				AStorageStack* const Stack = Cast<AStorageStack>(Block->StorageStacks[StackIndex]);
				if (Stack)
				{
					if (Stack->GetClass() == ItemClasses[ID])
					{
						Quantity += Stack->Quantity;
					}
				}
			}
		}
	}
	return Quantity;

}

bool APlayerCharacter::ChangeItem_Validate(int32 ID, int32 Quantity) { return true; } //Anti-Cheat

void APlayerCharacter::ChangeItem_Implementation(int32 ID, int32 Quantity)
{
	UE_LOG(LogTemp, Warning, TEXT("owned Blocks: %d"), CurrentPlayerstate->OwnedStorageBlocks.Num());

	for (int32 BlockIndex = 0; BlockIndex < CurrentPlayerstate->OwnedStorageBlocks.Num(); BlockIndex++)
	{
		AStorageBlock* const Block = Cast<AStorageBlock>(CurrentPlayerstate->OwnedStorageBlocks[BlockIndex]);
		if (Block && Block->BuildProgressionState == 5)
		{
			for (int32 StackIndex = 0; StackIndex < Block->StorageStacks.Num(); StackIndex++)
			{
				AStorageStack* const Stack = Cast<AStorageStack>(Block->StorageStacks[StackIndex]);
				if (Stack)
				{
					if (Stack->GetClass() == ItemClasses[ID])
					{
						if (Quantity + Stack->Quantity <= Stack->MaxQuantity && Quantity + Stack->Quantity > 0)
						{
							Stack->Quantity += Quantity;
							Stack->Sort();
							Quantity = 0;
							UpdateQuantity();
							return;
						}
						else if (Quantity + Stack->Quantity == Stack->MaxQuantity)
						{
							Quantity -= Stack->MaxQuantity - Stack->Quantity;
							Stack->Quantity = Stack->MaxQuantity;
							Stack->Sort();
						}
						else if (Quantity + Stack->Quantity == 0)
						{
							Quantity += Stack->Quantity;
							int32 StorageStackIndex = 0;
							Block->StorageStacks.Find(Stack, StorageStackIndex);
							Block->StorageStacks[StorageStackIndex] = NULL;
							Stack->Destroy();

						}
					}
				}
			}
		}
	}
	if (Quantity == 0)
	{
		UpdateQuantity();
		return;
	}
	if (Quantity > 0)
	{
		for (int32 BlockIndex = 0; BlockIndex < CurrentPlayerstate->OwnedStorageBlocks.Num(); BlockIndex++)
		{
			AStorageBlock* const Block = Cast<AStorageBlock>(CurrentPlayerstate->OwnedStorageBlocks[BlockIndex]);
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
							FVector Location = Block->GetItemPosition(StackIndex);
							AStorageStack* const Stack = World->SpawnActor<AStorageStack>(ItemClasses[ID], Location, FRotator(0.f, 0.f, 0.f), SpawnParameters);
							Block->StorageStacks[StackIndex] = Stack;
							Stack->SetActorRotation(FRotator(0, int(FMath::RandBool())*90.f, 0));
							Stack->SetActorScale3D(FVector(0.2f, 0.2f, 0.2f));



							if (Block->StorageStacks[StackIndex]->GetClass() == ItemClasses[ID])
							{
								if (Quantity <= Stack->MaxQuantity)
								{
									Stack->Quantity = Quantity;
									Stack->Sort();
									Quantity = 0;
									UpdateQuantity();
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
	UpdateQuantity();
}

void APlayerCharacter::SetModeToBuildExtend()
{
	UE_LOG(LogTemp, Warning, TEXT("SetModeToBuildExtend called"));
	ABuilding* const Building = Cast<ABuilding>(SelectedActor);
	if (Building && Role < ROLE_Authority)
	{
		UE_LOG(LogTemp, Warning, TEXT("SetModeToBuildExtend success"));
		Mode = EPlayerMode::BuildExtension;
		SelectedBuilding = Building->ExtensionBuildingClass;
	}
}