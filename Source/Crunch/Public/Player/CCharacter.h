// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "GenericTeamAgentInterface.h"
#include "GameFramework/Character.h"
#include "CCharacter.generated.h"

class UWidgetComponent;
class UCAttributeSet;
class UCAbilitySystemComponent;

UCLASS()
class CRUNCH_API ACCharacter : public ACharacter, public IAbilitySystemInterface, public IGenericTeamAgentInterface
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

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
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
	void BindGASChangeDelegates();
	void OnDeath(const FGameplayTag Tag, int32 NewCount);
	
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

	void SetStatusGaugeEnabled(bool bIsEnabled);
	/**************************************************************/
	/*                       Death and respawn                    */
	/**************************************************************/
public:
	bool IsDead() const;
	void RespawnImmediately();
private:
	FTransform RelativeMeshTransform;
	
	UPROPERTY(EditDefaultsOnly, Category="Death")
	float DeathMontageFinishTimeShift = -0.8f;
	
	UPROPERTY(EditDefaultsOnly, Category="Death")
	TObjectPtr<UAnimMontage> DeathMontage;

	FTimerHandle DeathMontageTimerHandle;
	void OnDeathMontageFinished();
	void SetRagdollEnabled(bool bIsEnabled);

	void PlayDeathAnimation();
	
	void StartDeathSequence();
	void Respawn();

	virtual void OnDeath();
	virtual void OnRespawn();

	/**************************************************************/
	/*                            Team                           */
	/**************************************************************/
public:
	// ~ Start of IGenericTeamAgentInterface
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;
	virtual FGenericTeamId GetGenericTeamId() const override;
	// ~ End of IGenericTeamAgentInterface

private:
	UPROPERTY(ReplicatedUsing= OnRep_TeamID)
	FGenericTeamId TeamId;

	UFUNCTION()
	virtual void OnRep_TeamID();

	/**************************************************************/
	/*                            AI                              */
	/**************************************************************/
private:
	void SetAIPerceptionStimuliSourceEnabled(bool bIsEnabled);
	
	UPROPERTY()
	TObjectPtr<class UAIPerceptionStimuliSourceComponent> PerceptionStimuliSourceComponent;

public:
};
