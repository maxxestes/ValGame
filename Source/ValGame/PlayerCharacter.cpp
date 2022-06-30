// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "DrawDebugHelpers.h"
#include "Math/UnrealMathUtility.h"


// Sets default values
APlayerCharacter::APlayerCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	CurrentVertRecoil = 0.f;
	CurrentHorRecoil = 0.f;
	isRunning = false;
	weaponIndex = 0;
	numHeldMoveKeys = 0;

}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	if (this->GetVelocity().Size() == 0) {
		isRunning = false;
		GEngine->AddOnScreenDebugMessage(18, 3, FColor::White, "Not running");
	}
	else {
		isRunning = true;
		GEngine->AddOnScreenDebugMessage(18, 3, FColor::White, "Running");
	}
}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &APlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APlayerCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &APlayerCharacter::OnFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &APlayerCharacter::OnFireStop);

	//Previously used to check to see if player is moving for gun accuracy. Now checked in tick.
	PlayerInputComponent->BindAction("Moving", IE_Pressed, this, &APlayerCharacter::OnMove);
	PlayerInputComponent->BindAction("Moving", IE_Released, this, &APlayerCharacter::OnMoveStop);

	PlayerInputComponent->BindAction("PrintInventory", IE_Pressed, this, &APlayerCharacter::PrintInventory);

	PlayerInputComponent->BindAction("SwitchWeaponPrimary", IE_Pressed, this, &APlayerCharacter::ChangeToPrimary);
	PlayerInputComponent->BindAction("SwitchWeaponSecondary", IE_Pressed, this, &APlayerCharacter::ChangeToSecondary);
	PlayerInputComponent->BindAction("SwitchWeaponKnife", IE_Pressed, this, &APlayerCharacter::ChangeToKnife);

	PlayerInputComponent->BindAction("ScrollNextWeapon", IE_Pressed, this, &APlayerCharacter::ScrollToNextWeapon);
	PlayerInputComponent->BindAction("ScrollPrevWeapon", IE_Pressed, this, &APlayerCharacter::ScrollToPrevWeapon);

}

void APlayerCharacter::OnFire()
{
	// try and fire a projectile
	if (_CurrentWeapon)
	{
		UWorld* const World = GetWorld();
		if (World != nullptr)
		{
			FVector NextShotVector = FirstPersonCameraComponent->GetForwardVector();
			FVector Start = FirstPersonCameraComponent->GetComponentLocation();

			NextShotVector = CalculateNextVector(NextShotVector);

			FVector End = ((NextShotVector * 500) + Start);
			FCollisionQueryParams CollisionParams;
			CollisionParams.bTraceComplex = true;
			CollisionParams.AddIgnoredActor(this);


			DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 5.f, 0, 5);

			CurrentVertRecoil += _CurrentWeapon->VerticleRecoilMultiplier;
			
			if (CurrentVertRecoil > _CurrentWeapon->maxRecoilMultiplier) {
				CurrentVertRecoil = _CurrentWeapon->maxRecoilMultiplier;
			}

			CurrentHorRecoil += FMath::RandRange(-_CurrentWeapon->HorizontalRecoilMultiplier, _CurrentWeapon->HorizontalRecoilMultiplier);
			CurrentHorRecoil = FMath::Clamp(CurrentHorRecoil, -_CurrentWeapon->maxRecoilMultiplier, _CurrentWeapon->maxRecoilMultiplier);


			GetWorld()->GetTimerManager().SetTimer(FireTimerHandle, this, &APlayerCharacter::OnFire, .2f, false);

		}
	}
}

void APlayerCharacter::OnFireStop()
{
	GetWorld()->GetTimerManager().ClearTimer(FireTimerHandle);
	CurrentVertRecoil = 0.f;
	CurrentHorRecoil = 0.f;
}

void APlayerCharacter::OnMove()
{
	//isRunning = true;
	//numHeldMoveKeys++;
	//GEngine->AddOnScreenDebugMessage(1, 3, FColor::White, "Running");
}

void APlayerCharacter::OnMoveStop()
{
	//numHeldMoveKeys--;
	//if (numHeldMoveKeys == 0) {
	//	isRunning = false;
	//	GEngine->AddOnScreenDebugMessage(1, 3, FColor::White, "NotRunning");
	//}
}

