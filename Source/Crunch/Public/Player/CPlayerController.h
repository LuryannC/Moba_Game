// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CPlayerController.generated.h"

class ACPlayerCharacter;
/**
 * 
 */
UCLASS()
class CRUNCH_API ACPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	// Only called on the server
	virtual void OnPossess(APawn* InPawn) override;
	// only called on the client
	virtual void AcknowledgePossession(class APawn* P) override;

private:
	UPROPERTY()
	TObjectPtr<ACPlayerCharacter> CPlayerCharacter;
};
