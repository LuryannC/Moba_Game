// Fill out your copyright notice in the Description page of Project Settings.


#include "Crunch/Public/Player/CCharacter.h"

#include "CGameplayTags.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GAS/CAbilitySystemComponent.h"
#include "GAS/CAbilitySystemStatics.h"
#include "GAS/CAttributeSet.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"
#include "Widgets/COverHeadStatsGauge.h"


ACCharacter::ACCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	CAbilitySystemComponent = CreateDefaultSubobject<UCAbilitySystemComponent>(TEXT("CAbilitySystemComponent"));
	CAttributeSet = CreateDefaultSubobject<UCAttributeSet>(TEXT("CAttributeSet"));
	OverHeadWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverHeadStatsGauge"));
	OverHeadWidgetComponent->SetupAttachment(GetRootComponent());

	BindGASChangeDelegates();
	
	PerceptionStimuliSourceComponent = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("PerceptionStimuliSourceComponent"));
}

void ACCharacter::BeginPlay()
{
	Super::BeginPlay();
	ConfigureOverHeadWidget();
	RelativeMeshTransform = GetMesh()->GetRelativeTransform();

	if (PerceptionStimuliSourceComponent)
	{
		PerceptionStimuliSourceComponent.Get()->RegisterForSense(UAISense_Sight::StaticClass());
	}
}

void ACCharacter::ServerSideInit()
{
	CAbilitySystemComponent.Get()->InitAbilityActorInfo(this, this);
	CAbilitySystemComponent.Get()->ApplyInitialEffects();
	CAbilitySystemComponent.Get()->GrantInitialAbilities();
}

void ACCharacter::ClientSideInit()
{
	CAbilitySystemComponent.Get()->InitAbilityActorInfo(this, this);
}

void ACCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

UAbilitySystemComponent* ACCharacter::GetAbilitySystemComponent() const
{
	return CAbilitySystemComponent;
}

void ACCharacter::BindGASChangeDelegates()
{
	if (CAbilitySystemComponent)
	{
		CAbilitySystemComponent.Get()->RegisterGameplayTagEvent(CGameplayTags::Dead.GetTag()).AddUObject(this, &ACCharacter::OnDeath);
	}
}

void ACCharacter::OnDeath(const FGameplayTag Tag, int32 NewCount)
{
	if (NewCount != 0)
	{
		StartDeathSequence();
	}
	else
	{
		Respawn();
	}
}

bool ACCharacter::IsLocallyControlledByPlayer() const
{
	return GetLocalRole() == ROLE_AutonomousProxy || GetRemoteRole() == ROLE_AutonomousProxy;
}

void ACCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	if (NewController && !NewController->IsPlayerController())
	{
		ServerSideInit();
	}
}

void ACCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACCharacter, TeamId);
}

void ACCharacter::ConfigureOverHeadWidget()
{
	if (!OverHeadWidgetComponent)
	{
		return;
	}

	if (IsLocallyControlledByPlayer())
	{
		OverHeadWidgetComponent->SetHiddenInGame(true);
		return;
	}
	
	UCOverHeadStatsGauge* OverHeadStatsGauge = Cast<UCOverHeadStatsGauge>(OverHeadWidgetComponent.Get()->GetUserWidgetObject());
	if (OverHeadStatsGauge)
	{
		OverHeadStatsGauge->ConfigureWithASC(GetAbilitySystemComponent());
		OverHeadWidgetComponent->SetHiddenInGame(false);

		GetWorldTimerManager().ClearTimer(StatGaugeVisibilityUpdateTimerHandle);
		GetWorldTimerManager().SetTimer(
			StatGaugeVisibilityUpdateTimerHandle,
			this,
			&ACCharacter::UpdateStatGaugeVisibility,
			HeadStatGaugeVisibilityUpdateRate,
			true);
	}
}

