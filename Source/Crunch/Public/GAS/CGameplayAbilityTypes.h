// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CGameplayAbilityTypes.generated.h"

UENUM(BlueprintType)
enum class ECAbilityInputID : uint8
{
	None				UMETA(DisplayName = "None"),
	BasicAttack			UMETA(DisplayName = "Basic Attack"),
	FirstAbility		UMETA(DisplayName = "First Ability"),
	SecondAbility		UMETA(DisplayName = "Second Ability"),
	ThirdAbility		UMETA(DisplayName = "Third Ability"),
	FourthAbility		UMETA(DisplayName = "Fourth Ability"),
	FifthAbility		UMETA(DisplayName = "Fifth Ability"),
	SixthAbility		UMETA(DisplayName = "Sixth Ability"),
	Confirm				UMETA(DisplayName = "Confirm"),
	Cancel				UMETA(DisplayName = "Cancel"),
};
