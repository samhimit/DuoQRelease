// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/Partner/StarlightCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "GameSystems/Progression/PressurePlateActor.h"
#include "GameSystems/DuoQGameMode.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Curves/CurveFloat.h"
#include "Components/AudioComponent.h"
#include "DuoQBlueprintFunctionLibrary.h"
//#include "IContentBrowserSingleton.h"
#include "Characters/HealthComponent.h"
#include "Characters/Partner/StarlightTarget.h"
#include "UI/GameplayHUD.h"
#include "Characters/Partner/StarlightGOAPController.h"
#include "UI/OffscreenIndicatorWidget.h"
#include "GameSystems/Dialogue/DialogueSubsystem.h"
#include "GameSystems/DuoQSaveSystem.h"
#include "GameSystems/SaveSubSystem.h"

AStarlightCharacter::AStarlightCharacter()
{
	Gun3P = CreateDefaultSubobject<UDuoQStarlightWeapon>(TEXT("GunMesh3P"));
	Gun3P->SetupAttachment(GetMesh());

	VoiceSource = CreateDefaultSubobject<UAudioComponent>(TEXT("VoiceSource"));
	VoiceSource->SetupAttachment(RootComponent, NAME_None);
	ReviveSound = CreateDefaultSubobject<UAudioComponent>(TEXT("ReviveSound"));
	ReviveSound->SetupAttachment(RootComponent);

	// Create a spring arm for the camera to follow behind Starlight
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);

	CameraBoom->bUsePawnControlRotation = true;

	CameraOffset = FVector(0.0f, 75.0f, 50.0f);
	CameraDistance = 300.0f;

	CameraBoom->TargetArmLength = CameraDistance;
	CameraBoom->SocketOffset = CameraOffset;

	// Create the camera component
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;
}

void AStarlightCharacter::GetGun()
{

	Gun3P->SetVisibility(true, true);
	bHasWeapon = true;
	Gun3P->bIsUsable = true;
	OnGunPickUp();
}

FVector AStarlightCharacter::FindClosestPressurePlate()
{
	TArray<AActor*> PressurePlateActors;
	// what we will return
	FVector Ans;
	double	ShortestDist = 1000000000.0;
	// get the list of all pressure plates in the scene
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APressurePlateActor::StaticClass(), PressurePlateActors);
	if (!PressurePlateActors.IsEmpty())
	{
		FVector PPLocation;
		float	CurrentDist;
		for (AActor* PP : PressurePlateActors)
		{
			if (APressurePlateActor* PPActor = Cast<APressurePlateActor>(PP))
			{
				PPLocation = PPActor->GetActorLocation();
				CurrentDist = FVector::Dist(PPLocation, GetActorLocation());

				// check if current dist is smaller
				if (CurrentDist < ShortestDist)
				{
					ShortestDist = CurrentDist;
					Ans = PPLocation;
				}
			}
		}
	}
	return Ans;
}

FVector AStarlightCharacter::GetCurrentFountainTargetLocation()
{
	if (CurrFountainIndex < FountainTargets.Num())
	{
		if (AActor* Fountain = FountainTargets[CurrFountainIndex].Get())
		{
			if (Fountain != nullptr)
			{
				if (AStarlightGOAPController* SGC = UDuoQBlueprintFunctionLibrary::GetStarlightGOAPController(this))
				{
					SGC->SetFocus(Fountain);
				}
				UStaticMeshComponent* Cube = Fountain->GetComponentByClass<UStaticMeshComponent>();
				return Cube->GetComponentLocation();
			}
		}
	}

	return FVector();
}

void AStarlightCharacter::HandleDeath()
{
	ADuoQGameMode* GameMode = Cast<ADuoQGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

	if (GameMode)
	{
		UReviveManager* ReviveManager = GameMode->GetReviveManager();
		if (ReviveManager)
		{
			ReviveManager->HandleDeath(this);
		}
	}

	if (Soul)
	{
		FVector	 DeathLocation = GetActorLocation();
		FRotator DeathRotation = GetActorRotation();

		SpawnedSoul = GetWorld()->SpawnActor<AActor>(Soul, DeathLocation, DeathRotation);

		if (SpawnedSoul)
		{
			UPrimitiveComponent* SoulCollision = Cast<UPrimitiveComponent>(SpawnedSoul->GetComponentByClass(UPrimitiveComponent::StaticClass()));
			if (SoulCollision)
			{
				SoulCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			}

			// Save Starlight's soul in the ReviveManager
			if (GameMode && GameMode->GetReviveManager())
			{
				GameMode->GetReviveManager()->StarlightSoul = SpawnedSoul;
			}
		}
	}
	OnStarlightDead.Broadcast();
	FVector FarAwayLocation = FVector(100000.0f, GetActorLocation().Y, GetActorLocation().Z);
	SetActorLocation(FarAwayLocation);
	GetMesh()->SetVisibility(false);

	UE_LOG(LogTemp, Warning, TEXT("Starlight has died"));
}

