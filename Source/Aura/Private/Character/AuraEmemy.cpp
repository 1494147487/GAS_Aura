


#include "Character/AuraEmemy.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"	
#include "Aura/Aura.h"

AAuraEmemy::AAuraEmemy()
{
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>(TEXT("AttributeSet"));

}

void AAuraEmemy::HighlightActor()
{

	GetMesh()->SetRenderCustomDepth(true);
	GetMesh()->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);

	if (Weapon)
	{
		Weapon->SetRenderCustomDepth(true);
		Weapon->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
	}
}

void AAuraEmemy::UnHighlightActor()
{

	GetMesh()->SetRenderCustomDepth(false);
	if (Weapon)
	{
		Weapon->SetRenderCustomDepth(false);
	}
}

void AAuraEmemy::BeginPlay()
{
	Super::BeginPlay();

	AbilitySystemComponent->InitAbilityActorInfo(this, this);
}
