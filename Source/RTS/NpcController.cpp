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

	if (Role == ROLE_Authority)
	{
		if (IsTargetReached() || Tasks[0] == ENpcTask::Free && Tasks.Num() > 1)
		{


			FindNextTask();


		}
		else
		{
			MoveToTarget();
		}
	}
}

void ANpcController::FindNextTask()
{
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetPawn()->GetOwner());
	AResourceBuilding* ResourceBuilding;
	switch (Tasks[0])
	{
	case ENpcTask::Free:
		break;

	case ENpcTask::PickupItemsFromBuilding:

		
		ResourceBuilding = Cast<AResourceBuilding>(TargetActors[0]);
		if (ResourceBuilding)
		{
			CarriedItemID = ResourceBuilding->ProductionID;
			CarriedItemQuantity = FMath::Min(ResourceBuilding->CurrentProductionQuantity, MaxCarriedItemQuantity);
			ResourceBuilding->CurrentProductionQuantity -= CarriedItemQuantity;
			ResourceBuilding->GrowProgression = 0.f;
			ResourceBuilding->GrowProgressionState = 0;
			

		}
		break;



	case ENpcTask::DropItemsToStorage:
		UE_LOG(LogTemp, Warning, TEXT("Kappa0; %s"), *GetPawn()->GetOwner()->GetName());
		if (PlayerCharacter){ PlayerCharacter->ChangeItem(CarriedItemID, CarriedItemQuantity); 	UE_LOG(LogTemp, Warning, TEXT("Kappa1")); }
		CarriedItemID = CarriedItemQuantity = 0;
		//Task = ENpcTask::Free;
		break;

	case ENpcTask::PickupItemsFromStorage:
		CarriedItemID = NeededItemID;
		CarriedItemQuantity = FMath::Min(FMath::Min(NeededItemQuantity, PlayerCharacter->CheckForQuantity(NeededItemID)), MaxCarriedItemQuantity);
		NeededItemID = NeededItemQuantity = 0;
		if (PlayerCharacter){ PlayerCharacter->ChangeItem(CarriedItemID, -CarriedItemQuantity); }
		break;


	case ENpcTask::DropItemsToBuilding:
		ResourceBuilding = Cast<AResourceBuilding>(TargetActors[0]);
		if (ResourceBuilding)
		{

			int32 OldConsumption;
			OldConsumption = ResourceBuilding->CurrentConsumptionQuantity;
			//ResourceBuilding->CurrentConsumptionQuantity = FMath::Min(ResourceBuilding->ConsumptionQuantity, ResourceBuilding->CurrentConsumptionQuantity + CarriedItemQuantity);
			
			
			if (Role == ROLE_Authority)
			{
				ResourceBuilding->SendConsumeStateUpdateToClients(float(FMath::Min(ResourceBuilding->ConsumptionQuantity, ResourceBuilding->CurrentConsumptionQuantity + CarriedItemQuantity)-1) / ResourceBuilding->ConsumptionQuantity);
			}
			ResourceBuilding->GrowProgressionState = 0;
			CarriedItemQuantity -= ResourceBuilding->CurrentConsumptionQuantity - OldConsumption;
			if (CarriedItemQuantity == 0)
			{ 
				CarriedItemID = 0; 
			}
			else
			{
				AddTask(ENpcTask::DropItemsToStorage, PlayerCharacter->OwnedStorageBuilding, ENpcTaskPriority::Urgent);
			}
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
			TargetLocation = TargetActors[0]->GetActorLocation();
			break;

		case ENpcTask::PickupItemsFromBuilding:
			TargetLocation = TargetActors[0]->GetActorLocation();
			break;

		case ENpcTask::PickupItemsFromStorage:
			TargetLocation = TargetActors[0]->GetActorLocation();
			break;


		case ENpcTask::GetBuildingConsumption:
			ResourceBuilding = Cast<AResourceBuilding>(TargetActors[0]);
			if (ResourceBuilding)
			{

				NeededItemID = ResourceBuilding->ConsumptionID;
				NeededItemQuantity = ResourceBuilding->ConsumptionQuantity - ResourceBuilding->CurrentConsumptionQuantity;


				//TargetActor = PlayerCharacter->OwnedStorageBuilding;
				TargetLocation = GetPawn()->GetActorLocation();
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

void ANpcController::AddTask(ENpcTask Task, AActor* TargetActor, ENpcTaskPriority Priority)
{
	if (Priority == ENpcTaskPriority::Low)
	{
		Tasks.Add(Task);
		TargetActors.Add(TargetActor);
	}
	else
	{
		Tasks.Insert(Task, 0);
		TargetActors.Insert(TargetActor, 0);
	}
}

