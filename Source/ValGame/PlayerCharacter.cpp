// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SageWall_Ability.h"
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
	isWalking = false;
	HoldingWall = false;
	weaponIndex = 0;
	numHeldMoveKeys = 0;
	LastFrameLookAt = FirstPersonCameraComponent->GetForwardVector();






}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	//ammoWidget->NativeConstruct();
	_AmmoWidget = CreateWidget<UPlayerWidget>(GetWorld(), PlayerWidget_BP);
	if (_AmmoWidget) {
		_AmmoWidget->AddToViewport();
	}

}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float changeInYaw = FirstPersonCameraComponent->GetForwardVector().Rotation().Yaw - LastFrameLookAt.Rotation().Yaw;

	if (changeInYaw > 0) {
		lookingRight = true;
		lookingLeft = false;
	}
	else if (changeInYaw < 0) {
		lookingLeft = true;
		lookingRight = false;
	}
	else {
		lookingLeft = false;
		lookingRight = false;
	}

	if (this->GetVelocity().Size() == 0) {
		isRunning = false;
		GEngine->AddOnScreenDebugMessage(18, 3, FColor::White, "Not running");
	}
	else {
		isRunning = true;
		GEngine->AddOnScreenDebugMessage(18, 3, FColor::White, "Running");
	}

	LastFrameLookAt = FirstPersonCameraComponent->GetForwardVector();
	if (HoldingWall) {
		UpdateWallPos(changeInYaw);
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

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("PrintInventory", IE_Pressed, this, &APlayerCharacter::PrintInventory);

	PlayerInputComponent->BindAction("PressDoor", IE_Pressed, this, &APlayerCharacter::PressDoor);

	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &APlayerCharacter::Reload);

	PlayerInputComponent->BindAction("SwitchWeaponPrimary", IE_Pressed, this, &APlayerCharacter::ChangeToPrimary);
	PlayerInputComponent->BindAction("SwitchWeaponSecondary", IE_Pressed, this, &APlayerCharacter::ChangeToSecondary);
	PlayerInputComponent->BindAction("SwitchWeaponKnife", IE_Pressed, this, &APlayerCharacter::ChangeToKnife);

	PlayerInputComponent->BindAction("Walk", IE_Pressed, this, &APlayerCharacter::Walk);
	PlayerInputComponent->BindAction("Walk", IE_Released, this, &APlayerCharacter::OnWalkStop);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &APlayerCharacter::Crouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &APlayerCharacter::OnCrouchStop);

	PlayerInputComponent->BindAction("ScrollNextWeapon", IE_Pressed, this, &APlayerCharacter::ScrollToNextWeapon);
	PlayerInputComponent->BindAction("ScrollPrevWeapon", IE_Pressed, this, &APlayerCharacter::ScrollToPrevWeapon);


	PlayerInputComponent->BindAction("Wall", IE_Pressed, this, &APlayerCharacter::HoldWall);
}

void APlayerCharacter::OnFire()
{
	// try and fire a projectile

	if (_CurrentWeapon && _CurrentWeapon->currentMagAmmo > 0)
	{
		UWorld* const World = GetWorld();
		if (World != nullptr)
		{
			FHitResult Hit;
			FHitResult HitWall;
			TArray<FHitResult> hitResultArray;
			FVector NextShotVector = FirstPersonCameraComponent->GetForwardVector();
			FVector Start = FirstPersonCameraComponent->GetComponentLocation();


			NextShotVector = CalculateNextVector(NextShotVector);

			FVector End = ((NextShotVector * 3000) + Start);
			FCollisionQueryParams CollisionParams;
			CollisionParams.bTraceComplex = true;
			CollisionParams.AddIgnoredActor(this);

			FCollisionQueryParams traceParams;

			FCollisionObjectQueryParams objectParams;
			//objectParams.ObjectTypesToQuery = ABreakableDoor;

			bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_GameTraceChannel1, traceParams);

			bool bHitWall = GetWorld()->LineTraceSingleByChannel(HitWall, Start, End, ECC_GameTraceChannel2, traceParams);

			
			
			//GetWorld()->LineTraceMultiByObjectType(hitResultArray, Start, End)

			DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 5.f, 0, 5);
			

			if (bHit) {
				DrawDebugBox(GetWorld(), Hit.ImpactPoint, FVector(5, 5, 5), FColor::Emerald, false, 2.0f);
					ABreakableDoor* doorHit = (ABreakableDoor*)Hit.GetActor();
				if (doorHit) {
					doorHit->setHealth(doorHit->getHealth() - _CurrentWeapon->baseDamage);
				}
			}

			if (bHitWall) {
				DrawDebugBox(GetWorld(), HitWall.ImpactPoint, FVector(5, 5, 5), FColor::Emerald, false, 2.0f);
				USageWallComp* wallHit = (USageWallComp*)HitWall.GetComponent();
				if (wallHit) {
					wallHit->setHealth(wallHit->getHealth() - _CurrentWeapon->baseDamage);
				}
			}

			CurrentVertRecoil += _CurrentWeapon->VerticleRecoilMultiplier;
			
			if (CurrentVertRecoil > _CurrentWeapon->maxRecoilMultiplier) {
				CurrentVertRecoil = _CurrentWeapon->maxRecoilMultiplier;
			}

			CurrentHorRecoil += FMath::RandRange(-_CurrentWeapon->HorizontalRecoilMultiplier, _CurrentWeapon->HorizontalRecoilMultiplier);
			CurrentHorRecoil = FMath::Clamp(CurrentHorRecoil, -_CurrentWeapon->maxRecoilMultiplier, _CurrentWeapon->maxRecoilMultiplier);

			if (_CurrentWeapon->GunType == FireType::Auto) {
				GetWorld()->GetTimerManager().SetTimer(FireTimerHandle, this, &APlayerCharacter::OnFire, _CurrentWeapon->fireRate, false);
			}

			_CurrentWeapon->ManageAmmoAfterShot(1);

			_AmmoWidget->updateAmmoCount(_CurrentWeapon->currentMagAmmo, _CurrentWeapon->reserveAmmo);
			

		}
	}
}

