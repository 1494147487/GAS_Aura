


#include "AbilitySystem/Abilities/AuraProjectSpell.h"
#include "Actor/AuraProjectile.h"  
#include "Interaction/CombatInterface.h"
#include <AbilitySystemBlueprintLibrary.h>
#include "AbilitySystemComponent.h" 

void UAuraProjectSpell::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,               // 这个技能实例在 ASC 里的唯一标识句柄，用于定位对应的 AbilitySpec
	const FGameplayAbilityActorInfo* ActorInfo,            // 打包了拥有者/表现Actor/ASC等常用指针的上下文信息
	const FGameplayAbilityActivationInfo ActivationInfo,   // 本次激活的网络身份信息（预测/服务器权威等）
	const FGameplayEventData* TriggerEventData             // 若由事件触发，携带触发数据；非事件触发时为 nullptr
)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	

	

}

void UAuraProjectSpell::SpawnProjectile(const FVector& ProjectileTargetLocation)
{
	const bool bIsServer = GetAvatarActorFromActorInfo()->HasAuthority();   // 判断当前执行环境是否为服务器（权威端）
	if (!bIsServer) return;													// GAS 的技能生成投射物属于有网络权威性的操作，客户端预测阶段直接跳过，只由服务器真正生成 Actor


	ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetAvatarActorFromActorInfo());

	if (CombatInterface)
	{
		const FVector SocketLocation = CombatInterface->GetCombatSocketLocation();

		FRotator Rotation = (ProjectileTargetLocation - SocketLocation).Rotation();
		Rotation.Pitch = 0.f;

		FTransform SpawnTransform;  // 生成变换（位置、旋转、缩放），目前是默认构造，即全零/单位变换，还没有赋值——这里通常需要用角色的枪口/朝向来填充，否则投射物会生成在世界原点
		SpawnTransform.SetLocation(SocketLocation);
		SpawnTransform.SetRotation(Rotation.Quaternion());
		//TODO 设置投射物的旋转

		//(延迟生成)不会执行 OnConstruction,不会注册组件,不会调用 BeginPlay
		AAuraProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(
			ProjectileClass,                                      // 要生成的投射物类（蓝图或 C++ 类），来自本 GameplayAbility 的属性
			SpawnTransform,                                        // 生成位置/朝向
			GetOwningActorFromActorInfo(),                         // Owner：技能拥有者（一般是 PlayerState 或角色本身），供投射物内部逻辑访问施法者信息
			Cast<APawn>(GetAvatarActorFromActorInfo()),            // Instigator：造成此次生成的 Pawn，常用于伤害归因（如判断击杀者）
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn        // 碰撞处理策略：即使生成位置与其他物体重叠，也强制生成，不做位置调整或取消
		);

		//代办事项 ...
		//1.给予投射物一个造成伤害的效果规格
		UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo());
		FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), SourceASC->MakeEffectContext());
		Projectile->DamageEffectSpecHandle = SpecHandle;

		Projectile->FinishSpawning(SpawnTransform);
	}
}
