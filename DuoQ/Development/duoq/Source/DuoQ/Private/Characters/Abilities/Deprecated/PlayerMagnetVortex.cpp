// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Abilities/Deprecated/PlayerMagnetVortex.h"

#include "UI/AbilityWidget.h"
#include "DuoQBlueprintFunctionLibrary.h"
#include "Characters/Abilities/ElectricDamageType.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "UI/GameplayHUD.h"
#include "Characters/Player/DuoQPlayerCharacter.h"
#include "Components/AudioComponent.h"
#include "UI/DuoQHUD.h"

// Sets default values for this component's properties
UPlayerMagnetVortex::UPlayerMagnetVortex()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	Radius = 30;
	MaxCharge = 100;
	UseChargeRate = 10;
	RechargeRate = 10;
	Damage = 0;
	MaxPlacementRange = 1000.0f;

	CurrentCharge = MaxCharge;
	bIsAbilityActive = false;
	bIsHoldingKey = false;
	bIndicatorPlaced = false;
	bIsInPlacementMode = false;
}


// Called when the game starts
void UPlayerMagnetVortex::BeginPlay()
{
	Super::BeginPlay();

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(GetWorld()->GetFirstPlayerController()->InputComponent))
	{
		EnhancedInputComponent->BindAction(MagneticVortexAction, ETriggerEvent::Started, this, &UPlayerMagnetVortex::SpawnMagnetVortex);
		//EnhancedInputComponent->BindAction(MagneticVortexAction, ETriggerEvent::Completed, this, &UPlayerMagnetVortex::DespawnMagnetVortex); //Uncomment this if we no longer want the magnet vortex to activate as soon the player places it
		EnhancedInputComponent->BindAction(CancelPlacementAction, ETriggerEvent::Started, this, &UPlayerMagnetVortex::CancelPlacement);
	}
}


// Called every frame
void UPlayerMagnetVortex::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // Check if we are in placement mode
	if (bIsInPlacementMode)
	{
		// Update the indicator location in real-time
		FVector NewLocation = GetAbilityPlacementLocation();

		if (IndicatorActor && NewLocation != FVector::ZeroVector)
		{
			IndicatorActor->SetActorLocation(NewLocation);
			IndicatorLocation = NewLocation;
		}
	}
	else if (bIsAbilityActive) // Ability is active and not in placement mode
	{
		// Drain the ability's charge over time
		DrainCharge(DeltaTime);

		// Check if the charge is depleted
		if (CurrentCharge <= 0.0f)
		{
			// Deactivate the magnet vortex since we are out of charge
			DespawnMagnetVortex();
		}
	}
	else // Ability is not active and not in placement mode
	{
		// Recharge the ability's charge over time
		Recharge(DeltaTime);
	}

	if(ADuoQPlayerCharacter* PlayerCharacter = UDuoQBlueprintFunctionLibrary::GetDuoQPlayer(this))
	{
		//PlayerCharacter->PlayerHUD->GameplayHUD->VortexWidget->SetCharge(CurrentCharge);
	}
}




void UPlayerMagnetVortex::SpawnMagnetVortex()
{
	if (bIsAbilityActive)
	{
		DespawnMagnetVortex();
	}

	else if (!bIsInPlacementMode && !bIndicatorPlaced)
	{
		// Enter placement mode
		bIsInPlacementMode = true;
		OnVortex.Broadcast();

		// If the indicator is not placed, create it
		if (!bIndicatorPlaced)
		{
			PlaceIndicator(); // Spawn the indicator actor
			UGameplayStatics::PlaySound2D(this, SelectSfx);
		}

		ACharacter* Character = Cast<ACharacter>(GetOwner());
		if (UFunction* LockFunc = Character->FindFunction(TEXT("LockAbilities")))
		{
			Character->ProcessEvent(LockFunc, nullptr);
		}

		if (UFunction* LockFunc = Character->FindFunction(TEXT("LockShooting")))
		{
			Character->ProcessEvent(LockFunc, nullptr);
		}


	}

	else if (bIndicatorPlaced)
	{
		if (!bIsAbilityActive && CurrentCharge > 0.0f)
		{
			bIsAbilityActive = true;
			UGameplayStatics::PlaySound2D(this, ActivationSfx);
			if (UAudioComponent* Audio = Cast<UAudioComponent>(LoopSfx.GetComponent(GetOwner())))
			{
				Audio->Play();
			}

			// Disable player movement and apply the stun as before
			ACharacter* Character = Cast<ACharacter>(GetOwner());
			if (Character)
			{
				// Store the current walk speed for restoration later
				OriginalWalkSpeed = Character->GetCharacterMovement()->MaxWalkSpeed;

				// Disable horizontal movement by setting walk speed to 0
				Character->GetCharacterMovement()->MaxWalkSpeed = 0.0f;

				// Set movement mode to Falling to allow gravity to continue applying
				Character->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Falling);
			}

			if (UFunction* LockFunc = Character->FindFunction(TEXT("LockAbilities")))
			{
				Character->ProcessEvent(LockFunc, nullptr);
			}

			if (UFunction* LockFunc = Character->FindFunction(TEXT("LockShooting")))
			{
				Character->ProcessEvent(LockFunc, nullptr);
			}

			ApplyStun(IndicatorLocation);
		}
	}
	else
	{
		// Exit placement mode and place the magnet
		bIsInPlacementMode = false;
		bIndicatorPlaced = true;

		IndicatorActor->Destroy();

		FVector	Location = GetAbilityPlacementLocation();
		FActorSpawnParameters SpawnParams;
		IndicatorActor = GetWorld()->SpawnActor<AActor>(IndicatorPlaced, Location, FRotator::ZeroRotator, SpawnParams);
		IndicatorActor->SetActorEnableCollision(false);

		ACharacter* Character = Cast<ACharacter>(GetOwner());

		if (UFunction* UnlockFunc = Character->FindFunction(TEXT("UnlockAbilities")))
		{
			Character->ProcessEvent(UnlockFunc, nullptr);
		}

		if (UFunction* UnlockFunc = Character->FindFunction(TEXT("UnlockShooting")))
		{
			Character->ProcessEvent(UnlockFunc, nullptr);
		}

		SpawnMagnetVortex(); //Remove this line if we no longer want the magnet vortex to activate as soon the player places it
	}
	
}

