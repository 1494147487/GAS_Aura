
#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "AuraAbilityTypes.generated.h"


USTRUCT(BlueprintType)
struct FAuraGameplayEffectContext : public FGameplayEffectContext
{
	GENERATED_BODY()

public:
	bool IsCriticalHit() const { return bIsCriticalHit; }
	bool IsBlockedHit() const { return bIsBlockedHit; }

	void SetIsCriticalHit(bool bInIsCriticalHit) { bIsCriticalHit = bInIsCriticalHit; }
	void SetIsBlockedHit(bool bInIsBlockedHit) { bIsBlockedHit = bInIsBlockedHit; }

	//返回用于序列化的实际结构体，子类必须重写这个函数！
	virtual UScriptStruct* GetScriptStruct() const
	{
		return StaticStruct();
	} 

	//正确地深拷贝出一份完整独立的 FGameplayEffectContext(或其子类)副本,同时解决两个问题——通过虚函数机制避免多态对象被切片(拷贝出正确的实际类型),
	// 并对 HitResult 做深拷贝(而不是共享同一块内存),确保返回的新对象和原对象完全独立、互不影响
	virtual FAuraGameplayEffectContext* Duplicate() const
	{
		FAuraGameplayEffectContext* NewContext = new FAuraGameplayEffectContext();
		*NewContext = *this;
		if (GetHitResult())
		{
			// Does a deep copy of the hit result
			NewContext->AddHitResult(*GetHitResult(), true);
		}
		return NewContext;
	}


	/** 自定义序列化，子类必须重写这个函数 */
	//FArchive& Ar 序列化时往里面写数据，反序列化时从里面读数据
	//class UPackageMap* Map：网络序列化时，用来把Actor指针这类"每台机器上内存地址不一样的东西"转换成一个双方都认可的统一标识符（比如网络GUID），这样才能正确地把一个Actor引用从服务器同步到客户端。
	//bool& bOutSuccess：一个输出参数，函数执行完之后，通过这个引用告诉调用者"这次序列化/反序列化到底成功了没有"。
	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);

	


protected:

	UPROPERTY()
	bool bIsBlockedHit = false;

	UPROPERTY()
	bool bIsCriticalHit = false;
};


template<>//// 为FGameplayEffectContext特化类型萃取，声明它的自定义序列化/拷贝行为，覆盖引擎默认处理方式
struct TStructOpsTypeTraits< FAuraGameplayEffectContext > : public TStructOpsTypeTraitsBase2< FAuraGameplayEffectContext >
{
	enum
	{
		WithNetSerializer = true,	// 使用自定义的NetSerialize，而不是默认反射序列化
		WithCopy = true		// 禁止memcpy浅拷贝，保证TSharedPtr<FHitResult>的引用计数正确，避免悬空指针
	};
};
