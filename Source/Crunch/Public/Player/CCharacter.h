// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "CCharacter.generated.h"

class UWidgetComponent;
class UCAttributeSet;
class UCAbilitySystemComponent;

UCLASS()
class CRUNCH_API ACCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACCharacter();
	void ServerSideInit();
	void ClientSideInit();
	bool IsLocallyControlledByPlayer() const;

	// Only called on server
	virtual void PossessedBy(AController* NewController) override;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/**************************************************************/
	/*                       Gameplay Ability                     */
	/**************************************************************/
public:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

private:
	UPROPERTY(VisibleDefaultsOnly, Category="Gameplay Ability")
	TObjectPtr<UCAbilitySystemComponent> CAbilitySystemComponent;
	UPROPERTY()
	TObjectPtr<UCAttributeSet> CAttributeSet;

	/**************************************************************/
	/*                            UI                              */
	/**************************************************************/
private:
	UPROPERTY(VisibleDefaultsOnly, Category="Gameplay Ability")
	TObjectPtr<UWidgetComponent> OverHeadWidgetComponent;
	void ConfigureOverHeadWidget();

	UPROPERTY(EditDefaultsOnly, Category="UI")
	float HeadStatGaugeVisibilityUpdateRate = 1.f;

	UPROPERTY(EditDefaultsOnly, Category="UI")
	float HeadStatGaugeVisibilityRangeSquared = 10000000.f;
	
	FTimerHandle StatGaugeVisibilityUpdateTimerHandle;
	void UpdateStatGaugeVisibility();
};
