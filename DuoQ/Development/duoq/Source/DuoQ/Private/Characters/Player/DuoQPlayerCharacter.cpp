// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/Player/DuoQPlayerCharacter.h"

#include "DuoQBlueprintFunctionLibrary.h"
#include "Animation/AnimInstance.h"
#include "GameSystems/Communication/CommunicationSubsystem.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/AudioComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameSystems/DuoQGameInstance.h"
#include "GameSystems/DuoQGameMode.h"
#include "Characters/HealthComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameSystems/Communication/CommunicationSubsystem.h"
#include "Characters/Abilities/DuoQElectricRevolver.h"
#include "Components/Image.h"
#include "UI/ControlsWidget.h"
#include "GameSystems/DuoQSaveSystem.h"
#include "GameSystems/SaveSubSystem.h"
#include "GameSystems/Dialogue/BackupInputComponent.h"
#include "GameSystems/Dialogue/DialogueSubsystem.h"
#include "GameSystems/Dialogue/DialogueTriggerComponent.h"
#include "UI/BackupCalloutOptionWidget.h"
#include "UI/BackupCalloutWidget.h"
#include "UI/DialogueWidget.h"
#include "UI/GameplayHUD.h"

ADuoQPlayerCharacter::ADuoQPlayerCharacter()
{
	// Character doesnt have a rifle at start
	bHasRifle = false;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	CameraRotationPoint = CreateDefaultSubobject<USceneComponent>(TEXT("CameraRig"));
	CameraRotationPoint->SetupAttachment(GetCapsuleComponent());

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(CameraRotationPoint);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	// Mesh1P->SetRelativeRotation(FRotator(0.9f, -19.19f, 5.2f));

	USkeletalMesh*									 SkelMesh;
	ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshAsset(TEXT("/Script/Engine.SkeletalMesh'/Game/Art/Characters/Player/SKM_Player_v1_0.SKM_Player_v1_0'"));
	SkelMesh = MeshAsset.Object;
	Mesh1P->SetSkeletalMesh(SkelMesh);

	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(Mesh1P, FName("game_cam_parent"));
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	SceneCaptureComponent2D = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SceneCapture"));
	SceneCaptureComponent2D->SetupAttachment(FirstPersonCameraComponent);

	// Create an AudioComponent
	SprintLoop = CreateDefaultSubobject<UAudioComponent>(TEXT("SprintLoop"));
	SprintLoop->SetupAttachment(GetCapsuleComponent());
	ReviveSound = CreateDefaultSubobject<UAudioComponent>(TEXT("ReviveSound"));
	ReviveSound->SetupAttachment(GetCapsuleComponent());

	GunComponent = CreateDefaultSubobject<UDuoQElectricRevolver>(TEXT("ElectricRevolver"));
	GunComponent->SetupAttachment(Mesh1P, FName("r_hand"));

	BackupInputComponent = CreateDefaultSubobject<UBackupInputComponent>(TEXT("BackupInputComponent"));

	GetMesh()->bOwnerNoSee = true;
}
void ADuoQPlayerCharacter::ClearTimer(UWorld* world, bool tick, bool deltaTime)
{
	world->GetTimerManager().ClearTimer(GetGunHandle);
}

void ADuoQPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	FWorldDelegates::OnWorldCleanup.AddUObject(this, &ADuoQPlayerCharacter::ClearTimer);
	ClearTimer(GetWorld(), true, true);
	// Add Input Mapping Context
	if (ADuoQGameMode* DuoQGameMode = Cast<ADuoQGameMode>(GetWorld()->GetAuthGameMode()))
	{
		DuoQGameMode->Player = this;
		if (UDuoQSaveSystem* SaveGameInstance = Cast<UDuoQSaveSystem>(UGameplayStatics::LoadGameFromSlot("TestSaveSlot", 0)))
		{
			CurrentEncounter = SaveGameInstance->SavedEncounter;
			if (SaveGameInstance->PlayerHasGun)
			{
				GetWorld()->GetTimerManager().SetTimer(
					GetGunHandle,
					this, &ADuoQPlayerCharacter::GetGun,
					0.5f,
					false);
			}
			else if (SaveGameInstance->PlayerProgress > 0)
			{
				GetWorld()->GetTimerManager().SetTimer(
					GetGunHandle,
					this, &ADuoQPlayerCharacter::GetGun,
					0.5f,
					false);
			}
		}
	}
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	Starlight = UGameplayStatics::GetActorOfClass(GetWorld(), AStarlightCharacter::StaticClass());
	ReviveLocation = GetActorLocation();

	Mesh1P->SetVisibility(false, true);
}

void ADuoQPlayerCharacter::SetHasRifle(bool bNewHasRifle)
{
	bHasRifle = bNewHasRifle;
}

void ADuoQPlayerCharacter::GetGunFromPickUp()
{
	FirstOnGunPickUp();
	GetWorldTimerManager().SetTimer(GunPickupTimerHandle, this, &ADuoQPlayerCharacter::GetGun, 0.2f, false);
}

void ADuoQPlayerCharacter::GetGun()
{
	UDuoQGameInstance* GameInstance = Cast<UDuoQGameInstance>(GetWorld()->GetGameInstance());
	USaveSubSystem*	   SaveSystem = GameInstance->GetSubsystem<USaveSubSystem>();
	SaveSystem->SetGun(true, false);

	if (Mesh1P)
	{
		Mesh1P->SetVisibility(true, true);
	}

	if (PlayerHUD->GameplayHUD)
	{
		PlayerHUD->GameplayHUD->Crosshair->SetVisibility(ESlateVisibility::Visible);
	}
	if (UDialogueSubsystem* DialogueSubsystem = GetGameInstance()->GetSubsystem<UDialogueSubsystem>())
	{
		DialogueSubsystem->OnEventRaised.Broadcast(TEXT("PlayerHasGun"));
	}
	GunComponent->bIsUsable = true;
	OnGunPickUp();
}

bool ADuoQPlayerCharacter::GetHasRifle()
{
	return false;
}

void ADuoQPlayerCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add movement
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void ADuoQPlayerCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();
	if (InputState == EInputState::RegularGameplay && Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X * LookSensitivity);
		AddControllerPitchInput(LookAxisVector.Y * LookSensitivity);
	}
	else if (InputState == EInputState::BackupInput)
	{
		BackupInputComponent->HandleBackupInput(LookAxisVector);
	}
}

void ADuoQPlayerCharacter::TeleportToSpawn()
{
	ADuoQGameMode* mode = Cast<ADuoQGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	SetActorLocation(mode->FindPlayerStart(UGameplayStatics::GetPlayerController(GetWorld(), 0))->GetActorLocation());
}

void ADuoQPlayerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	TickSprint();
}

void ADuoQPlayerCharacter::BackupInputStart()
{
	BackupInputComponent->BackupInputStart();
	InputState = EInputState::BackupInput;
}

void ADuoQPlayerCharacter::BackupInputRelease()
{
	BackupInputComponent->BackupInputRelease();
	InputState = EInputState::RegularGameplay;
}

void ADuoQPlayerCharacter::UnMute()
{
	if (UCommunicationSubsystem* CommunicationSubsystem = GetGameInstance()->GetSubsystem<UCommunicationSubsystem>())
	{
		CommunicationSubsystem->ResumeAudioCapture();
	}
}
void ADuoQPlayerCharacter::Mute()
{
	if (UCommunicationSubsystem* CommunicationSubsystem = GetGameInstance()->GetSubsystem<UCommunicationSubsystem>())
	{
		CommunicationSubsystem->PauseAudioCapture();
	}
}

