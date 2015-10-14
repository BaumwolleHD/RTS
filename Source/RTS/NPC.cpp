// Fill out your copyright notice in the Description page of Project Settings.

#include "RTS.h"
#include "NPC.h"
#include "ResourceBuilding.h"
#include "Building.h"
#include "AI/Navigation/NavigationSystem.h"



// Sets default values
ANPC::ANPC()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Moving = true;

}

// Called when the game starts or when spawned
void ANPC::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ANPC::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
	

	if (IsTargetReached())
	{
		UE_LOG(LogTemp, Warning, TEXT("reached"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("not reached"));
		MoveToTarget();
	}

}

bool ANPC::IsTargetReached()
{
	APawn* const Pawn = GetPawn();
	if (Pawn)
	{
		float const Distance = FVector::Dist(TargetLocation, Pawn->GetActorLocation());
		if (Distance > 100.f)
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

void ANPC::MoveToTarget()
{
	APawn* const Pawn = GetPawn();
	if (Pawn)
	{
		
		UNavigationSystem* const NavSys = GetWorld()->GetNavigationSystem();
		float const Distance = FVector::Dist(TargetLocation, Pawn->GetActorLocation());

		// We need to issue move command only if far enough in order for walk animation to play correctly
		if (NavSys && (Distance > 120.0f))
		{
			UE_LOG(LogTemp, Warning, TEXT("move"));
			//NavSys->SimpleMoveToLocation(this, FVector(0,0,0));
			MoveToLocation(TargetLocation, 120.f);
		}
	}
}

void ANPC::SetTarget(AActor* Target)
{
	TargetActor = Target;
	TargetLocation = Target->GetActorLocation();
}

void ANPC::SetTaskToPickup(AActor* TargetResourceBuilding)
{
	TargetActor = TargetResourceBuilding;
	TargetLocation = TargetResourceBuilding->GetActorLocation();
	Moving = true;
	Task = "MoveToPickup";
}

int32 ANPC::PickupItemsFromResorceBuilding()
{
	return 0;
}
