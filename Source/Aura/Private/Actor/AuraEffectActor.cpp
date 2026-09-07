


#include "Actor/AuraEffectActor.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystemComponent.h"
#include "Components/SphereComponent.h"
// Sets default values
AAuraEffectActor::AAuraEffectActor()
{
 	
	PrimaryActorTick.bCanEverTick = false;

	SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot")));//创建一个默认的场景组件作为根组件

}

////待办事项：将此处修改为应用一个游戏效果。目前暂时使用 cost_ge 进行设置



void AAuraEffectActor::BeginPlay()
{
	Super::BeginPlay();
	
	
}

//找到目标身上的 ASC → 准备一个 Gameplay Effect → 设置这个 Effect 的上下文 → 把 Effect 应用给目标。
void AAuraEffectActor::ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> GameplayEffectClass)
{
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);//获取目标的 AbilitySystemComponent
	if (TargetASC == nullptr) return;//如果目标没有 ASC，直接返回

	check(GameplayEffectClass);//检查 GameplayEffectClass 是否有效

	FGameplayEffectContextHandle EffectContextHandle = TargetASC->MakeEffectContext();//

	// 给 Effect Context 添加 Source Object（来源对象）
	//
	// this 就是当前的 AAuraEffectActor。
	EffectContextHandle.AddSourceObject(this);//


	const FGameplayEffectSpecHandle EffectSpecHandle = TargetASC->MakeOutgoingSpec(GameplayEffectClass, ActorLevel, EffectContextHandle);//配置好了GE信息

	//让这个 ASC，把这个 GE 效果施加到它自己(一般是角色)身上
	//Active Gameplay Effect Handle   这个 Handle 以后可以用来找到、判断、移除这个已经生效的 GE。
	const FActiveGameplayEffectHandle ActiveEffectHandle = 
		TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());//应用这个 Effect 给目标的 ASC
	//这个 Gameplay Effect 是不是永久型（Infinite）的？
	const bool bIsInfinite = EffectSpecHandle.Data.Get()->Def.Get()->DurationPolicy == EGameplayEffectDurationType::Infinite;

	if (bIsInfinite && InfiniteEffectRemovalPolicy == EffectRemovalPolicy::RemoveOnEndOverlap)
	{
		ActiveEffectHandles.Add(ActiveEffectHandle, TargetASC);
	}
}

void AAuraEffectActor::OnOverlap(AActor* TargetActor)
{
	if (InstantEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
		ApplyEffectToTarget(TargetActor, InstantGameplayEffectClass);
	}

	if (DurationEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
		ApplyEffectToTarget(TargetActor, DurationGameplayEffectClass);
	}

	if (InfiniteEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
		ApplyEffectToTarget(TargetActor, InfiniteGameplayEffectClass);
	}
}

void AAuraEffectActor::OnEndOverlap(AActor* TargetActor)
{
	if (InstantEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, InstantGameplayEffectClass);
	}

	if (DurationEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, DurationGameplayEffectClass);
	}

	if (InfiniteEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, InfiniteGameplayEffectClass);
	}

	if (InfiniteEffectRemovalPolicy == EffectRemovalPolicy::RemoveOnEndOverlap)
	{
		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
		if (!IsValid(TargetASC))return;//

		TArray<FActiveGameplayEffectHandle> HandlesToRemove;

		for (TTuple<FActiveGameplayEffectHandle, UAbilitySystemComponent*> Handlepair : ActiveEffectHandles)
		{
			if (TargetASC == Handlepair.Value)
			{
				TargetASC->RemoveActiveGameplayEffect(Handlepair.Key,1);//// ① 真正的移除发生在这里
				HandlesToRemove.Add(Handlepair.Key);
			}
		}

		for (FActiveGameplayEffectHandle& Handle :HandlesToRemove)
		{
			ActiveEffectHandles.FindAndRemoveChecked(Handle);
		}


	}
}



