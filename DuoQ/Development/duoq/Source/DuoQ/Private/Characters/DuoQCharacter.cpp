// Copyright Epic Games, Inc. All Rights Reserved.

#include "Characters/DuoQCharacter.h"

#include "Animation/AnimInstance.h"
#include "GameSystems/DuoQGameMode.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameSystems/Audio/GroundData.h"
#include "Components/AudioComponent.h"

//////////////////////////////////////////////////////////////////////////
// ADuoQCharacter

ADuoQCharacter::ADuoQCharacter()
	: ACharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	AIPerceptionStimuliSourceComponent = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("AIPerceptionStimuli"));

	/*
	FoleyAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("FoleyAudioComponent"));
	FoleyAudioComponent->SetupAttachment(GetCapsuleComponent());
	FoleyAudioComponent->SetAutoActivate(false);
	FoleyAudioComponent->bCanPlayMultipleInstances = true;
	*/

}

UDuoQGameInstance* ADuoQCharacter::GetDuoQGameInstance()
{
	if(UDuoQGameInstance* DuoQGameInstance = Cast<UDuoQGameInstance>(GetGameInstance()))
	{
		return DuoQGameInstance;
	}
	UE_LOG(LogTemp, Warning, TEXT("Failed to get DuoQGameInstance in DuoQCharacter"));
	return nullptr;
}

/*
void ADuoQCharacter::PlayFoley(TMap<TEnumAsByte<EPhysicalSurface>, USoundBase*> SoundMap)
{
	FVector Blend = FVector::ZeroVector;
	GetSurface(Blend);
	FoleyAudioComponent->SetSound(SoundMap.FindRef(LastSurface));
	if (LastSurface == EPhysicalSurface::SurfaceType9)
	{
		FoleyAudioComponent->SetFloatParameter(FName(TEXT("Dirt")), Blend.X);
		FoleyAudioComponent->SetFloatParameter(FName(TEXT("Grass")), Blend.Y);
		FoleyAudioComponent->SetFloatParameter(FName(TEXT("Stone")), Blend.Z);
	}
	FoleyAudioComponent->Play();
}
*/

void ADuoQCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	if (ADuoQGameMode* DuoQGameMode = Cast<ADuoQGameMode>(GetWorld()->GetAuthGameMode()))
	{
		if (!DuoQGameMode->bIsLevelLoaded)
		{
			GetCharacterMovement()->DisableMovement();
			DuoQGameMode->LevelLoaded.AddDynamic(this, &ADuoQCharacter::OnLevelLoaded);
		}
	}
}

TEnumAsByte<EPhysicalSurface> ADuoQCharacter::GetSurface(FVector& outBlend)
{
	FHitResult Hit;
	if (UKismetSystemLibrary::LineTraceSingle(
			this, 
			GetActorLocation(), 
			GetActorLocation() + SurfaceLineCast, 
			ETraceTypeQuery::TraceTypeQuery3, 
			false, 
			TArray<AActor*, FDefaultAllocator>(), 
			EDrawDebugTrace::None, 
			Hit, 
			true))
	{
		EPhysicalSurface HitSurface = UGameplayStatics::GetSurfaceType(Hit);
		if (HitSurface == EPhysicalSurface::SurfaceType9)
		{
			outBlend = GetGroundSurface(Hit);
		}
		LastSurface = HitSurface;
	}

	return LastSurface;
}

FVector ADuoQCharacter::GetGroundSurface(FHitResult& Hit)
{
	if (UGroundData* GroundData = Hit.GetActor()->GetComponentByClass<UGroundData>())
	{
		FHitResult UVHit;
		if (UKismetSystemLibrary::LineTraceSingle(
				this,
				Hit.ImpactPoint + GroundLineCast,
				Hit.ImpactPoint - GroundLineCast,
				GroundData->GroundDataAsset->TraceChannel,
				true,
				TArray<AActor*, FDefaultAllocator>(),
				EDrawDebugTrace::None,
				UVHit,
				true))
		{
			FVector2D UV;
			if (UGameplayStatics::FindCollisionUV(UVHit, 0, UV))
			{
				FColor GroundColor = GroundData->GetGroundColor(UV.X, UV.Y);
				return FVector(GroundColor.R / 255.0f, GroundColor.G / 255.0f, GroundColor.B / 255.0f);
			}
		}
	}

	return FVector::YAxisVector; // Default to grass
}

void ADuoQCharacter::HandleDeath()
{
	UE_LOG(LogTemp, Warning, TEXT("Character has died"));
}
void ADuoQCharacter::OnLevelLoaded()
{
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Falling);
}
//////////////////////////////////////////////////////////////////////////// Input
