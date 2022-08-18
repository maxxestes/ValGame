// Fill out your copyright notice in the Description page of Project Settings.


#include "SageWallComp.h"


USageWallComp::USageWallComp() {
	Health = 100;
	wallMesh = CreateDefaultSubobject<UStaticMesh>(TEXT("WallMesh"));
}

void USageWallComp::TakeDamage(float damage)
{
	Health -= damage;
	if (Health <= 0) {
		this->DestroyComponent();
	}
}

float USageWallComp::getHealth()
{
	return this->Health;
}

void USageWallComp::setHealth(float newHealth)
{
	if (newHealth <= 0) {
		DestroyComponent();
	}
	this->Health = newHealth;
}
