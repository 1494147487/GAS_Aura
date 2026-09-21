
#include "Player/AuraPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Interaction/EnemyInterface.h"
#include <Input/AuraInputComponent.h>
#include "AbilitySystem/AuraAbilitySystemComponent.h" 
#include "Components/SplineComponent.h"
#include "AuraGameplayTags.h"
#include <AbilitySystemBlueprintLibrary.h>
#include "NavigationPath.h"
#include <NavigationSystem.h>

AAuraPlayerController::AAuraPlayerController()
{
	bReplicates = true;

	Spline = CreateDefaultSubobject<USplineComponent>("Spline");
}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	CursorTrace();

	AutoRun();
}

void AAuraPlayerController::AutoRun()
{
	if (!bAutoRunning)return;
	if (APawn* ControlledPawn = GetPawn())
	{	//这个函数做的事情是一次几何投影:在整条 Spline 曲线上,
		//找到离 ControlledPawn->GetActorLocation()(角色当前世界坐标)距离最短的那一个点,返回它的世界坐标,存进 LocationOnSpline。
		const FVector LocationOnSpline = Spline->FindLocationClosestToWorldLocation(ControlledPawn->GetActorLocation(), ESplineCoordinateSpace::World);
		// 在刚才求出的投影点位置，计算 Spline 在该处的切线方向（已归一化），
		// 也就是"如果沿着这条路径继续往前走，应该朝哪个方向"
		const FVector Direction = Spline->FindDirectionClosestToWorldLocation(LocationOnSpline, ESplineCoordinateSpace::World);
		ControlledPawn->AddMovementInput(Direction);

		const float DistanceToDestination = (LocationOnSpline - CachedDestination).Length();
		if (DistanceToDestination <= AutoRunAcceptanceRadius)
		{
			bAutoRunning = false;
		}

	}
}

void AAuraPlayerController::CursorTrace()
{
	
	
	// 从摄像机穿过鼠标光标位置发射一条射线，检测鼠标指向的物体
	// 参数1：ECC_Visibility，射线只检测【可见性碰撞通道】的物体
	// 参数2：false，关闭精细网格检测，使用简单碰撞盒检测(性能更高)
	// 参数3：CursorResult，输出参数(引用)，射线命中信息会存入该变量
	GetHitResultUnderCursor(ECC_Visibility, false, CursorResult);//`GetHitResultUnderCursor`：从鼠标光标发射射线，检测鼠标指向的物体，并把命中信息存入 CursorResult

	if (!CursorResult.bBlockingHit)return;

	LastActor = ThisActor;
	ThisActor = Cast<IEnemyInterface>(CursorResult.GetActor());

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
	
	if (ThisActor != LastActor)
	{
		if (LastActor) { LastActor->UnHighlightActor(); }
		if (ThisActor) { ThisActor->HighlightActor(); }
	}
	
}

void AAuraPlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
	{
		bTargeting = ThisActor ? true : false;
		bAutoRunning = false;
	}

}

void AAuraPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (!InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
	{
		if (GetASC())
		{
			GetASC()->AbilityInputTagReleased(InputTag);
		}
		return;
	}

	if (GetASC())GetASC()->AbilityInputTagReleased(InputTag);

	if (!bTargeting && !bShiftkeyDown)
	{
		const APawn* ControlledPawn = GetPawn();
		if (FollowTime <= ShortPressThreshold && ControlledPawn)
		{	// 以 PlayerController(this)为世界上下文,同步计算一条从角色当前位置到鼠标点击点的导航路径
			if (UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(//
				this, ControlledPawn->GetActorLocation(), CachedDestination))
			{
				Spline->ClearSplinePoints();

				for (const FVector& PointLoc : NavPath->PathPoints)
				{
					Spline->AddSplinePoint(PointLoc, ESplineCoordinateSpace::World);
					// 在PointLoc处画一个半径8、绿色、不常驻、显示5秒的调试球体，用于可视化路径点
					//DrawDebugSphere(GetWorld(), PointLoc, 8.f, 8, FColor::Green,false,5.f);
				}
				//这一行是把"鼠标点的位置"替换成"导航系统认可的可达位置"。
				CachedDestination = NavPath->PathPoints[NavPath->PathPoints.Num() - 1];

				bAutoRunning = true;
			}

		}

		FollowTime = 0.f;
		bTargeting = false;
	}

}

void AAuraPlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{
	if (!InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
	{
		if (GetASC())
		{
			GetASC()->AbilityInputTagHeld(InputTag);
		}
		return;
	}

	if (bTargeting || bShiftkeyDown)
	{
		if (GetASC())
		{
			GetASC()->AbilityInputTagHeld(InputTag);
		}
	}
	else
	{
		FollowTime += GetWorld()->GetDeltaSeconds();
		
		if (CursorResult.bBlockingHit)
		{
			CachedDestination = CursorResult.ImpactPoint;
		}

		if (APawn* ControlledPawn = GetPawn())
		{
			const FVector WorldDirection = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
			ControlledPawn->AddMovementInput(WorldDirection);

		}
	}
	
}


UAuraAbilitySystemComponent* AAuraPlayerController::GetASC()
{
	if (AuraAbilitySystemComnponent == nullptr)
	{
		AuraAbilitySystemComnponent = Cast<UAuraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn<APawn>()));
	}

	return AuraAbilitySystemComnponent;
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

//// 本地控制的 PlayerController 关联 Pawn 并初始化 InputComponent 后由引擎自动调用一次,在此完成 Enhanced Input 与 GAS 技能的绑定
void AAuraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// 将原生的InputComponent强制转换为【增强输入组件】
	// CastChecked：转换失败直接崩溃报错，方便快速定位问题
	//UEnhancedInputComponent 是 Enhanced Input 系统专用的输入组件,专门负责"把 Input Action 和具体的响应函数关联起来"这件事
	UAuraInputComponent* AuraInputComponent = CastChecked<UAuraInputComponent>(InputComponent);

	// 绑定输入动作：当 MoveAction 被按下触发时，执行 Move 函数
	// 参数1：要监听的输入动作资源(MoveAction)
	// 参数2：触发时机 Triggered = 按键按下/持续按住的时候触发
	// 参数3：回调函数所属对象（当前玩家控制器 this）
	// 参数4：被调用的成员函数地址
	AuraInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);
	//以后只要 MoveAction 这个动作进入 Triggered 阶段(也就是玩家正按着移动键), 就去调用 this(当前这个 PlayerController 实例)的 Move 这个成员函数。"
	AuraInputComponent->BindAction(ShiftAction, ETriggerEvent::Started, this, &AAuraPlayerController::ShiftPressed);
	AuraInputComponent->BindAction(ShiftAction, ETriggerEvent::Completed, this, &AAuraPlayerController::ShiftReleased);
	AuraInputComponent->BindAbilityActions(InputConfig, this, &ThisClass::AbilityInputTagPressed, &ThisClass::AbilityInputTagReleased, &ThisClass::AbilityInputTagHeld);
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

