// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GA_Combo.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "CGameplayTags.h"
#include "GameplayTagsManager.h"
#include "GAS/CAbilitySystemStatics.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"

UGA_Combo::UGA_Combo()
{
	AbilityTags.AddTag(CGameplayTags::BasicAttack);
	BlockAbilitiesWithTag.AddTag(CGameplayTags::BasicAttack);
}

void UGA_Combo::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		UAbilityTask_PlayMontageAndWait* PlayComboMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, ComboMontage);
		PlayComboMontageTask->OnBlendOut.AddDynamic(this, &UGA_Combo::K2_EndAbility);
		PlayComboMontageTask->OnCancelled.AddDynamic(this, &UGA_Combo::K2_EndAbility);
		PlayComboMontageTask->OnCompleted.AddDynamic(this, &UGA_Combo::K2_EndAbility);
		PlayComboMontageTask->OnInterrupted.AddDynamic(this, &UGA_Combo::K2_EndAbility);
		PlayComboMontageTask->ReadyForActivation();

		UAbilityTask_WaitGameplayEvent* WaitComboChangeEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, GetComboChangedEventTag(), nullptr, false, false);
		WaitComboChangeEventTask->EventReceived.AddDynamic(this, &UGA_Combo::ComboChangedEventReceived);
		WaitComboChangeEventTask->ReadyForActivation();
	}

	if (K2_HasAuthority())
	{
		UAbilityTask_WaitGameplayEvent* WaitTargetingEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, CGameplayTags::ComboDamage.GetTag());
		WaitTargetingEventTask->EventReceived.AddDynamic(this, &UGA_Combo::DoDamage);
		WaitTargetingEventTask->ReadyForActivation();
	}

	SetupWaitComboInputPress();
}

FGameplayTag UGA_Combo::GetComboChangedEventTag()
{
	return CGameplayTags::Change.GetTag();
}

FGameplayTag UGA_Combo::GetComboEndedEventTag()
{
	return CGameplayTags::ComboEnd.GetTag();
}

void UGA_Combo::SetupWaitComboInputPress()
{
	UAbilityTask_WaitInputPress* WaitInputPress = UAbilityTask_WaitInputPress::WaitInputPress(this);
	WaitInputPress->OnPress.AddDynamic(this, &UGA_Combo::HandleInputPress);
	WaitInputPress->ReadyForActivation();
}

void UGA_Combo::HandleInputPress(float TimeWaited)
{
	SetupWaitComboInputPress();
	TryCommitCombo();
}

void UGA_Combo::TryCommitCombo()
{
	if (NextComboName == NAME_None)
	{
		return;
	}

	UAnimInstance* AnimInstance = GetOwnerAnimInstance();
	if (!AnimInstance)
	{
		return;
	}

	AnimInstance->Montage_SetNextSection(AnimInstance->Montage_GetCurrentSection(ComboMontage), NextComboName, ComboMontage);
}

TSubclassOf<UGameplayEffect> UGA_Combo::GetCurrentComboDamageEffect() const
{
	UAnimInstance* AnimInstance = GetOwnerAnimInstance();
	if (AnimInstance)
	{
		FName CurrentSectionName = AnimInstance->Montage_GetCurrentSection(ComboMontage);
		const TSubclassOf<UGameplayEffect>* FoundEffectPtr = DamageEffectMap.Find(CurrentSectionName);
		if (FoundEffectPtr)
		{
			return *FoundEffectPtr;
		}
	}

	return DefaultDamageEffect;
}

void UGA_Combo::ComboChangedEventReceived(FGameplayEventData Data)
{
	FGameplayTag EventTag = Data.EventTag;
	if (EventTag == GetComboEndedEventTag())
	{
		NextComboName = NAME_None;
		UE_LOG(LogTemp, Warning, TEXT("next combo is cleared"));
		return;
	}

	TArray<FName> TagsNames;
	UGameplayTagsManager::Get().SplitGameplayTagFName(EventTag, TagsNames);
	NextComboName = TagsNames.Last();

	UE_LOG(LogTemp, Warning, TEXT("next combo is now: %s"), *NextComboName.ToString());
}

void UGA_Combo::DoDamage(FGameplayEventData Data)
{
	TArray<FHitResult> HitResults = GetHitResultsFromSweepLocationTargetData(Data.TargetData, SweepSphereRadius, bDrawDebug, true);

	for (const auto& HitResult : HitResults)
	{
		TSubclassOf<UGameplayEffect> GameplayEffect = GetCurrentComboDamageEffect();
		FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(GameplayEffect, GetAbilityLevel(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo()));

		FGameplayEffectContextHandle EffectContextHandle = MakeEffectContext(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo());
		EffectContextHandle.AddHitResult(HitResult);

		EffectSpecHandle.Data->SetContext(EffectContextHandle);

		ApplyGameplayEffectSpecToTarget(
			GetCurrentAbilitySpecHandle(),
			CurrentActorInfo,
			CurrentActivationInfo,
			EffectSpecHandle,
			UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(HitResult.GetActor()));
	}
}
