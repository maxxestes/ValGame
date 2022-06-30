// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterAbility.h"

// Sets default values
ACharacterAbility::ACharacterAbility()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void ACharacterAbility::CastAbility(FVector lookDirection)
{

}

// Called when the game starts or when spawned
void ACharacterAbility::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACharacterAbility::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

