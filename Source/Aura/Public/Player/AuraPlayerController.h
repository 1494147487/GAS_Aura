

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "AuraPlayerController.generated.h"

/**
 * 
 */

class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class IEnemyInterface;

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

	// 移动的回调函数；当按下绑定好WASD按键时引擎自动调用
	void Move(const FInputActionValue& InputActionValue);

	void CursorTrace();

	TScriptInterface<IEnemyInterface> LastActor;//`TScriptInterface<IEnemyInterface>`：类型，只能存储**实现了 IEnemyInterface 接口**的对象
	TScriptInterface<IEnemyInterface> ThisActor;
};
