

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayEffectTypes.h"
#include "AuraProjectile.generated.h"


class USphereComponent;
class UProjectileMovementComponent;
class UNiagaraSystem;

UCLASS()
class AURA_API AAuraProjectile : public AActor
{
	GENERATED_BODY()

public:

	AAuraProjectile();

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn = true))
	FGameplayEffectSpecHandle DamageEffectSpecHandle;


protected:

	virtual void BeginPlay() override;
	virtual void Destroyed() override;

	/**
 * 球体碰撞组件重叠开始回调
 * @param OverlappedComponent    触发重叠的组件，也就是投射物自身的SphereCollision球体碰撞组件
 * @param OtherActor             和本投射物发生重叠的外部Actor对象
 * @param OtherComp              OtherActor身上，实际产生重叠的那个碰撞组件
 * @param OtherBodyIndex         碰撞目标的Body索引；同一个Actor有多个碰撞体时，用来区分是哪一个碰撞体发生重叠
 * @param bFromSweep             是否由移动扫检测产生的重叠；投射物飞行碰撞该值一般为true，静态相交为false
 * @param SweepResult            碰撞扫查结果结构体，保存碰撞点坐标、碰撞法线、碰撞物体等详细碰撞信息
 */
	UFUNCTION()
	void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, 
		AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);


private:
	bool bHit = false;
	UPROPERTY(EditDefaultsOnly)
	float LifeSpan = 15.f;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> Sphere;

	UPROPERTY(EditAnyWhere)
	TObjectPtr<UNiagaraSystem> ImpactEffect;
	UPROPERTY(EditAnyWhere)
	TObjectPtr<USoundBase> ImpactSound;

	UPROPERTY(EditAnyWhere)
	TObjectPtr<USoundBase> LoopingSound;

	UPROPERTY()
	TObjectPtr<UAudioComponent>LoopingSoundComponent;
};