//Function below is no longer needed, but in case we want to revert magnet vortex to activate separately from placement, it is being kept.
void UPlayerMagnetVortex::DespawnMagnetVortex()
{

	if (bIsAbilityActive)
	{
		// Existing despawn logic when the ability is active
		bIsAbilityActive = false;

		// Sends a trigger to start the deactivation sound and stop the loop at the peak of the deactivation sound
		if (UAudioComponent* Audio = Cast<UAudioComponent>(LoopSfx.GetComponent(GetOwner())))
		{
			Audio->SetTriggerParameter(FName(TEXT("Stop")));
		}

		ACharacter* Character = Cast<ACharacter>(GetOwner());
		if (Character)
		{
			Character->GetCharacterMovement()->MaxWalkSpeed = OriginalWalkSpeed;
			Character->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
		}

		if (IndicatorActor)
		{
			IndicatorActor->Destroy();
			IndicatorActor = nullptr;
		}

		if (UFunction* UnlockFunc = Character->FindFunction(TEXT("UnlockAbilities")))
		{
			Character->ProcessEvent(UnlockFunc, nullptr);
		}

		if (UFunction* UnlockFunc = Character->FindFunction(TEXT("UnlockShooting")))
		{
			Character->ProcessEvent(UnlockFunc, nullptr);
		}

		bIndicatorPlaced = false;
	}
	
}

void UPlayerMagnetVortex::Recharge(float DeltaTime)
{
	if (CurrentCharge < MaxCharge)
	{
		CurrentCharge += RechargeRate * DeltaTime;
		CurrentCharge = FMath::Min(CurrentCharge, MaxCharge);
	}
}

void UPlayerMagnetVortex::DrainCharge(float DeltaTime)
{
	CurrentCharge -= UseChargeRate * DeltaTime;
	CurrentCharge = FMath::Max(CurrentCharge, 0.0f);
}

void UPlayerMagnetVortex::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	//
}

void UPlayerMagnetVortex::ApplyStun(FVector Location)
{
	// Set the parameters for radial damage
	float					 DamageAmount = Damage;							// Set this to the desired damage value
	FVector					 DamageOrigin = Location; // Origin of the damage (center of the vortex)
	float					 DamageRadius = Radius;							// Set this to the desired radius
	TSubclassOf<UDamageType> DamageType = UElectricDamageType::StaticClass();

	// Apply the radial damage
	UGameplayStatics::ApplyRadialDamage(
		this,								   // World context object
		DamageAmount,						   // BaseDamage
		DamageOrigin,						   // Origin
		DamageRadius,						   // DamageRadius
		DamageType,							   // DamageTypeClass
		TArray<AActor*>(),					   // IgnoreActors
		GetOwner(),							   // DamageCauser
		GetOwner()->GetInstigatorController(), // InstigatedBy (InstigatorController)
		true,								   // bDoFullDamage
		ECC_Pawn							   // DamagePreventionChannel (you can choose an appropriate collision channel)
	);

	DrawDebugSphere(GetWorld(), DamageOrigin, DamageRadius, 12, FColor::Red, false, 4.0f);
}

