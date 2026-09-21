

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "TargetDataUnderMouse.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMouseTargetDataSignature, const FGameplayAbilityTargetDataHandle&, DataHandle);
/**
 * 
 */
UCLASS()
class AURA_API UTargetDataUnderMouse : public UAbilityTask
{
	GENERATED_BODY()
	
public:															
																	//// 蓝图节点上显示的名字(替代真实函数名 CreateTargetDataUnderMouse)
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (DisplayName = "TargetDataUnderMouse", 
		HidePin = "OwningAbility", //// 隐藏 OwningAbility 参数的输入引脚,不让用户手动连线
		DefaultToSelf = "OwningAbility",  // 隐藏的这个参数自动填为"调用者自身"(当前技能蓝图实例)
		BlueprintInternalUseOnly = "true")) // 这个函数本身不出现在蓝图节点搜索里,只作为异步任务节点的内部实现
	static UTargetDataUnderMouse* CreateTargetDataUnderMouse(UGameplayAbility* OwningAbility); 
	//static:静态成员函数,不需要先有一个 UTargetDataUnderMouse 实例才能调用

	UPROPERTY(BlueprintAssignable)//声明这个 AbilityTask 类对外暴露的委托(输出引脚)
	FMouseTargetDataSignature ValidData;

private:
	virtual void Activate()override;

	void SendMouseCursorData();

	void OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& DataHandle, FGameplayTag ActivationTag);
};
