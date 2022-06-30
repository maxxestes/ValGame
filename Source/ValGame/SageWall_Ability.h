// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterAbility.h"
#include "SageWall_Ability.generated.h"

/**
 * 
 */
UCLASS()
class VALGAME_API ASageWall_Ability : public ACharacterAbility
{
	GENERATED_BODY()
	public:

		ASageWall_Ability();

		void PlaceWall(FVector placeLocation);

	private:

		USceneComponent* SceneComponent;

		UStaticMesh* WallComponent;
	
};
