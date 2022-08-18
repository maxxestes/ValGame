// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "SageWallComp.generated.h"

/**
 * 
 */
UCLASS()
class VALGAME_API USageWallComp : public UStaticMeshComponent
{
	GENERATED_BODY()

public:

		USageWallComp();

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Properties")
		float Health;

		void TakeDamage(float damage);

		float getHealth();

		void setHealth(float damage);

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Properties")
		UStaticMesh* wallMesh;

	
};