void ADuoQPlayerCharacter::ToggleMute()
{
	if (UCommunicationSubsystem* CommunicationSubsystem = GetGameInstance()->GetSubsystem<UCommunicationSubsystem>())
	{
		if (CommunicationSubsystem->IsRecording())
		{
			CommunicationSubsystem->PauseAudioCapture();
		}
		else
		{
			CommunicationSubsystem->ResumeAudioCapture();
		}
	}
}

void ADuoQPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ADuoQPlayerCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ADuoQPlayerCharacter::Look);

		// Reviving
		// EnhancedInputComponent->BindAction(ReviveAction, ETriggerEvent::Triggered, this, &ADuoQPlayerCharacter::StartReviveAttempt);
		// EnhancedInputComponent->BindAction(ReviveAction, ETriggerEvent::Completed, this, &ADuoQPlayerCharacter::CancelReviveAttempt);
		SetupPushToTalkToggle(false);

		// Pausing
		EnhancedInputComponent->BindAction(PauseAction, ETriggerEvent::Triggered, this, &ADuoQPlayerCharacter::Pause);

		// show controls
		EnhancedInputComponent->BindAction(ViewControlsAction, ETriggerEvent::Started, this, &ADuoQPlayerCharacter::ShowControls);
		EnhancedInputComponent->BindAction(ViewControlsAction, ETriggerEvent::Completed, this, &ADuoQPlayerCharacter::HideControls);

		EnhancedInputComponent->BindAction(BackupInputAction, ETriggerEvent::Started, this, &ADuoQPlayerCharacter::BackupInputStart);
		EnhancedInputComponent->BindAction(BackupInputAction, ETriggerEvent::Completed, this, &ADuoQPlayerCharacter::BackupInputRelease);
	}
}

void ADuoQPlayerCharacter::SetupPushToTalkToggle(bool bToggle)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		bool bRemovedControl = false;
		for (auto& Binding : TalkBindings)
		{
			bRemovedControl = EnhancedInputComponent->RemoveBindingByHandle(Binding) || bRemovedControl;
		}

		if (bRemovedControl)
		{
			if (UCommunicationSubsystem* CommunicationSubsystem = GetGameInstance()->GetSubsystem<UCommunicationSubsystem>())
			{
				CommunicationSubsystem->PauseAudioCapture();
			}
		}

		if (bToggle)
		{
			TalkBindings.Emplace(EnhancedInputComponent->BindAction(TalkAction, ETriggerEvent::Started, this, &ADuoQPlayerCharacter::ToggleMute).GetHandle());
		}
		else
		{
			TalkBindings.Emplace(EnhancedInputComponent->BindAction(TalkAction, ETriggerEvent::Started, this, &ADuoQPlayerCharacter::UnMute).GetHandle());
			TalkBindings.Emplace(EnhancedInputComponent->BindAction(TalkAction, ETriggerEvent::Completed, this, &ADuoQPlayerCharacter::Mute).GetHandle());
		}
	}
}

FGenericTeamId ADuoQPlayerCharacter::GetGenericTeamId() const
{
	return ETeamIDs::Player;
}

void ADuoQPlayerCharacter::TickSprint()
{
	FVector CurrentMoveDir = GetVelocity();
	CurrentMoveDir.Normalize();
	float dot = FVector::DotProduct(CurrentMoveDir, GetActorForwardVector());
	bool  bShouldSprint = dot >= SprintDot;
	GetCharacterMovement()->MaxWalkSpeed = bShouldSprint ? SprintSpeed : WalkSpeed;
}

void ADuoQPlayerCharacter::Pause()
{
	if (PlayerHUD)
	{
		PlayerHUD->TogglePauseMenu();
	}
}

