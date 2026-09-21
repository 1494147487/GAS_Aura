

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "AbilitySystem/Data/AttributeInfo.h"
#include "AttributeMenuWidgetController.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAttributeInfoSignature, const FAuraAttributeInfo&, Info);
struct FGameplayAttribute;

UCLASS(Blueprintable,Blueprintable)
class AURA_API UAttributeMenuWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()

public:
	virtual void BroadcastInitialValues()override;//虚函数，子类可以重写它来广播初始值

	virtual void BindCallbacksToDependencies()override;//虚函数，子类可以重写它来绑定回调函数到依赖的组件上

	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FAttributeInfoSignature AttributeInfoDelegate;

protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAttributeInfo> AttributeInfo;//在继承的属性菜单控制器蓝图类里会配置好这个资产数据
	
private:
	
	void BroadcastAttributeInfo(const FGameplayTag& AttributeTag, const FGameplayAttribute& Attribute);
};
