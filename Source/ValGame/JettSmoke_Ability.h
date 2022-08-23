// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterAbility.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/BoxComponent.h"
#include "JettSmoke_Ability.generated.h"

/**
 * 
 */
UCLASS()
class VALGAME_API AJettSmoke_Ability : public ACharacterAbility
{
	GENERATED_BODY()
	
	public:
		AJettSmoke_Ability();

		virtual void BeginPlay() override;

		//virtual void TickComponent(float DeltaTime, ELevelTick tickType, FActorComponentTickFunction* ThisTickFunction) override;

		void Kill();

		void Expand();

		bool isExpanded;

		float SmokeLifetime;

		float maximumTravelTime;

		FTimerHandle LifetimeExpandedHandle;

		FTimerHandle MaximumTravelHandle;

		FVector currDir;

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
			float MissleSpeed;

		UFUNCTION()
			void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp,
				int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);


		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Properties")
			UStaticMeshComponent* SmokeBall;

		UPROPERTY(EditAnywhere)
			UBoxComponent* Hitbox;


	private:

		UPROPERTY(EditAnywhere)
			USceneComponent* SceneComponent;




};