void ACCharacter::UpdateStatGaugeVisibility()
{
	APawn* LocalPlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	if (LocalPlayerPawn)
	{
		float DistanceSquared = FVector::DistSquared(GetActorLocation(), LocalPlayerPawn->GetActorLocation());
		OverHeadWidgetComponent->SetHiddenInGame(DistanceSquared > HeadStatGaugeVisibilityRangeSquared);
	}
}

void ACCharacter::SetStatusGaugeEnabled(const bool bIsEnabled)
{
	GetWorldTimerManager().ClearTimer(StatGaugeVisibilityUpdateTimerHandle);
	if (bIsEnabled)
	{
		ConfigureOverHeadWidget();
	}
	else
	{
		OverHeadWidgetComponent.Get()->SetHiddenInGame(true);
	}
}

bool ACCharacter::IsDead() const
{
	return GetAbilitySystemComponent()->HasMatchingGameplayTag(CGameplayTags::Dead.GetTag());
}

void ACCharacter::RespawnImmediately()
{
	if (HasAuthority())
	{
		GetAbilitySystemComponent()->RemoveActiveEffectsWithGrantedTags(FGameplayTagContainer(CGameplayTags::Dead.GetTag()));
	}
}

void ACCharacter::OnDeathMontageFinished()
{
	if (IsDead())
	{
		SetRagdollEnabled(true);
	}
}

void ACCharacter::SetRagdollEnabled(bool bIsEnabled)
{
	if (bIsEnabled)
	{
		GetMesh()->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		GetMesh()->SetSimulatePhysics(true);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	}
	else
	{
		GetMesh()->SetSimulatePhysics(false);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetMesh()->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		GetMesh()->SetRelativeTransform(RelativeMeshTransform);
	}
}

void ACCharacter::PlayDeathAnimation()
{
	if (DeathMontage)
	{
		float MontageDuration = PlayAnimMontage(DeathMontage);
		GetWorldTimerManager().SetTimer(DeathMontageTimerHandle, this, &ACCharacter::OnDeathMontageFinished, MontageDuration + DeathMontageFinishTimeShift);
	}
}

void ACCharacter::StartDeathSequence()
{
	OnDeath();

	if (CAbilitySystemComponent)
	{
		CAbilitySystemComponent->CancelAllAbilities();
	}
	
	PlayDeathAnimation();
	SetStatusGaugeEnabled(false);
	
	GetCharacterMovement()->SetMovementMode(MOVE_None);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetAIPerceptionStimuliSourceEnabled(false);
}

void ACCharacter::Respawn()
{
	OnRespawn();
	SetAIPerceptionStimuliSourceEnabled(true);
	SetRagdollEnabled(false);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	GetMesh()->GetAnimInstance()->StopAllMontages(0.f);
	SetStatusGaugeEnabled(true);

	if (HasAuthority() && GetController())
	{
		TWeakObjectPtr<AActor> StartSpot = GetController()->StartSpot;
		if (StartSpot.IsValid())
		{
			SetActorTransform(StartSpot->GetActorTransform());
		}
	}

	if (CAbilitySystemComponent)
	{
		CAbilitySystemComponent->ApplyFullStatEffect();
	}
}

void ACCharacter::OnDeath()
{
}

void ACCharacter::OnRespawn()
{
}

void ACCharacter::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	TeamId = NewTeamID;
}

FGenericTeamId ACCharacter::GetGenericTeamId() const
{
	return TeamId;
}

void ACCharacter::OnRep_TeamID()
{
}

void ACCharacter::SetAIPerceptionStimuliSourceEnabled(bool bIsEnabled)
{
	if (!PerceptionStimuliSourceComponent)
	{
		return;
	}

	if (bIsEnabled)
	{
		PerceptionStimuliSourceComponent.Get()->RegisterWithPerceptionSystem();
	}
	else
	{
		PerceptionStimuliSourceComponent.Get()->UnregisterFromPerceptionSystem();
	}
}
