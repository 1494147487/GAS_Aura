


#include "AbilitySystem/AbilityTasks/TargetDataUnderMouse.h"
#include "AbilitySystemComponent.h"


// 调用 NewAbilityTask 模板函数创建一个 Task 实例并返回
UTargetDataUnderMouse* UTargetDataUnderMouse::CreateTargetDataUnderMouse(UGameplayAbility* OwningAbility)
{
	UTargetDataUnderMouse* MyObj = NewAbilityTask<UTargetDataUnderMouse>(OwningAbility);//

	return MyObj;
}
//→ 调用你的静态工厂函数 CreateTargetDataUnderMouse() 拿到 Task 对象
//→ 编译器自动生成的胶水代码接着调用 MyObj->ReadyForActivation()
//→ ReadyForActivation() 内部再调用你重写的 Activate()
void UTargetDataUnderMouse::Activate()
{

	const bool bIsLocallyControlled = Ability->GetCurrentActorInfo()->IsLocallyControlled();//
	if (bIsLocallyControlled)
	{
		SendMouseCursorData();
	}
	else
	{
		FGameplayAbilitySpecHandle SpecHandle = GetAbilitySpecHandle();//这是哪个技能
		FPredictionKey ActivationPredictionKey = GetActivationPredictionKey();//这个技能第几次释放

		AbilitySystemComponent.Get()->AbilityTargetDataSetDelegate(SpecHandle, ActivationPredictionKey).AddUObject(
			this, //// 整句话:找到/创建这个技能这次释放专属的委托,把 OnTargetDataReplicatedCallback 挂上去,
			&UTargetDataUnderMouse::OnTargetDataReplicatedCallback);

		//:检查"客户端的数据是不是已经先到达了   如果数据已经在了,这个函数内部会立刻触发刚绑定好的委托
		const bool bCalledDelegate = AbilitySystemComponent.Get()->CallReplicatedTargetDataDelegatesIfSet(SpecHandle, ActivationPredictionKey);
		if (!bCalledDelegate)
		{
			SetWaitingOnRemotePlayerData();
		}
	}

	
}

void UTargetDataUnderMouse::SendMouseCursorData()
{
	FScopedPredictionWindow ScopedPrediction(AbilitySystemComponent.Get());//:在这个变量存在的生命周期内,临时打开一个"预测窗口",告诉系统"接下来发生的这些操作,是属于客户端预测的一部分"。

	APlayerController* PC = Ability->GetCurrentActorInfo()->PlayerController.Get();
	FHitResult  CursorResult;
	PC->GetHitResultUnderCursor(ECC_Visibility, false, CursorResult);

	//ValidData.Broadcast(CursorResult.Location);
	FGameplayAbilityTargetDataHandle DataHandle;  // 声明一个空的目标数据句柄,用来装后面构造出的具体目标数据
	FGameplayAbilityTargetData_SingleTargetHit* Data = new FGameplayAbilityTargetData_SingleTargetHit();  // 在堆上 new 一个"单体命中"类型的目标数据,所有权后面会交给 Handle 托管
	Data->HitResult = CursorResult;  // 把之前射线检测得到的命中结果存进这份目标数据里
	DataHandle.Add(Data);  // 把这份数据塞进 Handle 容器(内部用智能指针托管生命周期,可以装多份)


	AbilitySystemComponent->ServerSetReplicatedTargetData(//客户端调用这个函数,把目标数据发给服务器
		GetAbilitySpecHandle(),                       // 标识这份数据属于哪个技能实例,服务器靠它找到对应技能处理  GetAbilitySpecHandle() 就是从当前正在执行的这个 UGameplayAbility 实例上,取出它自己
		GetActivationPredictionKey(),                 // 这次技能激活对应的预测键,用于客户端预测系统关联本次调用  "这是第几次释放"
		DataHandle,                                    // 真正要发送的目标数据本体  Handle兼容
		FGameplayTag(),                                // 应用标签,这里传空表示不需要
		AbilitySystemComponent->ScopedPredictionKey    // ← 这里用到的就是 ScopedPrediction 构造时设置好的这个键
	);  // 调用 Server RPC,把打包好的目标数据整体发送给服务器

	if (ShouldBroadcastAbilityTaskDelegates())
	{
		ValidData.Broadcast(DataHandle);
	}

}

void UTargetDataUnderMouse::OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& DataHandle, FGameplayTag ActivationTag)
{
	//告诉 ASC"这份数据我已经用过了,可以从内部缓存里清掉了
	AbilitySystemComponent->ConsumeClientReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey());
	if (ShouldBroadcastAbilityTaskDelegates())//是基类提供的一个安全检查(比如确认 Ability 还没被取消/结束,避免对着一个已经失效的 Ability 广播)
	{
		ValidData.Broadcast(DataHandle);
	}
}