/*
 * Deprecated function, do not use!
 */
void AStarlightCharacter::StartShooting()
{
	USkeletalMeshComponent* CurrMesh = GetMesh();
	FString					Name = CurrMesh->GetName();
	float					AnimTime = CurrMesh->GetAnimInstance()->Montage_Play(ShootMontage);
	FString					TheFloatStr = FString::SanitizeFloat(AnimTime);
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TheFloatStr);
	}
	if (UDuoQStarlightWeapon* Weapon = Cast<UDuoQStarlightWeapon>(Gun3P))
	{
		Weapon->TriggerDown();
	}
}

FGenericTeamId AStarlightCharacter::GetGenericTeamId() const
{
	return ETeamIDs::Player;
}

void AStarlightCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (ADuoQGameMode* DuoQGameMode = Cast<ADuoQGameMode>(GetWorld()->GetAuthGameMode()))
	{
		DuoQGameMode->Starlight = this;
		if (UDuoQSaveSystem* SaveGameInstance = Cast<UDuoQSaveSystem>(UGameplayStatics::LoadGameFromSlot("TestSaveSlot", 0)))
		{
			AActor* PlayerStart = DuoQGameMode->FindPlayerStart(GetWorld()->GetFirstPlayerController());
			SetActorLocation(PlayerStart->GetActorLocation() + PlayerStart->GetActorForwardVector() * 55.0f);
			UDuoQGameInstance* GameInstance = Cast<UDuoQGameInstance>(GetWorld()->GetGameInstance());
			USaveSubSystem*	   SaveSystem = GameInstance->GetSubsystem<USaveSubSystem>();
			SaveSystem->bTalaHasGun = false;
			SaveSystem->bPlayerHasGun = false;
			SaveSystem->Save(0);
			if (SaveGameInstance->StarlightHasGun)
			{
				GetGun();
			}
			else if (SaveGameInstance->PlayerProgress > 0)
			{
				GetGun();
			}
			DuoQGameMode->LevelLoaded.AddDynamic(this, &AStarlightCharacter::DestroyObjects);
		}
	}
	FOnTimelineFloat TimelineProgress;
	TimelineProgress.BindUFunction(this, FName("TimelineCallback"));
	FOnTimelineEvent TimelineEnd;
	TimelineEnd.BindUFunction(this, FName("TimelineFinishedCallback"));
	MyTimeline.AddInterpFloat(FloatCurve, TimelineProgress);
	MyTimeline.AddEvent(1.0f, TimelineEnd);
	MyTimeline.SetLooping(false);

	ReviveLocation = GetActorLocation();

	GetComponentByClass<UHealthComponent>()->OnHealthChanged.AddDynamic(this, &AStarlightCharacter::UpdateHealthBar);
}

void AStarlightCharacter::DestroyObjects()
{

	for (FCheckpointObject o : CheckpointDestroyList)
	{
		if (o.Progress == Cast<UDuoQSaveSystem>(UGameplayStatics::LoadGameFromSlot("TestSaveSlot", 0))->PlayerProgress)
		{
			for (TSoftObjectPtr<AActor> a : o.Targets)
			{
				if(a.Get())
				{
					a->Destroy();
				}
			}
			break;
		}
	}
}

void AStarlightCharacter::TimelineCallback(float val)
{
	FRotator curr = FMath::LerpRange(TurnOrigin, TurnTarget, val);
	GetController()->SetControlRotation(curr);
	SetActorRotation(curr);
}

void AStarlightCharacter::TimelineTick(float deltaTime)
{
	MyTimeline.TickTimeline(deltaTime);
}

void AStarlightCharacter::TurnToPlayer()
{
	TurnTarget = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)->GetActorLocation());
	TurnOrigin = GetActorRotation();
	MyTimeline.PlayFromStart();
}

void AStarlightCharacter::TurnToFuturePlayer()
{
	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	FVector		PlayerLocation = PlayerCharacter->GetActorLocation();
	FVector		PlayerVelocity = PlayerCharacter->GetVelocity();

	float DistanceToPlayer = FVector::Dist(GetActorLocation(), PlayerLocation);
	float RocketSpeed = 1500.0f;
	float TimeToTarget = DistanceToPlayer / RocketSpeed;

	// Predict future player location based on their current velocity
	FVector PredictedLocation = PlayerLocation + (PlayerVelocity * TimeToTarget);

	TurnTarget = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), PredictedLocation);
	TurnOrigin = GetActorRotation();
	MyTimeline.PlayFromStart();
}

