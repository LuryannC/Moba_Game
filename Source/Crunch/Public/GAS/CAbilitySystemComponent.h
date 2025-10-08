// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "CGameplayAbility.h"
#include "CAbilitySystemComponent.generated.h"


enum class ECAbilityInputID : uint8;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CRUNCH_API UCAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	UCAbilitySystemComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void ApplyInitialEffects();
	void GrantInitialAbilities();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly, Category="Gameplay Effects")
	TArray<TSubclassOf<UGameplayEffect>> InitialEffects;

	UPROPERTY(EditDefaultsOnly, Category="Gameplay Abilities")
	TMap<ECAbilityInputID, TSubclassOf<UCGameplayAbility>> Abilities;

	UPROPERTY(EditDefaultsOnly, Category="Gameplay Abilities")
	TMap<ECAbilityInputID, TSubclassOf<UCGameplayAbility>> BasicAbilities;
};
