 //Fill out your copyright notice in the Description page of Project Settings.

#include "RTS.h"
#include "NpcController.h"
#include "ResourceBuilding.h"
#include "PlayerCharacter.h"


ANpcController::ANpcController()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Task = "Free";
}


// Called when the game starts or when spawned
void ANpcController::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ANpcController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	if (IsTargetReached())
	{
		FindNextTask();
	}
	else
	{
		MoveToTarget();
	}

}

void ANpcController::FindNextTask()
{
	if (Task == "MoveToPickup")
	{
		AResourceBuilding* const Building = Cast<AResourceBuilding>(TargetActor);
		if (Building)
		{
			ItemID = Building->ProductionID;
			ItemQuantity = Building->ProductionQuantity;
			Building->GrowProgression = 0.f;
			Building->GrowProgressionState = 4;
			
			SetTaskToStorage();
			
		}
	}
	else if (Task == "MoveToStorage")
	{
		PlaceItemsInStorage();
	}

}

bool ANpcController::IsTargetReached()
{
	APawn* const Pawn = GetPawn();
	if (Pawn)
	{
		float const Distance = FVector::Dist(TargetLocation, Pawn->GetActorLocation());
		if (Distance > 200.f)
		{
			return false;
		}
		else
		{
			return true;
		}
	}
	return false;
}

void ANpcController::MoveToTarget()
{
	APawn* const Pawn = GetPawn();
	if (Pawn)
	{

		UNavigationSystem* const NavSys = GetWorld()->GetNavigationSystem();
		float const Distance = FVector::Dist(TargetLocation, Pawn->GetActorLocation());

		// We need to issue move command only if far enough in order for walk animation to play correctly
		if (NavSys && (Distance > 120.0f))
		{
			MoveToLocation(TargetLocation, 120.f);
		}
	}
}

void ANpcController::SetTarget(AActor* Target)
{
	TargetActor = Target;
	TargetLocation = Target->GetActorLocation();
}

void ANpcController::SetTaskToPickup(AActor* TargetResourceBuilding)
{
	TargetActor = TargetResourceBuilding;
	TargetLocation = TargetResourceBuilding->GetActorLocation();
	Moving = true;
	Task = "MoveToPickup";
}

void ANpcController::SetTaskToStorage()
{
	
	Task = "MoveToStorage";
	TargetActor = NULL;
	TargetLocation = FVector(0,0,0);;
	Moving = true;
}

int32 ANpcController::PlaceItemsInStorage()
{
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetPawn()->GetOwner());
	if (PlayerCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("%d, %d, %d"), ItemID, ItemQuantity, PlayerCharacter->OwnedStorageBlock.Num());
		PlayerCharacter->ChangeItem(ItemQuantity, ItemID);
		Task = "Free";
	}
	return 0;
}