void APlayerCharacter::OnFireStop()
{
	if (_CurrentWeapon) {
		GetWorld()->GetTimerManager().ClearTimer(FireTimerHandle);
		CurrentVertRecoil = 0.f;
		CurrentHorRecoil = 0.f;
	}
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
	else if (HoldingWall) {
		PutWallAway();
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
	else if (HoldingWall) {
		PutWallAway();
	}
}

void APlayerCharacter::Walk()
{
	isWalking = true;
}

void APlayerCharacter::OnWalkStop()
{
	isWalking = false;
}

void APlayerCharacter::Crouch()
{
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 32.f)); // Position the camera
}

void APlayerCharacter::OnCrouchStop()
{
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
}

void APlayerCharacter::UseWall()
{
	if (_AbilityOne) {
		_AbilityOne->PlaceWall();
		HoldingWall = false;
		_CurrentWeapon = _Primary;
		_AbilityOne->NumberOfCharges = 0;
		_AmmoWidget->updateWallCount(_AbilityOne->NumberOfCharges);
	}
}

void APlayerCharacter::HoldWall()
{
	if (HoldingWall) {
		if (_AbilityOne->rot == false) {
			_AbilityOne->rot = true;
		}
		else {
			_AbilityOne->rot = false;
		}
		return;
	}
	if (_AbilityOne && _AbilityOne->NumberOfCharges <= 0) {
		return;
	}

	
		HoldingWall = true;
		_CurrentWeapon = nullptr;
		this->InputComponent->BindAction("Fire", IE_Pressed, this, &APlayerCharacter::UseWall);
		FHitResult Hit;
		FVector NextShotVector = FirstPersonCameraComponent->GetForwardVector();
		FVector Start = FirstPersonCameraComponent->GetComponentLocation();



		FVector End = ((NextShotVector * 10000) + Start);
		FCollisionQueryParams CollisionParams;
		CollisionParams.bTraceComplex = true;
		CollisionParams.AddIgnoredActor(this);


		bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, CollisionParams);


		if (bHit) {
			_AbilityOne = GetWorld()->SpawnActor<ASageWall_Ability>(SageWall_BP);
			float dist = FVector::Dist2D(Start, Hit.ImpactPoint);
			if (dist > 800) {
				Hit.ImpactPoint = ((NextShotVector * 800) + Start);
			}
			FVector DownVec = Hit.ImpactPoint;
			DownVec.Z -= 3000;
			GetWorld()->LineTraceSingleByChannel(Hit, Hit.ImpactPoint, DownVec, ECC_Visibility, CollisionParams);
			_AbilityOne->SetActorLocation(Hit.ImpactPoint, false, 0, ETeleportType::None);
		
			FRotator wallRot = FRotator(0.f, 90.f, 0.f);

			FRotator playerLook = FirstPersonCameraComponent->GetForwardVector().Rotation();
			playerLook.Pitch = 0.f;


			_AbilityOne->SetActorRotation(playerLook);


			_AbilityOne->AddActorLocalRotation(wallRot);
	
		

	}
}

void APlayerCharacter::PutWallAway()
{
	HoldingWall = false;
	_AbilityOne->Destroy();
	_AbilityOne = nullptr;
	_CurrentWeapon = _Primary;
}

