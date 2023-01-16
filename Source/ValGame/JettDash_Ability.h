// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterAbility.h"
#include "JettDash_Ability.generated.h"

/**
 * 
 */
UCLASS()
class VALGAME_API AJettDash_Ability : public ACharacterAbility
{
	GENERATED_BODY()

public:

	AJettDash_Ability();

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float AbilityForce;

	
};
