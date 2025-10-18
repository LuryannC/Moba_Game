// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CCharacter.h"
#include "GenericTeamAgentInterface.h"
#include "CPlayerCharacter.generated.h"

enum class ECAbilityInputID : uint8;
class UInputAction;
class UInputComponent;
class UInputMappingContext;

struct FInputActionValue;

UCLASS()
class CRUNCH_API ACPlayerCharacter : public ACCharacter
{
	GENERATED_BODY()

public:
	ACPlayerCharacter();
	virtual void PawnClientRestart() override;

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:

	UPROPERTY(VisibleDefaultsOnly, Category="View")
	TObjectPtr<class USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleDefaultsOnly, Category="View")
	TObjectPtr<class UCameraComponent> ViewCamera;
	
	FVector GetLookRightDirection() const;
	FVector GetLookForwardDirection() const;
	FVector GetMoveForwardDirection() const;

	/**************************************************************/
	/*                           Input                            */ 
	/**************************************************************/
private:
	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputAction> JumpInputAction;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputAction> LookInputAction;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputAction> MoveInputAction;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TMap<ECAbilityInputID, UInputAction*> GameplayAbilityInputActions;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputMappingContext> GameplayInputMappingContext;
	
	void LookAction(const FInputActionValue& InputActionValue);
	void MoveAction(const FInputActionValue& InputActionValue);
	void HandleAbilityInput(const FInputActionValue& InputActionValue, ECAbilityInputID InputID);


	virtual void OnDeath() override;
	virtual void OnRespawn() override;
};
