// Copyright Epic Games, Inc. All Rights Reserved.


#include "ValGameGameModeBase.h"
#include "PlayerCharacter.h"

AValGameGameModeBase::AValGameGameModeBase()
{
	DefaultPawnClass = APlayerCharacter::StaticClass();
	// etc.
}