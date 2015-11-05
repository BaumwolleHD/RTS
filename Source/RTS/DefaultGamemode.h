// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameMode.h"
#include "NpcCharacter.h"
#include "NpcController.h"
#include "DefaultGamestate.h"
#include "DefaultGamemode.generated.h"

/**
 * 
 */
UCLASS()
class RTS_API ADefaultGamemode : public AGameMode
{
	GENERATED_BODY()
public:
	ADefaultGamemode();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Npcs)
		bool bSpawnBeggers;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Npcs)
		int32 SpawnRate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Npcs)
		int32 Spawncount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Npcs)
		float SpawnTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Npcs)
		TSubclassOf<class ANpcCharacter> NpcClass;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gamemode)
		ADefaultGamestate* CurrentGamestate;


protected:
	void NpcInfluxHandler(float DeltaTime);

	
};
