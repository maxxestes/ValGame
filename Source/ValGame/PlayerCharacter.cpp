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
	HoldingSmoke = false;
	weaponIndex = 0;
	numHeldMoveKeys = 0;
	LastFrameLookAt = FirstPersonCameraComponent->GetForwardVector();
	numAbilityOneCharges = 1;
	numAbilityTwoCharges = 2;





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
		UpdateWallPos();
	}

	if (HoldingSmoke) {
		UpdateSmokePos();
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

	PlayerInputComponent->BindAction("ThrowSmoke", IE_Pressed, this, &APlayerCharacter::ThrowSmoke);
	PlayerInputComponent->BindAction("ThrowSmoke", IE_Released, this, &APlayerCharacter::LetGoSmoke);

	PlayerInputComponent->BindAction("Updraft", IE_Pressed, this, &APlayerCharacter::UseUpdraft);
	PlayerInputComponent->BindAction("Updraft", IE_Released, this, &APlayerCharacter::LetGoUpdraft);

	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &APlayerCharacter::Reload);

	PlayerInputComponent->BindAction("SwitchWeaponPrimary", IE_Pressed, this, &APlayerCharacter::ChangeToPrimary);
	PlayerInputComponent->BindAction("SwitchWeaponSecondary", IE_Pressed, this, &APlayerCharacter::ChangeToSecondary);
	PlayerInputComponent->BindAction("SwitchWeaponKnife", IE_Pressed, this, &APlayerCharacter::ChangeToKnife);

	PlayerInputComponent->BindAction("Walk", IE_Pressed, this, &APlayerCharacter::Walk);
	PlayerInputComponent->BindAction("Walk", IE_Released, this, &APlayerCharacter::OnWalkStop);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &APlayerCharacter::Crouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &APlayerCharacter::OnCrouchStop);

	PlayerInputComponent->BindAction("Dash", IE_Pressed, this, &APlayerCharacter::Dash);

	PlayerInputComponent->BindAction("ScrollNextWeapon", IE_Pressed, this, &APlayerCharacter::ScrollToNextWeapon);
	PlayerInputComponent->BindAction("ScrollPrevWeapon", IE_Pressed, this, &APlayerCharacter::ScrollToPrevWeapon);


	PlayerInputComponent->BindAction("Wall", IE_Pressed, this, &APlayerCharacter::HoldWall);
}

