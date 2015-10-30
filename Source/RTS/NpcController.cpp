#include "RTS.h"
#include "NpcController.h"
#include "ResourceBuilding.h"
#include "PlayerCharacter.h"


ANpcController::ANpcController()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Tasks.Add(ENpcTask::Free);
	TargetActors.AddDefaulted();
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


	if (IsTargetReached() || Tasks[0] == ENpcTask::Free && Tasks.Num() > 1)
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
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetPawn()->GetOwner());
	switch (Tasks[0])
	{
	case ENpcTask::Free:
		break;

	case ENpcTask::PickupItemsFromBuilding:

		AResourceBuilding* Building;
		Building = Cast<AResourceBuilding>(TargetActors[0]);
		if (Building)
		{
			CarriedItemID = Building->ProductionID;
			CarriedItemQuantity = Building->ProductionQuantity;
			Building->GrowProgression = 0.f;
			Building->GrowProgressionState = 0;
			Building->SendGrowStateUpdateToClients(0.f);

		}
		break;



	case ENpcTask::DropItemsToStorage:
		if (PlayerCharacter){ PlayerCharacter->ChangeItem(CarriedItemQuantity, CarriedItemID); }
		CarriedItemID = CarriedItemQuantity = 0;
		//Task = ENpcTask::Free;
		break;

	case ENpcTask::PickupItemsFromStorage:
		CarriedItemID = NeededItemID;
		CarriedItemQuantity = FMath::Min(NeededItemQuantity, PlayerCharacter->CheckForQuantity(NeededItemID));
		if (PlayerCharacter){ PlayerCharacter->ChangeItem(CarriedItemQuantity, CarriedItemID); }
		break;


	case ENpcTask::DropItemsToBuilding:
		Building = Cast<AResourceBuilding>(TargetActors[0]);
		if (Building)
		{
			Building->CurrentConsumptionQuantity = FMath::Min(Building->ConsumptionQuantity, Building->CurrentConsumptionQuantity + CarriedItemQuantity);
			CarriedItemID = CarriedItemQuantity = 0;
			Building->GrowProgressionState = 0;
		}
		break;



	}

	Tasks.RemoveAt(0);
	if (Tasks.Num() == 0)
	{ 
		Tasks.Add(ENpcTask::Free); 
	}
	else
	{


		if (TargetActors.Num() == 0)
		{
			Moving = false;
			return;
		}
		else
		{
			TargetActors.RemoveAt(0);
		}


		Moving = true;
		switch (Tasks[0])
		{
		case ENpcTask::Free:
			Moving = false;
			return;

		case ENpcTask::DropItemsToBuilding:
			TargetLocation = TargetActors[0]->GetActorLocation();
			break;

		case ENpcTask::DropItemsToStorage:
			if (PlayerCharacter && PlayerCharacter->OwnedStorageBuilding)
			{
				//TargetActor = PlayerCharacter->OwnedStorageBuilding;
				TargetLocation = PlayerCharacter->OwnedStorageBuilding->GetActorLocation();
			}
			break;

		case ENpcTask::PickupItemsFromBuilding:
			TargetLocation = TargetActors[0]->GetActorLocation();
			break;

		case ENpcTask::PickupItemsFromStorage:
			if (PlayerCharacter && PlayerCharacter->OwnedStorageBuilding)
			{
				//TargetActor = PlayerCharacter->OwnedStorageBuilding;
				TargetLocation = PlayerCharacter->OwnedStorageBuilding->GetActorLocation();
			}

			break;

		}
		
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

		if (NavSys && Distance > 120.f)
		{
			MoveToLocation(TargetLocation, 120.f);
		}
	}
}

void ANpcController::AddTask(ENpcTask Task, AActor* TargetActor)
{
	Tasks.Add(Task);
	TargetActors.Add(TargetActor);
}