void APlayerCharacter::ChangeToPrimary()
{
	if (_Primary) {
		EquipWeapon(_Primary);
	}
}
void APlayerCharacter::ChangeToSecondary()
{
	if (_Secondary) {
		EquipWeapon(_Secondary);
	}
}
void APlayerCharacter::ChangeToKnife()
{
	EquipWeapon(_Knife);
}

void APlayerCharacter::ScrollToNextWeapon()
{
	if (_CurrentWeapon) {
		if (_Primary && _CurrentWeapon == _Primary && _Secondary) {
			EquipWeapon(_Secondary);
		}
		else if (_Secondary && _CurrentWeapon == _Secondary) {
			EquipWeapon(_Knife);
		}
		else {
			//Can't switch past knife. Not a loop
		}
	}
}

void APlayerCharacter::ScrollToPrevWeapon()
{
	if (_CurrentWeapon) {
		if (_Knife && _CurrentWeapon == _Knife && _Secondary) {
			EquipWeapon(_Secondary);
		}
		else if (_Primary && _Secondary && _CurrentWeapon == _Secondary) {
			EquipWeapon(_Primary);
		}
		else {
			//Can't switch past Primary. Not a loop
		}
	}
}

void APlayerCharacter::PickUpGun(AGun* newGun)
{
	if (newGun) {
		if (newGun->type == GunType::Primary) {
			_Primary = newGun;
		}
		else if (newGun->type == GunType::Secondary) {
			_Secondary = newGun;
		}
		else 
		{
			//Its a knife which isn't possible to pick up or drop
			//for that matter but eh
		}
		EquipWeapon(newGun);
	}

	


	
}

void APlayerCharacter::EquipWeapon(AGun* newCurrentWeapon) {
	/*
	if (_equippedWeapons.Num() - 1 >= indexOfWeapon) {
		_CurrentWeapon = _equippedWeapons[indexOfWeapon];
	}
	*/
	if (newCurrentWeapon) {
		_CurrentWeapon = newCurrentWeapon;
	}
}

void APlayerCharacter::PrintInventory()
{
	FString sInventory = "";

	if (_Primary) {
		_Primary->AppendName(sInventory);
		sInventory.Append(" | ");
	}
	if (_Secondary) {
		_Secondary->AppendName(sInventory);
		sInventory.Append(" | ");
	}
	if (_Knife) {
		_Knife->AppendName(sInventory);
		sInventory.Append(" | ");
	}
		
	
	FString currentWeapon = "";

	if (_CurrentWeapon) {
		_CurrentWeapon->AppendName(currentWeapon);
	}
	



	GEngine->AddOnScreenDebugMessage(1, 3, FColor::White, *sInventory);
	GEngine->AddOnScreenDebugMessage(6, 3, FColor::Red, *currentWeapon);
}


FVector APlayerCharacter::CalculateNextVector(const FVector& CurrentLookAtVector)
{
	FRotator Rot = CurrentLookAtVector.Rotation();
	if (isRunning) {
		float maxRecoil = _CurrentWeapon->maxRecoilMultiplier;

		float vertRecoil = FMath::RandRange(-maxRecoil, maxRecoil);
		vertRecoil += CurrentVertRecoil;
		vertRecoil = FMath::Clamp(vertRecoil, -maxRecoil, maxRecoil);

		float horRecoil = FMath::RandRange(-maxRecoil, maxRecoil);
		horRecoil += CurrentHorRecoil;
		horRecoil = FMath::Clamp(horRecoil, -maxRecoil, maxRecoil);

		//FRotator newShotRotation = FRotator(vertRecoil - 2.65f, horRecoil + .75f, 0);
		Rot.Pitch += vertRecoil -2.65f;
		Rot.Yaw += horRecoil + .75f;
		return Rot.Vector();
	}
	else {
		Rot.Pitch += (CurrentVertRecoil - 2.65f);
		Rot.Yaw += (CurrentHorRecoil + .75f);
		return Rot.Vector();
	}
	//return CurrentLookAtVector;
}


void APlayerCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void APlayerCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}


