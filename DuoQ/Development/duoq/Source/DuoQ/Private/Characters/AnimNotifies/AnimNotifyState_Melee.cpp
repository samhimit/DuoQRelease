// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/AnimNotifies/AnimNotifyState_Melee.h"
#include "Kismet/KismetSystemLibrary.h"
#include "DuoQBlueprintFunctionLibrary.h"
#include "Characters/Partner/StarlightCharacter.h"

void UAnimNotifyState_Melee::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	USceneComponent* Scene = MeshComp->GetChildComponent(1);
	if (USkeletalMeshComponent* SMC = Cast<USkeletalMeshComponent>(Scene))
	{
		GunMesh = SMC;
	}
}

void UAnimNotifyState_Melee::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	// reset the damaged enemies array
	AStarlightCharacter* Starlight = UDuoQBlueprintFunctionLibrary::GetStarlightCharacter(MeshComp->GetOwner()->GetWorld());
	Starlight->DamagedEnemies.Empty();
}

void UAnimNotifyState_Melee::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	// start tracing the melee attack
	FHitResult							  HitResult;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypesArray;
	ObjectTypesArray.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Visibility));
	TArray<AActor*> IgnoredActors;
	IgnoredActors.Add(MeshComp->GetOwner());

	FVector Start;
	FVector End;
	if (GunMesh)
	{
		Start = GunMesh->GetSocketLocation(FName(TEXT("GunRear")));
		End = GunMesh->GetSocketLocation(FName(TEXT("Muzzle")));
	}

	// start the sphere trace
	UKismetSystemLibrary::SphereTraceSingle(MeshComp->GetOwner()->GetWorld(), Start, End, 20.0f, UEngineTypes::ConvertToTraceType(ECC_Visibility), false, IgnoredActors,
		EDrawDebugTrace::ForDuration, HitResult, true);

	AStarlightCharacter* Starlight = UDuoQBlueprintFunctionLibrary::GetStarlightCharacter(MeshComp->GetOwner()->GetWorld());

	// this check prevents an enemy from being hit multiple times during a melee attack
	if (!(Starlight->DamagedEnemies.Contains(HitResult.GetActor())))
	{
		Starlight->DamagedEnemies.AddUnique(HitResult.GetActor());

		// needs to add damage logic here
	}
}
