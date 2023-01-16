// Fill out your copyright notice in the Description page of Project Settings.


#include "JettDash_Ability.h"


AJettDash_Ability::AJettDash_Ability() {

	NumberOfCharges = 1;

	AbilityForce = 10000.0f;


}

void AJettDash_Ability::BeginPlay()
{
	Super::BeginPlay();
}