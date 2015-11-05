// Fill out your copyright notice in the Description page of Project Settings.

#include "RTS.h"
#include "DefaultGamemode.h"
#include "DefaultPlayerstate.h"
#include "DefaultGamestate.h"
#include "Blueprint/UserWidget.h"


ADefaultGamemode::ADefaultGamemode()
{

}

void ADefaultGamemode::BeginPlay()
{
	Super::BeginPlay();

	CurrentGamestate = Cast<ADefaultGamestate>(UGameplayStatics::GetGameState(this));

}

void ADefaultGamemode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	NpcInfluxHandler(DeltaTime);
}


void ADefaultGamemode::NpcInfluxHandler(float DeltaTime)
{
	if (Spawncount < 4)
	{
		SpawnTimer -= DeltaTime;

		if (SpawnTimer <= 0)
		{
			
			UWorld* World = GetWorld();
			if (World)
			{
				Spawncount++;
				SpawnTimer = SpawnRate * FMath::FRandRange(0.75f, 1.25f);
				for (int32 UserIndex = 0; UserIndex < CurrentGamestate->PlayerCharacterList.Num(); UserIndex++)
				{
					FActorSpawnParameters SpawnParameters;
					SpawnParameters.Owner = CurrentGamestate->PlayerCharacterList[UserIndex];
					SpawnParameters.Instigator = Instigator;
					ANpcCharacter* const SpawnedNpc = World->SpawnActor<ANpcCharacter>(NpcClass, FVector(1000.f, 0.f, 100.f), FRotator(0.f, 0.f, 0.f), SpawnParameters);
					Cast<ADefaultPlayerstate>(CurrentGamestate->PlayerStateList[UserIndex])->OwnedNpcs.Add(SpawnedNpc);
					//Cast<ANpcController>(SpawnedNpc->GetController())->Job = ENpcJob::StorageWorker;


				}
			}
		}
	}
}