void APlayerCharacter::UpdateWallPos(float rotation)
{
	FHitResult Hit;
	FVector NextShotVector = FirstPersonCameraComponent->GetForwardVector();
	FVector Start = FirstPersonCameraComponent->GetComponentLocation();



	FVector End = ((NextShotVector * 10000) + Start);
	FCollisionQueryParams CollisionParams;
	CollisionParams.bTraceComplex = true;
	CollisionParams.AddIgnoredActor(this);
	CollisionParams.AddIgnoredActor(_AbilityOne);


	bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, CollisionParams);

	//DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 5.f, 0, 5);


	if (bHit) {
		//DrawDebugBox(GetWorld(), Hit.ImpactPoint, FVector(5, 5, 5), FColor::Emerald, false, 2.0f);
		float dist = FVector::Dist2D(Start, Hit.ImpactPoint);
		if (dist > 800) {
			Hit.ImpactPoint = ((NextShotVector * 800) + Start);
		}
		FVector DownVec = Hit.ImpactPoint;
		DownVec.Z -= 3000;
		//DrawDebugBox(GetWorld(), Hit.ImpactPoint, FVector(5, 5, 5), FColor::Emerald, false, 2.0f);
		//DrawDebugLine(GetWorld(), Hit.ImpactPoint, DownVec, FColor::Red, false, 5.f, 0, 5);
		GetWorld()->LineTraceSingleByChannel(Hit, Hit.ImpactPoint, DownVec, ECC_Visibility, CollisionParams);
		_AbilityOne->SetActorLocation(Hit.ImpactPoint, false, 0, ETeleportType::None);


		FRotator wallRot = FRotator(0.f, 90.f, 0.f);

		FRotator playerLook = FirstPersonCameraComponent->GetForwardVector().Rotation();
		playerLook.Pitch = 0.f;


		_AbilityOne->SetActorRotation(playerLook);

		if (!_AbilityOne->rot) {
			_AbilityOne->AddActorLocalRotation(wallRot);
		}
		


	}
}


void APlayerCharacter::PickUpGun(AGun* newGun)
{
	if (newGun) {
		if (newGun->GunType == GunType::Primary) {
			_Primary = newGun;
		}
		else if (newGun->GunType == GunType::Secondary) {
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

void APlayerCharacter::PressDoor() {
	if (_currentSwitch) {
		_currentSwitch->_LinkedDoor->RaiseDoor();
		GEngine->AddOnScreenDebugMessage(1, 3, FColor::White, _currentSwitch->GetName());
	}
}

void APlayerCharacter::Reload()
{
	if (_CurrentWeapon) {
		_CurrentWeapon->Reload();
		_AmmoWidget->updateAmmoCount(_CurrentWeapon->currentMagAmmo, _CurrentWeapon->reserveAmmo);
	}
}

void APlayerCharacter::SwitchProximityEnter(ADoorSwitchActor* newSwitch)
{
	if (newSwitch) {
		_currentSwitch = newSwitch;
	}
}

void APlayerCharacter::SwitchProximityExit()
{
	_currentSwitch = nullptr;
}

void APlayerCharacter::EquipWeapon(AGun* newCurrentWeapon) {
	if (newCurrentWeapon) {
		_CurrentWeapon = newCurrentWeapon;
		_AmmoWidget->updateAmmoCount(_CurrentWeapon->currentMagAmmo, _CurrentWeapon->reserveAmmo);
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
		currentWeapon.Append(" | ");
		currentWeapon.Append(FString::FromInt(_CurrentWeapon->currentMagAmmo));
		currentWeapon.Append(" | ");
		currentWeapon.Append(FString::FromInt(_CurrentWeapon->reserveAmmo));
	}
	



	GEngine->AddOnScreenDebugMessage(1, 3, FColor::White, *sInventory);
	GEngine->AddOnScreenDebugMessage(6, 3, FColor::Red, *currentWeapon);
}


FVector APlayerCharacter::CalculateNextVector(const FVector& CurrentLookAtVector)
{
	FRotator Rot = CurrentLookAtVector.Rotation();
	if (isRunning) {
		float maxRecoil = _CurrentWeapon->maxRecoilMultiplier;
		float maxAdditionalRecoil;
		if (isWalking) {
			maxAdditionalRecoil = _CurrentWeapon->WalkAimAccuracy;
		}
		else {
			maxAdditionalRecoil = _CurrentWeapon->maxRecoilMultiplier;
		}
		float vertRecoil = FMath::RandRange(-maxAdditionalRecoil, maxAdditionalRecoil);
		vertRecoil += CurrentVertRecoil;
		vertRecoil = FMath::Clamp(vertRecoil, -maxRecoil, maxRecoil);

		float horRecoil = FMath::RandRange(-maxAdditionalRecoil, maxAdditionalRecoil);
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
		if (isWalking) {
			Value = Value / 2;
		}

		

		



		
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void APlayerCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		if (isWalking) {
			Value = Value / 2;
		}
		UCharacterMovementComponent* movComp = this->GetCharacterMovement();

		if (movComp->IsFalling()) {

			if (lookingRight && Value > 0) {
				movComp->AirControl = 1;
			}
			else if (lookingLeft && Value < 0) {
				movComp->AirControl = 1;
			}
			else {
				movComp->AirControl = 0;
			}
		}

		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}