void AStarlightCharacter::HandleGameOver()
{
	if (SpawnedSoul)
	{
		SpawnedSoul->Destroy();
	}

	this->SetActorLocation(ReviveLocation);
	OnStarlightRevive.Broadcast();
}

bool AStarlightCharacter::StrafeWhileMouseDead(AActor* TargetToFind)
{
	// try to get the movement component
	if (UCharacterMovementComponent* MC = GetCharacterMovement())
	{
		// perform vector math to see if the target is to the left or right
		FVector StarlightFwd = GetActorForwardVector();
		FVector SLocation = GetActorLocation();
		FVector StarlightToTarget = TargetToFind->GetActorLocation() - SLocation;

		FVector Cross = FVector::CrossProduct(StarlightFwd, StarlightToTarget);

		bHasSetDirection = true;

		// figure out if we want to strafe left or right
		if (Cross.Z > 0.0f)
		{
			// starlight needs to strafe right
			// AddMovementInput(GetActorRightVector(), 1.0f);
			bIsGoingRight = true;
		}
		else
		{
			// AddMovementInput((GetActorRightVector() * -1.0f), 1.0f);
			bIsGoingRight = false;
		}

		// find out the angle
		StarlightToTarget.Normalize();
		StarlightFwd.Normalize();
		float Dot = FVector::DotProduct(StarlightToTarget, StarlightFwd);
		Dot = FMath::Acos(Dot);
		Dot = FMath::Abs(Dot);

		// transfrom in degrees
		Dot = FMath::RadiansToDegrees(Dot);

		if (GEngine)
		{
			// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Calculated angle: %f"), Dot));
		}

		if (Dot <= 5.0f)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	return false;
}

bool AStarlightCharacter::IsPlayerInFront(float InFrontAngle)
{
	FVector				  StarligthFwd = GetActorForwardVector();
	ADuoQPlayerCharacter* Player = UDuoQBlueprintFunctionLibrary::GetDuoQPlayer(this);
	FVector				  StarToPlay = Player->GetActorLocation() - GetActorLocation();
	StarToPlay.Normalize();
	StarligthFwd.Normalize();

	float Dot = FVector::DotProduct(StarToPlay, StarligthFwd);
	Dot = FMath::Acos(Dot);
	Dot = FMath::Abs(Dot);
	Dot = FMath::RadiansToDegrees(Dot);

	if (Dot <= InFrontAngle)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void AStarlightCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bHasSetDirection)
	{
		if (bIsGoingRight)
		{
			AddMovementInput(GetActorRightVector(), 1.0f);
		}
		else
		{
			AddMovementInput((GetActorRightVector() * -1.0f), 1.0f);
		}
	}

	if (ADuoQPlayerCharacter* Player = UDuoQBlueprintFunctionLibrary::GetDuoQPlayer(GetWorld()))
	{
		if (ADuoQGameMode* GameMode = UDuoQBlueprintFunctionLibrary::GetDuoQGameMode(GetWorld()))
		{
			UReviveManager* RM = GameMode->GetReviveManager();
			if (RM->StarlightSoul)
			{
				float DistFromPlayer = FVector::Dist(RM->StarlightSoul->GetActorLocation(), Player->GetActorLocation());
				if (RM->PlayerState != EState::Dead && RM->StarlightState == EState::Dead)
				{
					if (DistFromPlayer < Player->MaxReviveDist)
					{
						// UDuoQBlueprintFunctionLibrary::GetHUDInstance(GetWorld())->GameplayHUD->ShowReviveText();
					}
					// UDuoQBlueprintFunctionLibrary::GetDuoQPlayerController(GetWorld(), 0)->GetOffscreenIndicatorWidget()->SetVisibility(ESlateVisibility::Collapsed);
					// UDuoQBlueprintFunctionLibrary::GetHUDInstance(GetWorld())->GameplayHUD->HideOffscreenIndicator();
				}
				else
				{
					UDuoQBlueprintFunctionLibrary::GetHUDInstance(GetWorld())->GameplayHUD->HideReviveText();
					UDuoQBlueprintFunctionLibrary::GetHUDInstance(GetWorld())->GameplayHUD->ShowOffscreenIndicator();
					UDuoQBlueprintFunctionLibrary::GetDuoQPlayerController(GetWorld(), 0)->GetOffscreenIndicatorWidget()->SetVisibility(ESlateVisibility::Visible);
				}
			}
		}
	}

	if (AStarlightGOAPController* STG = UDuoQBlueprintFunctionLibrary::GetStarlightGOAPController(this))
	{
		if (UGOAPAction* CurrAction = STG->GetCurrentAction())
		{
			// check to see if current action is StayPut
			if (CurrAction->getName().Equals("StayPut"))
			{
				// we also need to check if Tala is in conversation so that we don't make her look away if she is saying something
				if (UDuoQGameInstance* DGI = UDuoQBlueprintFunctionLibrary::GetDuoQGameInstance(this))
				{
					if (UDialogueSubsystem* DS = DGI->GetSubsystem<UDialogueSubsystem>())
					{
						// should focus on playr
						if (DS->GetIsInConversation())
						{
							if (ADuoQPlayerCharacter* Player = UDuoQBlueprintFunctionLibrary::GetDuoQPlayer(this))
							{
								STG->SetFocus(Player);
								return;
							}
						}
					}
				}
			}
		}
	
	}

	/*
	// we start checking if Tala needs to change her lookat location if her current action is stayput
	if (AStarlightGOAPController* STG = UDuoQBlueprintFunctionLibrary::GetStarlightGOAPController(this))
	{
		if (UGOAPAction* CurrAction = STG->GetCurrentAction())
		{
			// check to see if current action is StayPut
			if (CurrAction->getName().Equals("StayPut"))
			{

				// check to see if current look duration is long enough
				if (CurrentLookDurationCounter < TargetLookDurationCounter)
				{
					CurrentLookDurationCounter += DeltaSeconds;
				}
				else // look duration is long enough so we set a new look at location based on the offsets we have
				{
					// reset target look duration
					TargetLookDurationCounter = FMath::RandRange(LookDurationLowerBound, LookDurationUpperBound);
					CurrentLookDurationCounter = 0.0f;

					// finding a new lookat location
					if (ADuoQPlayerCharacter* Player = UDuoQBlueprintFunctionLibrary::GetDuoQPlayer(this))
					{
						// randomly select one of the lookats
						FVector NewLookAt = Player->GetActorLocation() + LookOffsets[(int)FMath::RandRange(0, LookOffsets.Num() - 1)];
						STG->SetFocalPoint(NewLookAt);
					}
				}
			}
			else
			{
				CurrentLookDurationCounter = 0.0f;
			}
		}
	}*/
}

