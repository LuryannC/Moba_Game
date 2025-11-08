// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/CAIController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "BrainComponent.h"
#include "CGameplayTags.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Player/CCharacter.h"


ACAIController::ACAIController()
{
	PrimaryActorTick.bCanEverTick = true;

	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent"));
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));

	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = false;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = false;

	SightConfig->SightRadius = 1000.f;
	SightConfig->LoseSightRadius = 1200.f;

	SightConfig->SetMaxAge(5.0f);
	
	SightConfig->PeripheralVisionAngleDegrees = 180.f;

	AIPerceptionComponent->ConfigureSense(*SightConfig);
	AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &ACAIController::TargetPerceptionUpdated);
	AIPerceptionComponent->OnTargetPerceptionForgotten.AddDynamic(this, &ACAIController::TargetPerceptionForgotten);
}

void ACAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	IGenericTeamAgentInterface* PawnTeamInterface = Cast<IGenericTeamAgentInterface>(InPawn);
	if (PawnTeamInterface)
	{
		SetGenericTeamId(PawnTeamInterface->GetGenericTeamId());
		ClearAndDisableAllSenses();
		EnableAllSenses();
	}

	UAbilitySystemComponent* PawnASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InPawn);
	if (PawnASC)
	{
		PawnASC->RegisterGameplayTagEvent(CGameplayTags::Dead.GetTag()).AddUObject(this, &ACAIController::OnPawnDeadTagUpdated);
	}
}

void ACAIController::BeginPlay()
{
	Super::BeginPlay();
	if (BehaviorTree)
	{
		RunBehaviorTree(BehaviorTree);
	}
}

void ACAIController::TargetPerceptionUpdated(AActor* TargetActor, FAIStimulus Stimulus)
{
	if (Stimulus.WasSuccessfullySensed())
	{
		if (!GetCurrentTarget())
		{
			SetCurrentTarget(TargetActor);
		}
	}
	else
	{
		ForgetActorIfDead(TargetActor);
	}
}

const UObject* ACAIController::GetCurrentTarget() const
{
	const UBlackboardComponent* BlackboardComponent = GetBlackboardComponent();
	if (BlackboardComponent)
	{
		return GetBlackboardComponent()->GetValueAsObject(BBTargetKeyName);
	}
	return nullptr;
}

void ACAIController::SetCurrentTarget(AActor* NewTarget)
{
	if (NewTarget)
	{
		GetBlackboardComponent()->SetValueAsObject(BBTargetKeyName, NewTarget);
	}
	else
	{
		GetBlackboardComponent()->ClearValue(BBTargetKeyName);
	}
}

AActor* ACAIController::GetNextPerceivedActor() const
{
	if (PerceptionComponent)
	{
		TArray<AActor*> Actors;
		AIPerceptionComponent->GetPerceivedHostileActors(Actors);

		if (Actors.Num() != 0)
		{
			return Actors[0];
		}
	}
	
	return nullptr;
}

void ACAIController::ForgetActorIfDead(AActor* ActorToForget)
{
	const UAbilitySystemComponent* ActorASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(ActorToForget);
	if (!ActorASC)
	{
		return;
	}

	if (ActorASC->HasMatchingGameplayTag(CGameplayTags::Dead.GetTag()))
	{
		for (UAIPerceptionComponent::TActorPerceptionContainer::TIterator It = AIPerceptionComponent->GetPerceptualDataIterator(); It; ++It)
		{
			if (It.Key() != ActorToForget)
			{
				continue;
			}

			for (FAIStimulus& Stimulus : It.Value().LastSensedStimuli)
			{
				Stimulus.SetStimulusAge(TNumericLimits<float>::Max());
			}
		}
	}
}

void ACAIController::TargetPerceptionForgotten(AActor* ForgottenActor)
{
	if (!ForgottenActor)
	{
		return;
	}

	if (GetCurrentTarget() == ForgottenActor)
	{
		SetCurrentTarget(GetNextPerceivedActor());
	}
}

void ACAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACAIController::ClearAndDisableAllSenses()
{
	AIPerceptionComponent->AgeStimuli(TNumericLimits<float>::Max());

	for (auto SenseConfigIt = AIPerceptionComponent->GetSensesConfigIterator(); SenseConfigIt; ++SenseConfigIt)
	{
		AIPerceptionComponent->SetSenseEnabled((*SenseConfigIt)->GetSenseImplementation(),false);
	}

	if (GetBlackboardComponent())
	{
		GetBlackboardComponent()->ClearValue(BBTargetKeyName);
	}
}

void ACAIController::EnableAllSenses()
{
	for (auto SenseConfigIt = AIPerceptionComponent->GetSensesConfigIterator(); SenseConfigIt; ++SenseConfigIt)
	{
		AIPerceptionComponent->SetSenseEnabled((*SenseConfigIt)->GetSenseImplementation(),true);
	}
}

void ACAIController::OnPawnDeadTagUpdated(const FGameplayTag Tag, int32 Count)
{
	if (Count != 0)
	{
		GetBrainComponent()->StopLogic("Dead");
		ClearAndDisableAllSenses();
	}
	else
	{
		GetBrainComponent()->StartLogic();
		EnableAllSenses();
	}
}
