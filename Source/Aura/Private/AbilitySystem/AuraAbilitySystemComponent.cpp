


#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include <AbilitySystem/Abilities/AuraGameplayAbility.h>

void UAuraAbilitySystemComponent::AbilityActorInfoSet()
{	//语义:"每当有一个 GameplayEffect 被施加到【这个 ASC 所代表的 Actor】身上时"触发。
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UAuraAbilitySystemComponent::ClientEffectApplied);//
	
	//const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
	////GameplayTags.Attributes_Secondary_Armor.ToString()

	//GEngine->AddOnScreenDebugMessage(
	//	-1,
	//	10.f,
	//	FColor::Orange, 
	//	FString::Printf(TEXT("Tag: %s"), *GameplayTags.Attributes_Secondary_Armor.ToString()));
}

void UAuraAbilitySystemComponent::AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities)
{
	for (TSubclassOf<UGameplayAbility> AbilityClass : StartupAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		if (const UAuraGameplayAbility* AuraAbility = Cast<UAuraGameplayAbility>(AbilitySpec.Ability))
		{//DynamicAbilityTags 是 GAS 提供的一个专门用来在运行时给技能实例追加标签的容器
			AbilitySpec.DynamicAbilityTags.AddTag(AuraAbility->StartupInputTag);//把技能自己配置好的 StartupInputTag,写进这份 Spec 的"动态标签"里
			GiveAbility(AbilitySpec);
		}

	}
}

void UAuraAbilitySystemComponent::AbilityInputTagHeld(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid())return;
	// 遍历这个 ASC 当前所有"可激活"的技能
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		// 找到动态 Tag 里精确匹配这次输入 Tag 的那个技能(对应之前授予时打上的 StartupInputTag)
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			AbilitySpecInputPressed(AbilitySpec);
			// 避免重复激活已经在运行中的技能
			if (!AbilitySpec.IsActive())
			{
				// 真正激活这个技能
				TryActivateAbility(AbilitySpec.Handle);
			}
		}
	}

}

void UAuraAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid())return;
	// 遍历这个 ASC 当前所有"可激活"的技能
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		// 找到动态 Tag 里精确匹配这次输入 Tag 的那个技能(对应之前授予时打上的 StartupInputTag)
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			AbilitySpecInputReleased(AbilitySpec);
			
		}
	}
}

void UAuraAbilitySystemComponent::ClientEffectApplied_Implementation(UAbilitySystemComponent* AbilitySystemComponent,
	const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle)
{
	FGameplayTagContainer TagContainer;
	EffectSpec.GetAllAssetTags(TagContainer);

	EffectAssetTags.Broadcast(TagContainer);

	
}
