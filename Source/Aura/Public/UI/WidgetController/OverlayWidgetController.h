

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "OverlayWidgetController.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthChangedSignature, float, NewHealth);//声明一个可绑定的委托，允许蓝图或其他C++类订阅生命值变化事件
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMaxHealthChangedSignature, float, NewMaxHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnManaChangedSignature, float, NewMana);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMaxManaChangedSignature, float, NewMaxMana);

UCLASS(BlueprintType, Blueprintable)//声明一个可以在蓝图中使用的类，允许蓝图继承和实例化它
class AURA_API UOverlayWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()

public:
	virtual void BroadcastInitialValues() override;
	virtual void BindCallbacksToDependencies() override;//重写父类的虚函数，绑定回调函数到依赖的组件上

	UPROPERTY(BlueprintAssignable, Category = "GAS|Attribute")//声明一个可绑定的委托，允许蓝图或其他C++类订阅生命值变化事件
	FOnHealthChangedSignature OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Attribute")//声明一个可绑定的委托，允许蓝图或其他C++类订阅生命值变化事件
	FOnMaxHealthChangedSignature OnMaxHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Attribute")//声明一个可绑定的委托，允许蓝图或其他C++类订阅生命值变化事件
	FOnManaChangedSignature OnManaChanged;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Attribute")//声明一个可绑定的委托，允许蓝图或其他C++类订阅生命值变化事件
	FOnMaxManaChangedSignature OnMaxManaChanged;

protected:


	void HealthChanged(const FOnAttributeChangeData& Data)const;//声明一个函数，用于处理生命值变化事件
	void MaxHealthChanged(const FOnAttributeChangeData& Data)const;//声明一个函数，用于处理最大生命值变化事件
	void ManaChanged(const FOnAttributeChangeData& Data)const;//声明一个函数，用于处理法力值变化事件
	void MaxManaChanged(const FOnAttributeChangeData& Data)const;//声明一个函数，用于处理最大法力值变化事件
};
