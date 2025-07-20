// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Components/WidgetComponent.h"
#include "Components/SphereComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameSystems/DuoQGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "DuoQCharacter.generated.h"

class UAIPerceptionStimuliSourceComponent;
class UInputComponent;
class USkeletalMeshComponent;
class USceneComponent;
class UCameraComponent;
class UAnimMontage;
class USoundBase;

UCLASS(config = Game)
class ADuoQCharacter : public ACharacter, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	enum ETeamIDs
	{
		Player = 0,
		Enemies = 1,

		NoTeamId = 255
	};

public:
	ADuoQCharacter();

	UFUNCTION(BlueprintCallable)
	UDuoQGameInstance* GetDuoQGameInstance();

	//UFUNCTION(BlueprintCallable)
	//void PlayFoley(TMap<TEnumAsByte<EPhysicalSurface>, USoundBase*> SoundMap);
	
	UFUNCTION(BlueprintCallable)
	TEnumAsByte<EPhysicalSurface> GetSurface(FVector& outBlend);

protected:
	virtual void BeginPlay();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UAIPerceptionStimuliSourceComponent* AIPerceptionStimuliSourceComponent;

	//UPROPERTY(BlueprintReadWrite, EditAnywhere)
	//UAudioComponent* FoleyAudioComponent;

	UPROPERTY(BlueprintReadOnly)
	TEnumAsByte<EPhysicalSurface> LastSurface;

public:
	// Virutal Function to handle character death
	virtual void HandleDeath();

private:
	UFUNCTION()
	void OnLevelLoaded();

	UFUNCTION()
	FVector GetGroundSurface(FHitResult& Hit);

	const FVector SurfaceLineCast = FVector(0.0f, 0.0f, -500.0f);
	const FVector GroundLineCast = FVector(0.0f, 0.0f, 10.0f);
};
