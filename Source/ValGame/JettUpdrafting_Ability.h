// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterAbility.h"
#include "JettUpdrafting_Ability.generated.h"

/**
 * 
 */
UCLASS()
class VALGAME_API AJettUpdrafting_Ability : public ACharacterAbility
{
	GENERATED_BODY()

	public:

		AJettUpdrafting_Ability();

		virtual void BeginPlay() override;

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
			float AbilityDuration;

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
			float AbilitySpeed;
};