FVector UPlayerMagnetVortex::GetAbilityPlacementLocation()
{
	AActor* Owner = GetOwner();

	if (Owner)
	{
		// Cast the owner to APawn
		APawn* OwnerPawn = Cast<APawn>(Owner);

		if (OwnerPawn)
		{
			FVector	 ActorLocation = Owner->GetActorLocation();
			FVector	 Offset = FVector(MaxPlacementRange, 0.0f, 0.0f); // Offset for maximum range
			FRotator ControlRotation = OwnerPawn->GetControlRotation();

			// Get the desired placement location
			FVector DesiredLocation = ActorLocation + ControlRotation.RotateVector(Offset);

			// Perform the initial line trace
			FHitResult			  InitialHit;
			FCollisionQueryParams CollisionParams;
			CollisionParams.AddIgnoredActor(Owner);
			bool bInitialHit = GetWorld()->LineTraceSingleByChannel(InitialHit, ActorLocation, DesiredLocation, ECC_Visibility, CollisionParams);

			if (bInitialHit)
			{
				FVector HitLocation = InitialHit.ImpactPoint;
				FVector HitNormal = InitialHit.ImpactNormal;

				// Check if the surface normal is steep (indicating a wall)
				if (FMath::Abs(HitNormal.Z) < 0.7f) // Z close to 0 means it�s a vertical surface, < 0.7 is a wall
				{
					// Add an offset along the wall normal to move the trace away from the wall
					FVector DownwardTraceStart = HitLocation + (HitNormal * 50.0f); // Adjust the distance as needed

					// Perform a downward trace to find the ground, away from the wall
					FHitResult GroundHit;
					FVector	   GroundTraceEnd = DownwardTraceStart - FVector(0.0f, 0.0f, 10000.0f); // Trace downwards

					bool bGroundHit = GetWorld()->LineTraceSingleByChannel(GroundHit, DownwardTraceStart, GroundTraceEnd, ECC_Visibility, CollisionParams);

					if (bGroundHit)
					{
						// Return the ground location for placement
						return GroundHit.ImpactPoint;
					}
					else
					{
						// If no ground is found, return the offset location
						return DownwardTraceStart;
					}
				}
				else
				{
					// If the surface is suitable, use the hit location
					return HitLocation;
				}
			}
			else
			{
				// If no initial hit, perform a downward trace from the maximum range location
				FVector MaxRangeLocation = ActorLocation + (ControlRotation.Vector() * MaxPlacementRange);

				FHitResult GroundHit;
				FVector	   GroundTraceStart = MaxRangeLocation;
				FVector	   GroundTraceEnd = GroundTraceStart - FVector(0.0f, 0.0f, 10000.0f); // Trace downwards

				bool bGroundHit = GetWorld()->LineTraceSingleByChannel(GroundHit, GroundTraceStart, GroundTraceEnd, ECC_Visibility, CollisionParams);

				if (bGroundHit)
				{
					// Return the ground hit location
					return GroundHit.ImpactPoint;
				}
				else
				{
					// If no ground found, use the max range location
					return MaxRangeLocation;
				}
			}
		}
	}

	// Return zero vector if something went wrong
	return FVector::ZeroVector;
}

void UPlayerMagnetVortex::PlaceIndicator()
{
	FVector Location = GetAbilityPlacementLocation();
	if (Location != FVector::ZeroVector)
	{
		IndicatorLocation = Location;

		// Assuming you have an indicator actor or component
		if (IndicatorActor == nullptr)
		{
			// Spawn a new indicator actor if it doesn't exist
			FActorSpawnParameters SpawnParams;
			IndicatorActor = GetWorld()->SpawnActor<AActor>(IndicatorClass, Location, FRotator::ZeroRotator, SpawnParams);

			// If spawned successfully, ignore it in raycast checks
			if (IndicatorActor)
			{
				IndicatorActor->SetActorEnableCollision(false);
			}
		}
		else
		{
			// Move the existing indicator actor to the new location
			IndicatorActor->SetActorLocation(Location);
		}
	}
}

void UPlayerMagnetVortex::CancelPlacement()
{
	if (bIsInPlacementMode)
	{
		// Exit placement mode
		bIsInPlacementMode = false;

		// Destroy the indicator actor
		if (IndicatorActor)
		{
			IndicatorActor->Destroy();
			IndicatorActor = nullptr;
		}

		bIndicatorPlaced = false;

		ACharacter* Character = Cast<ACharacter>(GetOwner());

		if (UFunction* UnlockFunc = Character->FindFunction(TEXT("UnlockAbilities")))
		{
			Character->ProcessEvent(UnlockFunc, nullptr);
		}

		if (UFunction* UnlockFunc = Character->FindFunction(TEXT("UnlockShooting")))
		{
			Character->ProcessEvent(UnlockFunc, nullptr);
		}
	}
}



