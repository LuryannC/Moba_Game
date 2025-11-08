// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/CMinion.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

void ACMinion::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	Super::SetGenericTeamId(NewTeamID);
	PickSkinBasedOnTeamID();
}

void ACMinion::PickSkinBasedOnTeamID()
{
	USkeletalMesh* Skin = *SkinMap.Find(GetGenericTeamId());
	if (Skin)
	{
		GetMesh()->SetSkeletalMesh(Skin);
	}
}

bool ACMinion::IsActive() const
{
	return !IsDead();
}

void ACMinion::Activate()
{
	RespawnImmediately();
}

void ACMinion::SetGoal(AActor* NewGoal)
{
	if (AAIController* AIController = GetController<AAIController>())
	{
		if (UBlackboardComponent* BlackboardComponent = AIController->GetBlackboardComponent())
		{
			BlackboardComponent->SetValueAsObject(GoalBlackboardName, NewGoal);
		}
	}
}

void ACMinion::OnRep_TeamID()
{
	PickSkinBasedOnTeamID();
}
