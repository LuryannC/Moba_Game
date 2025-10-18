// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "GameFramework/PlayerController.h"
#include "CPlayerController.generated.h"

class UCGameplayWidget;
class ACPlayerCharacter;
/**
 * 
 */
UCLASS()
class CRUNCH_API ACPlayerController : public APlayerController, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	// Only called on the server
	virtual void OnPossess(APawn* InPawn) override;
	// only called on the client
	virtual void AcknowledgePossession(class APawn* P) override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	// ~ Start of IGenericTeamAgentInterface
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;
	virtual FGenericTeamId GetGenericTeamId() const override;
	// ~ End of IGenericTeamAgentInterface
private:

	void SpawnGameplayWidget();
	
	UPROPERTY()
	TObjectPtr<ACPlayerCharacter> CPlayerCharacter;

	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<UCGameplayWidget> GameplayWidgetClass;

	UPROPERTY()
	TObjectPtr<UCGameplayWidget> GameplayWidget;

	UPROPERTY(Replicated)
	FGenericTeamId TeamId;
};
