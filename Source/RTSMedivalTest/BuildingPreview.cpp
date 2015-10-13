// Fill out your copyright notice in the Description page of Project Settings.

#include "RTSMedivalTest.h"
#include "BuildingPreview.h"


// Sets default values
ABuildingPreview::ABuildingPreview()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;
	Mesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
}

// Called when the game starts or when spawned
void ABuildingPreview::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ABuildingPreview::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

