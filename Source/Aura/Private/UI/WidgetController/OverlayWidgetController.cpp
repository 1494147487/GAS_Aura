


#include "UI/WidgetController/OverlayWidgetController.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include <AbilitySystem/AuraAbilitySystemComponent.h>
#include <AbilitySystem/Data/AbilityInfo.h>
#include "Player/AuraPlayerState.h"
#include "AbilitySystem/Data/LevelUpInfo.h"
#include <AuraGameplayTags.h>


void UOverlayWidgetController::BroadcastInitialValues()
{
	
	
	OnHealthChanged.Broadcast(GetAuraAS()->GetHealth());//广播当前的生命值给所有订阅了OnHealthChanged事件的监听者
	OnMaxHealthChanged.Broadcast(GetAuraAS()->GetMaxHealth());
	OnManaChanged.Broadcast(GetAuraAS()->GetMana());
	OnMaxManaChanged.Broadcast(GetAuraAS()->GetMaxMana());

}

void UOverlayWidgetController::BindCallbacksToDependencies()//重写父类的虚函数，绑定回调函数到依赖的组件上  本质上也是个初始化函数，绑定回调函数到依赖的组件上
{

	////拿到 ASC     用 Health 的属性句柄取出「Health 变化委托」  把 this->HealthChanged 加进该委托的监听列表
	//AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
	//												//this：回调时用的实例（这个 OverlayWidgetController）
	//	UAuraAttributeSet::GetHealthAttribute()).AddUObject(this, &UOverlayWidgetController::HealthChanged);

	
	GetAuraPS()->OnXPChangedDelegate.AddUObject(this, &UOverlayWidgetController::OnXPChanged);
	GetAuraPS()->OnLevelChangedDelegate.AddLambda(
		[this](int32 NewLevel)
		{
			OnPlayerLevelChangedDelegate.Broadcast(NewLevel);
		}

	);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		UAuraAttributeSet::GetHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnHealthChanged.Broadcast(Data.NewValue);
			}
		);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		UAuraAttributeSet::GetMaxHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnMaxHealthChanged.Broadcast(Data.NewValue);
			}
		);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		UAuraAttributeSet::GetManaAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnManaChanged.Broadcast(Data.NewValue);
			}
		);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		UAuraAttributeSet::GetMaxManaAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnMaxManaChanged.Broadcast(Data.NewValue);
			}
		);


	if (GetAuraASC())
	{
		if (GetAuraASC()->bStartupAbilitiesGiven)
		{
			BroadcastAbilityInfo();
		}
		else
		{
			GetAuraASC()->AbilitiesGivenDelegate.AddUObject(this, &UOverlayWidgetController::BroadcastAbilityInfo);
		}
			
		GetAuraASC()->AbilityEquipped.AddUObject(this, &UOverlayWidgetController::OnAbilityEquipped);

		GetAuraASC()->EffectAssetTags.AddLambda(
			[this](const FGameplayTagContainer& AssetTags)
			{
				for (const FGameplayTag& Tag : AssetTags)
				{
					const FString Msg = FString::Printf(TEXT("GE Tag: %s"), *Tag.ToString());
					GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Blue, Msg);
					// 构造一个"父级"标签 Message，作为筛选条件
					// RequestGameplayTag 是从全局GameplayTag表里查找/请求一个已注册的Tag对象
					// 这里没有直接写死 "Message.HealthPotion"，而是只请求了顶层的 "Message"，
					// 目的是让这个函数能响应所有 Message.* 下的子标签（比如 Message.HealthPotion、Message.LevelUp 等）
					FGameplayTag MessageTag = FGameplayTag::RequestGameplayTag(FName("Message"));

					// 判断传进来的 Tag 是否属于 Message 这个大类
					// MatchesTag 是层级匹配：只要 Tag 是 MessageTag 本身或者它的任意子标签，就返回 true
					// 这里的判断顺序很关键：必须是 "具体的Tag.MatchesTag(笼统的MessageTag)"
					// 因为MatchesTag检查的是"调用者是否属于（或等于）参数这个类别"，方向反了就永远匹配不上

					if (Tag.MatchesTag(MessageTag))
					{
						//// 作用：拿着一个GameplayTag去DataTable里查找"某一行"，行结构类型由模板参数指定为 FUIWidgetRow
						const FUIWidgetRow* Row = GetDataTableRowByTag<FUIWidgetRow>(MessageWidgetDataTable, Tag);//
						MessageWidgetRowDelegate.Broadcast(*Row);
					}

				}
			}

		);
	}
}


void UOverlayWidgetController::OnXPChanged(int32 NewXP)
{

	const ULevelUpInfo* LevelUpInfo = GetAuraPS()->LevelUpInfo;
	checkf(LevelUpInfo, TEXT("Unabled to find LevelUpInfo. Please fill out AuraPlayerState Blueprint"));

	const int32 Level = LevelUpInfo->FindLevelForXP(NewXP);
	const int32 MaxLevel = LevelUpInfo->LevelUpInformation.Num();

	if (Level <= MaxLevel && Level > 0)
	{
		const int32 LevelUpRequirement = LevelUpInfo->LevelUpInformation[Level].LevelUpRequirement;
		const int32 PreviousLevelUpRequirement = LevelUpInfo->LevelUpInformation[Level - 1].LevelUpRequirement;

		const int32 DeltaLevelRequirement = LevelUpRequirement - PreviousLevelUpRequirement;
		const int32 XPForThisLevel = NewXP - PreviousLevelUpRequirement;

		const float XPBarPercent = static_cast<float>(XPForThisLevel) / static_cast<float>(DeltaLevelRequirement);

		OnXPPercentChangedDelegate.Broadcast(XPBarPercent);
	}
}

void UOverlayWidgetController::OnAbilityEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& Status, const FGameplayTag& Slot, const FGameplayTag& PreviousSlot)
{
	const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();

	FAuraAbilityInfo LastSlotInfo;
	LastSlotInfo.StatusTag = GameplayTags.Abilities_Status_Unlocked;
	LastSlotInfo.InputTag = PreviousSlot;
	LastSlotInfo.AbilityTag = GameplayTags.Abilities_None;
	// Broadcast empty info if PreviousSlot is a valid slot. Only if equipping an already-equipped spell
	AbilityInfoDelegate.Broadcast(LastSlotInfo);

	FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
	Info.StatusTag = Status;
	Info.InputTag = Slot;
	AbilityInfoDelegate.Broadcast(Info);

	
}

