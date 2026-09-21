

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include <GameplayTagContainer.h>
#include "AuraPlayerController.generated.h"


/**
 * 
 */

class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class IEnemyInterface;
class UAuraInputConfig;
class UAuraAbilitySystemComponent;
class USplineComponent;

UCLASS()
class AURA_API AAuraPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AAuraPlayerController();

	virtual void PlayerTick(float DeltaTime)override;//`PlayerTick` 是 **APlayerController** 自带的帧回调函数

protected:
	virtual void BeginPlay()override;
	virtual void SetupInputComponent()override;//`SetupInputComponent()` 是引擎自动调用的函数，用来**绑定按键?函数的回调**；

private:
	// 可在编辑器细节面板修改，归类到"Input"分组
	UPROPERTY(EditAnywhere, Category = "Input")
	// 保存按键映射配置表资源(W/S/A/D绑定给MoveAction)
	TObjectPtr<UInputMappingContext>AuraContext;

	// 可在编辑器细节面板修改，归类到"Input"分组
	UPROPERTY(EditAnywhere, Category = "Input")
	// 保存移动输入动作资源，专门用来触发移动逻辑
	TObjectPtr<UInputAction>MoveAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction>ShiftAction;

	bool bShiftkeyDown = false;
	void ShiftPressed() { bShiftkeyDown = true; }
	void ShiftReleased() { bShiftkeyDown = false; }

	// 移动的回调函数；当按下绑定好WASD按键时引擎自动调用
	void Move(const FInputActionValue& InputActionValue);

	void CursorTrace();

	IEnemyInterface* LastActor;//`TScriptInterface<IEnemyInterface>`：类型，只能存储**实现了 IEnemyInterface 接口**的对象
	IEnemyInterface* ThisActor;
	FHitResult CursorResult;

	void AbilityInputTagPressed(FGameplayTag InputTag);
	void AbilityInputTagReleased(FGameplayTag InputTag);
	void AbilityInputTagHeld(FGameplayTag InputTag);



	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAuraInputConfig> InputConfig;//在ue中设置好对应的数据资产

	UPROPERTY()
	TObjectPtr<UAuraAbilitySystemComponent> AuraAbilitySystemComnponent;

	UAuraAbilitySystemComponent* GetASC();


	FVector CachedDestination = FVector::ZeroVector;//光标点击的位置

	float FollowTime = 0.f;
	float ShortPressThreshold = 0.5f;
	bool bAutoRunning = false;
	bool bTargeting = false;


	UPROPERTY(EditDefaultsOnly)
	float AutoRunAcceptanceRadius = 50.f;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USplineComponent> Spline;

	void AutoRun();
};
