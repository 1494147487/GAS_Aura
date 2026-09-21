

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AuraWidgetController.generated.h"

/**
 * 
 */
class UAbilitySystemComponent;
class UAttributeSet;
class UAuraAttributeSet;

USTRUCT(BlueprintType)//把一个普通C++结构体"注册"进UE反射系统
struct FWidgetControllerParams
{
	GENERATED_BODY()

	FWidgetControllerParams() {}
	FWidgetControllerParams(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS):
	PlayerController(PC),PlayerState(PS),AbilitySystemComponent(ASC),AttributeSet(AS){}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)//可以在编辑器细节面板里被手动编辑 允许蓝图读取和修改这个字段的值
	TObjectPtr<APlayerController> PlayerController = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<APlayerState> PlayerState = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAttributeSet> AttributeSet = nullptr;
};


UCLASS()
class AURA_API UAuraWidgetController : public UObject
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void SetWidgetControllerParams(const FWidgetControllerParams& Params);//把传进来的结构体解包，分别存进自己的四个成员变量里：
	
	UFUNCTION(BlueprintCallable)
	virtual void BroadcastInitialValues();//虚函数，子类可以重写它来广播初始值

	virtual void BindCallbacksToDependencies();//虚函数，子类可以重写它来绑定回调函数到依赖的组件上

protected:

	UPROPERTY(BlueprintReadOnly,Category = "WidgetController")
	TObjectPtr<APlayerController> PlayerController;//可能UI需要读取玩家特有的状态（比如PlayerState上存的等级、经验）

	UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
	TObjectPtr<APlayerState> PlayerState;

	UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;//GAS的核心组件，技能、效果、属性的变化都通过它

	UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
	TObjectPtr<UAttributeSet> AttributeSet;//具体存血量、法力等数值属性的地方
};
