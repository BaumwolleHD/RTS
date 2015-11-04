// Fill out your copyright notice in the Description page of Project Settings.

#include "RTS.h"
#include "NpcCharacter.h"


// Sets default values
ANpcCharacter::ANpcCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

}