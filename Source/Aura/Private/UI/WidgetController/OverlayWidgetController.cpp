


#include "UI/WidgetController/OverlayWidgetController.h"
#include "AbilitySystem/AuraAttributeSet.h"

void UOverlayWidgetController::BroadcastInitialValues()
{
	
	const UAuraAttributeSet* AuraAttributeSet = Cast<UAuraAttributeSet>(AttributeSet);
	OnHealthChanged.Broadcast(AuraAttributeSet->GetHealth());//广播当前的生命值给所有订阅了OnHealthChanged事件的监听者
	OnMaxHealthChanged.Broadcast(AuraAttributeSet->GetMaxHealth());
	OnManaChanged.Broadcast(AuraAttributeSet->GetMana());
	OnMaxManaChanged.Broadcast(AuraAttributeSet->GetMaxMana());

}

void UOverlayWidgetController::BindCallbacksToDependencies()//重写父类的虚函数，绑定回调函数到依赖的组件上  本质上也是个初始化函数，绑定回调函数到依赖的组件上
{

	//拿到 ASC     用 Health 的属性句柄取出「Health 变化委托」  把 this->HealthChanged 加进该委托的监听列表
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
													//this：回调时用的实例（这个 OverlayWidgetController）
		UAuraAttributeSet::GetHealthAttribute()).AddUObject(this, &UOverlayWidgetController::HealthChanged);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		UAuraAttributeSet::GetMaxHealthAttribute()).AddUObject(this, &UOverlayWidgetController::MaxHealthChanged);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		UAuraAttributeSet::GetManaAttribute()).AddUObject(this, &UOverlayWidgetController::ManaChanged);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		UAuraAttributeSet::GetMaxManaAttribute()).AddUObject(this, &UOverlayWidgetController::MaxManaChanged);


}

void UOverlayWidgetController::HealthChanged(const FOnAttributeChangeData& Data)const
{
	
	OnHealthChanged.Broadcast(Data.NewValue);//广播当前的生命值给所有订阅了OnHealthChanged事件的监听者
}

void UOverlayWidgetController::MaxHealthChanged(const FOnAttributeChangeData& Data)const
{
	OnMaxHealthChanged.Broadcast(Data.NewValue);
}

void UOverlayWidgetController::ManaChanged(const FOnAttributeChangeData& Data) const
{
	OnManaChanged.Broadcast(Data.NewValue);
}

void UOverlayWidgetController::MaxManaChanged(const FOnAttributeChangeData& Data) const
{
	OnMaxManaChanged.Broadcast(Data.NewValue);
}
 