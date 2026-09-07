


#include "UI/HUD/AuraHUD.h"
#include "UI/Widgets/AuraUserWidget.h"
#include "UI/WidgetController/OverlayWidgetController.h"

UOverlayWidgetController* AAuraHUD::GetOverlayWidgetController(const FWidgetControllerParams& WCParams)
{
	if(OverlayWidgetController == nullptr)
	{
		OverlayWidgetController = NewObject<UOverlayWidgetController>(this,OverlayWidgetControllerClass);//
		OverlayWidgetController->SetWidgetControllerParams(WCParams);
		OverlayWidgetController->BindCallbacksToDependencies();//绑定回调函数到依赖的组件上
	}

	return OverlayWidgetController;
}

void AAuraHUD::InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS)
{
	checkf(OverlayWidgetClass, TEXT("Overlay Widget Class uninitialized, please fill out BP_AuraHUD"));
	checkf(OverlayWidgetControllerClass, TEXT("Overlay Widget Controller Class uninitialized, please fill out BP_AuraHUD"));

	UUserWidget* Widget = CreateWidget<UUserWidget>(GetWorld(), OverlayWidgetClass);//
	OverlayWidget = Cast<UAuraUserWidget>(Widget);

	const FWidgetControllerParams WidhetControllerParams(PC,PS,ASC,AS);
	UOverlayWidgetController* WidgetController = GetOverlayWidgetController(WidhetControllerParams);//如果OverlayWidgetController还没创建，就创建它，并把参数传进去

	OverlayWidget->SetWidgetController(WidgetController);//把OverlayWidgetController传给OverlayWidget，让它知道自己要控制的是什么 

	WidgetController->BroadcastInitialValues();//广播初始值，让OverlayWidgetController把当前的血量、法力等属性值传给OverlayWidget
	

	OverlayWidget->AddToViewport();
}


