
#include "Player/AuraPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Interaction/EnemyInterface.h"

AAuraPlayerController::AAuraPlayerController()
{
	bReplicates = true;
}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	CursorTrace();
}

void AAuraPlayerController::CursorTrace()
{
	FHitResult CursorResult;
	
	// 从摄像机穿过鼠标光标位置发射一条射线，检测鼠标指向的物体
	// 参数1：ECC_Visibility，射线只检测【可见性碰撞通道】的物体
	// 参数2：false，关闭精细网格检测，使用简单碰撞盒检测(性能更高)
	// 参数3：CursorResult，输出参数(引用)，射线命中信息会存入该变量
	GetHitResultUnderCursor(ECC_Visibility, false, CursorResult);//`GetHitResultUnderCursor`：从鼠标光标发射射线，检测鼠标指向的物体，并把命中信息存入 CursorResult

	if (!CursorResult.bBlockingHit)return;

	LastActor = ThisActor;
	ThisActor = CursorResult.GetActor();

	/**
 * 从鼠标光标发射射线。分为以下几种情况：
 * A. LastActor 为空，并且 ThisActor 也为空
 *    - 不做任何操作
 * B. LastActor 为空，但 ThisActor 有效
 *    - 高亮 ThisActor
 * C. LastActor 有效，但 ThisActor 为空
 *    - 取消高亮 LastActor
 * D. 两个 Actor 都有效，但 LastActor 与 ThisActor 不相同
 *    - 取消高亮 LastActor，并高亮 ThisActor
 * E. 两个 Actor 都有效，并且是同一个 Actor
 *    - 不做任何操作
 */
	
	if (LastActor == nullptr)
	{
		if(ThisActor != nullptr)
		{
			//B
			ThisActor->HighlightActor();
		}
		else
		{
			//A
		}
	}
	else
	{
		if(ThisActor == nullptr)
		{
			//C
			LastActor->UnHighlightActor();
		}
		else
		{
			if(LastActor != ThisActor)
			{
				//D
				LastActor->UnHighlightActor();
				ThisActor->HighlightActor();
			}
			else
			{
				//E
			}
		}
	}
	
}


void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay();

	check(AuraContext);

	// 从本地玩家身上获取【增强输入子系统】，用来管理按键映射上下文
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());

	if (Subsystem)
	{
		// 将我们的按键配置表(AuraContext)启用，优先级0(数值越大优先级越高)
		Subsystem->AddMappingContext(AuraContext, 0);
	}
	
	
	// 显示鼠标光标
	bShowMouseCursor = true;
	// 设置鼠标图标为系统默认样式
	DefaultMouseCursor = EMouseCursor::Default;

	// 创建一个「游戏+UI」混合输入模式的数据结构体
	FInputModeGameAndUI InputModeData;
	// 设置鼠标锁定规则：不要把鼠标锁死在游戏窗口内
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	// 设置鼠标被点击捕获的时候，不要隐藏光标
	InputModeData.SetHideCursorDuringCapture(false);
	// 将上面配置好的输入模式应用到玩家控制器，正式生效
	SetInputMode(InputModeData);
}

void AAuraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// 将原生的InputComponent强制转换为【增强输入组件】
	// CastChecked：转换失败直接崩溃报错，方便快速定位问题
	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);

	// 绑定输入动作：当 MoveAction 被按下触发时，执行 Move 函数
	// 参数1：要监听的输入动作资源(MoveAction)
	// 参数2：触发时机 Triggered = 按键按下/持续按住的时候触发
	// 参数3：回调函数所属对象（当前玩家控制器 this）
	// 参数4：被调用的成员函数地址
	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);
}

void AAuraPlayerController::Move(const FInputActionValue& InputActionValue)
{
	// 从增强输入的值中，读取二维向量(W/S对应Y，A/D对应X)
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();

	// 获取玩家控制器（相机）当前的旋转角度
	const FRotator Rotation = GetControlRotation();

	// 新建一个旋转，只保留水平旋转Yaw，清零Pitch(抬头低头)和Roll(翻滚)
	// 作用：移动永远在地面，不会跟着镜头朝上飞
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

	// 根据水平旋转，算出世界坐标系下相机的【正前方】单位方向向量
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

	// 根据水平旋转，算出世界坐标系下相机的【正右方】单位方向向量
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	// 安全获取当前控制器所操控的角色，判空避免空指针崩溃
	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		// 前后移动：W/S，沿着相机前方施加移动输入
		ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y);
		// 左右移动：A/D，沿着相机右方施加移动输入
		ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.X);
	}


}

