// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Gun.h"
#include "DoorSwitchActor.h"
#include "SageWall_Ability.h"
#include "PlayerCharacter.generated.h"


class UInputComponent;
class USkeletalMeshComponent;
class USceneComponent;
class UCameraComponent;
class UAnimMontage;
class USoundBase;

//DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUseItem);

UCLASS()
class VALGAME_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		UCameraComponent* FirstPersonCameraComponent;

	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);

	/** Fires a projectile. */
	void OnFire();

	void OnFireStop();

	void ChangeToPrimary();

	void ChangeToSecondary();

	void ChangeToKnife();

	void ScrollToNextWeapon();
	
	void ScrollToPrevWeapon();

	void Walk();

	void OnWalkStop();

	void Crouch();

	void OnCrouchStop();

	void UseWall();

	FVector CalculateNextVector(const FVector& CurrentLookAtVector);

	FTimerHandle FireTimerHandle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Aiming")
		float CurrentVertRecoil;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Aiming")
		float CurrentHorRecoil;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
		float isRunning;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
		float isWalking;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon Management")
		uint8 weaponIndex;

	uint8 numHeldMoveKeys;

	FVector LastFrameLookAt;

	bool lookingRight;

	bool lookingLeft;
		

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void PickUpGun(AGun* newGun);

	void EquipWeapon(AGun* newCurrentWeapon);

	UFUNCTION(BlueprintCallable)
	void PrintInventory();

	void PressDoor();

	void SwitchProximityEnter(ADoorSwitchActor* newSwitch);

	void SwitchProximityExit();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		TSubclassOf<ASageWall_Ability> SageWall_BP;



private:

	AGun* _Primary;

	AGun* _Secondary;

	AGun* _Knife;

	AGun* _CurrentWeapon;

	ADoorSwitchActor* _currentSwitch;

	ASageWall_Ability* _AbilityOne;




};
