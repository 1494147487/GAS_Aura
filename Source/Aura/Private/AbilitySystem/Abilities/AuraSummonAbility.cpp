


#include "AbilitySystem/Abilities/AuraSummonAbility.h"
#include "Kismet/KismetSystemLibrary.h"

TArray<FVector> UAuraSummonAbility::GetSpawnLocations()
{
	const FVector Forward = GetAvatarActorFromActorInfo()->GetActorForwardVector();
	const FVector Location = GetAvatarActorFromActorInfo()->GetActorLocation();

	const float DeltaSpread = SpawnSpread / (NumMinions); //计算出每个小兵之间的角度间隔

	//计算出左右扩散的方向向量													FVector::UpVector是(0,0,1)向上方向，作用是让Forward向量绕着UpVector旋转一定角度，得到左右扩散的方向
	const FVector RightOfSpread = Forward.RotateAngleAxis(SpawnSpread / 2.f, FVector::UpVector);
	const FVector LeftOfSpread = Forward.RotateAngleAxis(-SpawnSpread / 2.f, FVector::UpVector);
	TArray<FVector> SpawnLocations;

	for (int32 i = 0; i < NumMinions; i++)
	{
		const FVector Direction = LeftOfSpread.RotateAngleAxis(DeltaSpread * i, FVector::UpVector);
		FVector ChosenSpawnLocation = Location + Direction * FMath::RandRange(MinSpawnDistance, MaxSpawnDistance);

		FHitResult Hit;
		//作用是从ChosenSpawnLocation的上方400单位位置向下400单位位置发射一条射线，检测是否有碰撞物体，如果有碰撞，则Hit会包含碰撞信息
		GetWorld()->LineTraceSingleByChannel(Hit, ChosenSpawnLocation + FVector(0.f,0.f,400.f), ChosenSpawnLocation - FVector(0.f, 0.f, 400.f), ECC_Visibility);
		if(Hit.bBlockingHit)
		{
			ChosenSpawnLocation = Hit.ImpactPoint; //如果有碰撞，则将ChosenSpawnLocation设置为碰撞点的位置，这样小兵就会生成在地面上，而不是悬空
		}

		SpawnLocations.Add(ChosenSpawnLocation); 
		/*DrawDebugSphere(GetWorld(), ChosenSpawnLocation, 18.f, 12, FColor::Green, false, 3.f);
		UKismetSystemLibrary::DrawDebugArrow(GetAvatarActorFromActorInfo(), Location, Location + Direction * MaxSpawnDistance, 4.f, FLinearColor::Green);
		DrawDebugSphere(GetWorld(), Location + Direction * MinSpawnDistance, 5.f, 12, FColor::Red, false, 3.f);
		DrawDebugSphere(GetWorld(), Location + Direction * MaxSpawnDistance, 5.f, 12, FColor::Red, false, 3.f);*/

	}

	return SpawnLocations;
}

TSubclassOf<APawn> UAuraSummonAbility::GetRandomMinionClass() const
{
	const int32 SelectedIndex = FMath::RandRange(0, MinionClasses.Num() - 1);
	return MinionClasses[SelectedIndex];
}