FVector AStarlightCharacter::GetPlayerSpawnLocation()
{
	if (ADuoQGameMode* DuoQGameMode = Cast<ADuoQGameMode>(GetWorld()->GetAuthGameMode()))
	{
		DuoQGameMode->Starlight = this;
		if (UDuoQSaveSystem* SaveGameInstance = Cast<UDuoQSaveSystem>(UGameplayStatics::LoadGameFromSlot("TestSaveSlot", 0)))
		{
			if (SaveGameInstance->PlayerProgress != 0)
			{
				return DuoQGameMode->FindPlayerStart(GetWorld()->GetFirstPlayerController())->GetActorLocation();
			}
		}
	}
	return FVector();
}

UCameraComponent* AStarlightCharacter::GetFollowCamera() const
{
	return FollowCamera;
}

void AStarlightCharacter::ReviveStarlight()
{
	GetMesh()->SetVisibility(true);
	OnStarlightRevive.Broadcast();
}

void AStarlightCharacter::StartReviveAttempt()
{
	ADuoQGameMode* GameMode = Cast<ADuoQGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GameMode && GameMode->GetReviveManager() && GameMode->GetReviveManager()->PlayerSoul)
	{
		if (FVector::Dist(GetActorLocation(), GameMode->GetReviveManager()->PlayerSoul->GetActorLocation()) <= 200.0f)
		{
			// Start timer for completing the revive
			GetWorld()->GetTimerManager().SetTimer(RevivePlayerTimerHandle, this, &AStarlightCharacter::CompleteRevive, ReviveHoldTime, false);
			ReviveSound->Play();
		}
	}
}

bool AStarlightCharacter::CancelReviveAttempt()
{
	GetWorld()->GetTimerManager().ClearTimer(RevivePlayerTimerHandle);
	ADuoQGameMode* GameMode = Cast<ADuoQGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GameMode && GameMode->GetReviveManager())
	{
		if (GameMode->GetReviveManager()->PlayerState == EState::Dead)
		{
			ReviveSound->Stop();
		}
	}

	return false;
}

void AStarlightCharacter::CompleteRevive()
{
	// Complete the revive (revive Player)
	ADuoQGameMode* GameMode = Cast<ADuoQGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GameMode && GameMode->GetReviveManager() && GameMode->GetReviveManager()->PlayerSoul)
	{
		// Revive Player
		GameMode->GetReviveManager()->RevivePlayer();
		ReviveSound->SetTriggerParameter(TEXT("Stop"));
		ReviveSound->StopDelayed(2.0f);
	}
}

void AStarlightCharacter::UpdateHealthBar(float NewHealth, float MaxHealth)
{
	if (UDuoQHUD* HUD = UDuoQBlueprintFunctionLibrary::GetHUDInstance(GetWorld()))
	{
		HUD->UpdateHealthBarPartner(NewHealth, MaxHealth);
	}
}