void APlayerCharacter::OnFire()
{
	// try and fire a projectile

	if (_CurrentWeapon && !GetWorld()->GetTimerManager().IsTimerActive(ReloadTimerHandle))
	{
		UWorld* const World = GetWorld();
		if (World != nullptr)
		{
			FHitResult Hit;
			FHitResult HitWall;
			FHitResult HitAnything;
			TArray<FHitResult> hitResultArray;
			FVector NextShotVector = FirstPersonCameraComponent->GetForwardVector();
			FVector Start = FirstPersonCameraComponent->GetComponentLocation() + (NextShotVector * 10);


			NextShotVector = CalculateNextVector(NextShotVector);

			FVector End = ((NextShotVector * 3000) + Start);
			FCollisionQueryParams CollisionParams;
			CollisionParams.bTraceComplex = true;
			CollisionParams.AddIgnoredActor(this);


			bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_GameTraceChannel1, CollisionParams);

			bool bHitWall = GetWorld()->LineTraceSingleByChannel(HitWall, Start, End, ECC_GameTraceChannel2, CollisionParams);

			bool bHitAnything = GetWorld()->LineTraceSingleByChannel(HitAnything, Start, End, ECC_Camera, CollisionParams);
			

			if (bHitAnything) {
				DrawDebugBox(GetWorld(), HitAnything.ImpactPoint, FVector(5, 5, 5), FColor::Emerald, false, 2.0f);
			}

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

			uint8 currentMag = _CurrentWeapon->ManageAmmoAfterShot(1);

			if (currentMag <= 0) {
				//GEngine->AddOnScreenDebugMessage(1, 3, FColor::White, "wtf");
				Reload();
			}
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
	/*
	* Checks to see if the player have wall charges remaining before holding.
	*/
	if (_AbilityOne && _AbilityOne->NumberOfCharges <= 0) {
		return;
	}

	/*
	* If we press the wall button again while holding, we rotate the wall 90 degrees.
	*/
	if (HoldingWall) {
		if (_AbilityOne->rot == false) {
			_AbilityOne->rot = true;
		}
		else {
			_AbilityOne->rot = false;
		}
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
	_AbilityOne = GetWorld()->SpawnActor<ASageWall_Ability>(SageWall_BP);

	/*
	* If our line trace collides with visibility channel before 800 units, the wall is placed
	* at the collision. If not, the wall is placed at 800 units.
	*/
	if (bHit && FVector::Dist2D(Start, Hit.ImpactPoint) < 800) {	
		End = Hit.ImpactPoint;
	}
	else {
		End = ((NextShotVector * 800) + Start);
	}


	FVector DownVec = End;
	DownVec.Z -= 3000;
	bool downHit = GetWorld()->LineTraceSingleByChannel(Hit, End, DownVec, ECC_Visibility, CollisionParams);

	/*
	* Sets the location of the wall to the intersection of the previous vector end point
	* and a very long vector in the -z direction. This is to find the floor below the impact point.
	*/
	if (downHit) {
		_AbilityOne->SetActorLocation(Hit.ImpactPoint, false, 0, ETeleportType::None);
	}
	else {
		DownVec = Start;
		DownVec.Z -= 3000;
		GetWorld()->LineTraceSingleByChannel(Hit, Start, DownVec, ECC_Visibility, CollisionParams);
		_AbilityOne->SetActorLocation(Hit.ImpactPoint, false, 0, ETeleportType::None);
	}
	

	FRotator playerLook = FirstPersonCameraComponent->GetForwardVector().Rotation();
	playerLook.Pitch = 0.f;


	_AbilityOne->SetActorRotation(playerLook);


	_AbilityOne->AddActorLocalRotation(FRotator(0.f, 90.f, 0.f));
}

void APlayerCharacter::Dash()
{

	if (JettDash_BP) {
		if (!_AbilityFour) {
			_AbilityFour = GetWorld()->SpawnActor<AJettDash_Ability>(JettDash_BP);
		}
		if (_AbilityFour->NumberOfCharges > 0) {
			FString TheFloatStr = FString::SanitizeFloat(_AbilityFour->AbilityForce);

			GEngine->AddOnScreenDebugMessage(1, 3, FColor::White, TheFloatStr);

			LaunchCharacter(GetActorForwardVector() * _AbilityFour->AbilityForce, true, true);
			_AbilityFour->NumberOfCharges -= 1;
			_AmmoWidget->updateDashCount(_AbilityFour->NumberOfCharges);
		}
	}

	
}

void APlayerCharacter::UseUpdraft()
{
	if (JettUpdraft_BP) {
		if (!_AbilityThree) {
			_AbilityThree = GetWorld()->SpawnActor<AJettUpdrafting_Ability>(JettUpdraft_BP);
		}
		if (_AbilityThree->NumberOfCharges > 0) {
			if (GetMovementComponent()->IsFalling()) {
				this->JumpMaxCount += 1;
			}
			GetCharacterMovement()->JumpZVelocity = _AbilityThree->AbilitySpeed;
			Jump();
			_AbilityThree->NumberOfCharges -= 1;
			_AmmoWidget->updateUpdraftCount(_AbilityThree->NumberOfCharges);
		}
	}
}

void APlayerCharacter::LetGoUpdraft()
{
	GetCharacterMovement()->JumpZVelocity = 420;
	this->JumpMaxCount = 1;
}

void APlayerCharacter::ThrowSmoke()
{
	if (numAbilityTwoCharges > 0) {
		_CurrentWeapon = nullptr;
		_AbilityTwo = GetWorld()->SpawnActor<AJettSmoke_Ability>(JettSmoke_BP);
		FVector InFront = FirstPersonCameraComponent->GetForwardVector();
		FVector Start = FirstPersonCameraComponent->GetComponentLocation();
		FVector End = ((InFront * 150) + Start);
		_AbilityTwo->SetActorLocation(End, false, 0, ETeleportType::None);
		this->_AbilityTwo->currDir = (InFront * 30000) + Start;
		HoldingSmoke = true;
	}
}

void APlayerCharacter::LetGoSmoke()
{
	if (numAbilityTwoCharges > 0) {
		HoldingSmoke = false;
		_CurrentWeapon = _Primary;
		numAbilityTwoCharges -= 1;
		_AmmoWidget->updateSmokeCount(numAbilityTwoCharges);
		if (!this->_AbilityTwo->isExpanded) {
			this->_AbilityTwo->SmokeBall->SetSimulatePhysics(true);
			this->_AbilityTwo->SmokeBall->SetEnableGravity(true);
		}
	}
}


void APlayerCharacter::UpdateSmokePos()
{
	if (!this->_AbilityTwo->isExpanded) {
		FVector NextShotVector = FirstPersonCameraComponent->GetForwardVector();
		FVector Start = FirstPersonCameraComponent->GetComponentLocation();



		FVector End = ((NextShotVector * 30000) + Start);


		FVector newEnd = (UKismetMathLibrary::VInterpTo(this->_AbilityTwo->currDir, End, GetWorld()->DeltaTimeSeconds, 5.0f));
		this->_AbilityTwo->currDir = newEnd;
		FVector dirGoing = newEnd - this->_AbilityTwo->GetActorLocation();
		if (dirGoing.Normalize()) {
			dirGoing *= _AbilityTwo->MissleSpeed;
			this->_AbilityTwo->SetActorLocation(this->_AbilityTwo->GetActorLocation() + dirGoing);
		}
	}
	else {
		HoldingSmoke = false;
	}
	
}



void APlayerCharacter::PutWallAway()
{
	HoldingWall = false;
	_AbilityOne->Destroy();
	_AbilityOne = nullptr;
	_CurrentWeapon = _Primary;
}



void APlayerCharacter::UpdateWallPos()
{
	FHitResult Hit;
	FVector NextShotVector = FirstPersonCameraComponent->GetForwardVector();
	FVector Start = FirstPersonCameraComponent->GetComponentLocation();
	FVector End = ((NextShotVector * 10000) + Start);
	FCollisionQueryParams CollisionParams;
	CollisionParams.bTraceComplex = true;
	CollisionParams.AddIgnoredActor(this);
	bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, CollisionParams);

	if (bHit) {
		float dist = FVector::Dist2D(Start, Hit.ImpactPoint);
		if (dist > 800) {
			End = ((NextShotVector * 800) + Start);
		}
		else {
			End = Hit.ImpactPoint;
		}
	}
	else {
		End = ((NextShotVector * 800) + Start);
	}

	FVector DownVec = End;
	DownVec.Z -= 3000;
	bool downHit = GetWorld()->LineTraceSingleByChannel(Hit, End, DownVec, ECC_Visibility, CollisionParams);

	if (downHit) {
		_AbilityOne->SetActorLocation(Hit.ImpactPoint, false, 0, ETeleportType::None);
	}
	else {
		DownVec = Start;
		DownVec.Z -= 3000;
		downHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, DownVec, ECC_Visibility, CollisionParams);
		_AbilityOne->SetActorLocation(Hit.ImpactPoint, false, 0, ETeleportType::None);
	}


	FRotator playerLook = FirstPersonCameraComponent->GetForwardVector().Rotation();
	playerLook.Pitch = 0.f;
	_AbilityOne->SetActorRotation(playerLook);

		if (!_AbilityOne->rot) {
			_AbilityOne->AddActorLocalRotation(FRotator(0.f, 90.f, 0.f));
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
		GetWorld()->GetTimerManager().SetTimer(ReloadTimerHandle, ReloadDeligate, _CurrentWeapon->reloadTime, false);
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
		_AmmoWidget->updateGunDisplay(_CurrentWeapon->GetName());
		if (GetWorld()->GetTimerManager().IsTimerActive(ReloadTimerHandle)) {
			GetWorld()->GetTimerManager().ClearTimer(ReloadTimerHandle);	
		}
		if (_CurrentWeapon->currentMagAmmo <= 0) {
			GetWorld()->GetTimerManager().SetTimer(ReloadTimerHandle, ReloadDeligate, _CurrentWeapon->reloadTime, false);
		}
		ReloadDeligate.BindUFunction(_CurrentWeapon, FName("Reload"), _AmmoWidget);
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
		//FString TheFloatStr = FString::SanitizeFloat(Value);

		//GEngine->AddOnScreenDebugMessage(1, 3, FColor::White, TheFloatStr);

		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
		
	}
}


