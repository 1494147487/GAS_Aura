


#include "AbilitySystem/AsyncTasks/WaitCooldownChange.h"
#include "AbilitySystemComponent.h"
#include "ActiveGameplayEffectHandle.h"
#include "GameplayEffectTypes.h"


UWaitCooldownChange* UWaitCooldownChange::WaitForCooldownChange(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayTag& InCooldownTag)
{
	UWaitCooldownChange* WaitCooldownChange = NewObject<UWaitCooldownChange>();
	WaitCooldownChange->ASC = AbilitySystemComponent;
	WaitCooldownChange->CooldownTag = InCooldownTag;

	if (!IsValid(AbilitySystemComponent) || !InCooldownTag.IsValid())
	{
		WaitCooldownChange->EndTask();
		return nullptr;
	}

	// 向 ASC 登记"冷却标签数量变化"的监听  了解冷却标签何时移除
	AbilitySystemComponent->RegisterGameplayTagEvent(//// 接收通知的对象：当前创建出来的异步任务实例  接收后的回调函数：UWaitCooldownChange::CooldownTagChanged
		InCooldownTag, EGameplayTagEventType::NewOrRemoved).AddUObject(WaitCooldownChange, &UWaitCooldownChange::CooldownTagChanged);

	// 监听GameplayEffect 被添加"的通知
	AbilitySystemComponent->OnActiveGameplayEffectAddedDelegateToSelf.AddUObject(WaitCooldownChange, &UWaitCooldownChange::OnActiveEffecttAdded);

	return WaitCooldownChange;
}

void UWaitCooldownChange::EndTask()
{
	if (!IsValid(ASC))return;

	// 取消监听"冷却标签数量变化"的通知
	ASC->RegisterGameplayTagEvent(CooldownTag, EGameplayTagEventType::NewOrRemoved).RemoveAll(this);

	SetReadyToDestroy();// 让这个异步任务实例可以被销毁 异步对象是指工厂函数创建出来的对象，蓝图里用完就没用了，C++里也没用完就没用了，所以要销毁它
	MarkAsGarbage();// 标记为垃圾对象，等待被回收
}

void UWaitCooldownChange::CooldownTagChanged(const FGameplayTag InCooldownTag, int32 NewCount)
{
	if (NewCount == 0)
	{
		CooldownEnd.Broadcast(0.f);
	}
	
}

void UWaitCooldownChange::OnActiveEffecttAdded(UAbilitySystemComponent* TargetASC, const FGameplayEffectSpec& SpecApplied, FActiveGameplayEffectHandle ActiveEffectHandle)
{
	FGameplayTagContainer AssetTags;
	SpecApplied.GetAllAssetTags(AssetTags);   // 取出 GE 自己的"资产标签"（描述这个 GE 是什么）

	FGameplayTagContainer GrantedTags;
	SpecApplied.GetAllGrantedTags(GrantedTags); // 取出 GE 授予目标 Actor 的标签

	if (AssetTags.HasTagExact(CooldownTag) || GrantedTags.HasTagExact(CooldownTag))
	{
		//// 构造查询条件：匹配"授予了冷却标签"的 Active GE（GetSingleTagContainer 把单个标签包成容器）																								单个标签包成只有一个元素的容器
		FGameplayEffectQuery GameplayEffectQuery = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(CooldownTag.GetSingleTagContainer());
		// 用条件在 ASC 上查询，返回所有匹配 GE 的剩余时间（秒），可能为空，也可能有多个
		TArray<float> TimesRemaining = ASC->GetActiveEffectsTimeRemaining(GameplayEffectQuery);

		if (TimesRemaining.Num() > 0)
		{
		                       
			float HighestTime = TimesRemaining[0];    // 先假设第一个就是最大的
			for (int32 i = 0; i < TimesRemaining.Num(); i++)
			{
				if (TimesRemaining[i] > HighestTime)  // 发现更大的，就更新记录
				{
					HighestTime = TimesRemaining[i];
				
				}
			}

			float TimeRemaining = HighestTime;        // 最终的冷却剩余时间
			CooldownStart.Broadcast(TimeRemaining);
		}
	}
}
