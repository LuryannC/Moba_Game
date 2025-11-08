// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "GameplayTagContainer.h"
#include "Perception/AIPerceptionTypes.h"
#include "CAIController.generated.h"

UCLASS()
class CRUNCH_API ACAIController : public AAIController
{
	GENERATED_BODY()

public:
	ACAIController();

	virtual void OnPossess(APawn* InPawn) override;

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	
private:

	UPROPERTY(EditDefaultsOnly, Category="AI Behaviour")
	FName BBTargetKeyName;

	UPROPERTY(EditDefaultsOnly, Category="AI Behaviour")
	class UBehaviorTree* BehaviorTree;
	
	UPROPERTY(VisibleDefaultsOnly, Category="Perception")
	class UAIPerceptionComponent* AIPerceptionComponent;

	UPROPERTY(EditDefaultsOnly, Category="Perception")
	class UAISenseConfig_Sight* SightConfig;

	UFUNCTION()
	void TargetPerceptionUpdated(AActor* TargetActor, FAIStimulus Stimulus);

	UFUNCTION()
	void TargetPerceptionForgotten(AActor* ForgottenActor);

	const UObject* GetCurrentTarget() const;
	void SetCurrentTarget(AActor* NewTarget);

	AActor* GetNextPerceivedActor() const;

	void ForgetActorIfDead(AActor* ActorToForget);

	void ClearAndDisableAllSenses();
	void EnableAllSenses();

	void OnPawnDeadTagUpdated(const FGameplayTag Tag, int32 Count);
};