void ADuoQPlayerCharacter::HandleDeath()
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
		// Get the location and rotation where the character died
		FVector	 DeathLocation = GetActorLocation();
		FRotator DeathRotation = GetActorRotation();

		// Spawn the object at the death location
		SpawnedSoul = GetWorld()->SpawnActor<AActor>(Soul, DeathLocation, DeathRotation);

		if (SpawnedSoul)
		{
			UPrimitiveComponent* SoulCollision = Cast<UPrimitiveComponent>(SpawnedSoul->GetComponentByClass(UPrimitiveComponent::StaticClass()));
			if (SoulCollision)
			{
				SoulCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			}

			// Save the Player's soul in the ReviveManager
			if (GameMode && GameMode->GetReviveManager())
			{
				GameMode->GetReviveManager()->PlayerSoul = SpawnedSoul;
			}
		}
	}
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		PlayerController->DisableInput(PlayerController);
		PlayerController->SetIgnoreLookInput(true);
	}
	OnPlayerDeath.Broadcast();
	UE_LOG(LogTemp, Warning, TEXT("Player has died"));
}

void ADuoQPlayerCharacter::DisableMovementOnDeath()
{
	GetMesh()->SetVisibility(false);
}

void ADuoQPlayerCharacter::RevivePlayer()
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		PlayerController->EnableInput(PlayerController);
		PlayerController->SetIgnoreLookInput(false);
	}
	GetMesh()->SetVisibility(true);
	OnPlayerRevive.Broadcast();
}

void ADuoQPlayerCharacter::RemoveActivatedTrigger(FString Dialogue)
{
	for (TObjectIterator<UDialogueTriggerComponent> DialogueIterator; DialogueIterator; ++DialogueIterator)
	{
		if (const UDialogueTriggerComponent* DialogueTriggerComponent = *DialogueIterator)
		{
			if (DialogueTriggerComponent->GetWorld() == GetWorld() && DialogueTriggerComponent->TriggeredDialogue == Dialogue)
			{
				if (DialogueTriggerComponent->GetOwner() != this)
				{
					DialogueTriggerComponent->GetOwner()->Destroy();
				}
			}
		}
	}
}

void ADuoQPlayerCharacter::HandleGameOver()
{
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		PlayerController->EnableInput(PlayerController);
	}
	OnPlayerRevive.Broadcast();

	this->SetActorLocation(ReviveLocation);
}

void ADuoQPlayerCharacter::StartReviveAttempt()
{
	ADuoQGameMode* GameMode = Cast<ADuoQGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GameMode && GameMode->GetReviveManager() && GameMode->GetReviveManager()->StarlightSoul)
	{
		if (FVector::Dist(GetActorLocation(), GameMode->GetReviveManager()->StarlightSoul->GetActorLocation()) <= MaxReviveDist)
		{
			// Start timer for completing the revive
			GetWorld()->GetTimerManager().SetTimer(ReviveTimerHandle, this, &ADuoQPlayerCharacter::CompleteRevive, ReviveHoldTime, false);
			ReviveSound->Play();
		}
	}
}

void ADuoQPlayerCharacter::CancelReviveAttempt()
{
	GetWorld()->GetTimerManager().ClearTimer(ReviveTimerHandle);
	ADuoQGameMode* GameMode = Cast<ADuoQGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GameMode && GameMode->GetReviveManager())
	{
		if (GameMode->GetReviveManager()->StarlightState == EState::Dead)
		{
			ReviveSound->Stop();
		}
	}
}

void ADuoQPlayerCharacter::CompleteRevive()
{
	// Complete the revive (revive Starlight)
	ADuoQGameMode* GameMode = Cast<ADuoQGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GameMode && GameMode->GetReviveManager() && GameMode->GetReviveManager()->StarlightSoul)
	{
		// Revive Starlight
		GameMode->GetReviveManager()->ReviveStarlight();
		ReviveSound->SetTriggerParameter(TEXT("Stop"));
		ReviveSound->StopDelayed(2.0f);
	}
}

void ADuoQPlayerCharacter::ShowControls()
{
	PlayerHUD->ControlsWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
}

void ADuoQPlayerCharacter::HideControls()
{
	PlayerHUD->ControlsWidget->SetVisibility(ESlateVisibility::Collapsed);
}