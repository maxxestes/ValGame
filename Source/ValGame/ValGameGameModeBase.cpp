// Copyright Epic Games, Inc. All Rights Reserved.


#include "ValGameGameModeBase.h"
#include "PlayerCharacter.h"

AValGameGameModeBase::AValGameGameModeBase()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/PlayerCharacter_BP"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
	else {
		DefaultPawnClass = APlayerCharacter::StaticClass();
	}
	
	// etc.
}