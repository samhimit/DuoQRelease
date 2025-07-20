#include "Characters/ReviveManager.h"

#include "DuoQBlueprintFunctionLibrary.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "UI/GameplayHUD.h"
#include "GameSystems/Audio/AudioSubsystem.h"
#include "Components/RadialSlider.h"
#include "Components/TextBlock.h"
//>>>> ORIGINAL //AGP_20243-20251/DuoQ/Development/duoq/Source/DuoQ/Private/Characters/ReviveManager.cpp#46
// #include "Factories/BlueprintFunctionLibraryFactory.h"
//==== THEIRS //AGP_20243-20251/DuoQ/Development/duoq/Source/DuoQ/Private/Characters/ReviveManager.cpp#47
//==== YOURS //Colinxu_DuoQ2024_Windows/DuoQ/Development/duoq/Source/DuoQ/Private/Characters/ReviveManager.cpp
// #include "Factories/BlueprintFunctionLibraryFactory.h"
//< < < <

UReviveManager::UReviveManager()
{
	SoulPickupTime = 0.0f;
	ReviveTime = 0.0f;
	AutoReviveTime = 5.0f;
	bPlayerHasSoul = false;
	bStarlightHasSoul = false;
	ReviveHealth = 1.0f;
	bBothPlayersDead = false;
	StarlightState = EState::Alive;
	PlayerState = EState::Alive;
	FadeTimeCameraSwitch = 0.0f;

	// Initialize widget class
	static ConstructorHelpers::FClassFinder<UUserWidget> FadeWidgetClass(TEXT("/Game/UI/WBP_FadeScreen"));
	if (FadeWidgetClass.Succeeded())
	{
		FadeWidget = FadeWidgetClass.Class;
	}

	static ConstructorHelpers::FClassFinder<UUserWidget> GameOverWidgetClass(TEXT("/Game/UI/WBP_GameOverScreen"));
	if (GameOverWidgetClass.Succeeded())
	{
		GameOverWidget = GameOverWidgetClass.Class;
	}

	static ConstructorHelpers::FClassFinder<UUserWidget> ReviveWidgetClass(TEXT("/Game/UI/WBP_FadeToBlack"));
	if (ReviveWidgetClass.Succeeded())
	{
		ReviveWidget = ReviveWidgetClass.Class;
	}
	FWorldDelegates::OnWorldCleanup.AddUObject(this, &UReviveManager::ClearTimer);
}
void UReviveManager::ClearTimer(UWorld* world, bool tick, bool deltaTime)
{
	world->GetTimerManager().ClearTimer(ReviveTimerHandle);
	world->GetTimerManager().ClearTimer(AutoPlayerReviveTimerHandle);
	world->GetTimerManager().ClearTimer(AutoStarlightReviveTimerHandle);
	world->GetTimerManager().ClearTimer(SwitchCameraHandle);
	world->GetTimerManager().ClearTimer(GameOverTimerHandle);
}

void UReviveManager::GameOver()
{
	UE_LOG(LogTemp, Warning, TEXT("Both the player and Starlight has died."));

	if (GameOverWidget)
	{
		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		GameOverInstance = UDuoQBlueprintFunctionLibrary::GetHUDInstance(GetWorld())->GameOverScreen;
		if (GameOverInstance)
		{
			// FadeWidgetInstance->RemoveFromParent();
			FadeWidgetInstance->SetVisibility(ESlateVisibility::Collapsed);
			// GameOverInstance->AddToViewport();
			GameOverInstance->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			FOutputDeviceNull OutputDevice;
			GameOverInstance->CallFunctionByNameWithArguments(TEXT("StartFadeAnimation"), OutputDevice, nullptr, true);

			if (UAudioSubsystem* Audio = GetWorld()->GetGameInstance()->GetSubsystem<UAudioSubsystem>())
			{
				Audio->StopMusic(2.0f);
			}
			ReviveTimerMax = 5.0f;
			GetWorld()->GetTimerManager().ClearTimer(ReviveTimerHandle);
			GetWorld()->GetTimerManager().ClearTimer(GameOverTimerHandle);
			GetWorld()->GetTimerManager().ClearTimer(SwitchCameraHandle);
			GetWorld()->GetTimerManager().SetTimer(
				ReviveTimerHandle,
				[this, PlayerController]() {
					RevivePlayer();
					ReviveStarlight();
					// GameOverInstance->RemoveFromParent();
					GameOverInstance->SetVisibility(ESlateVisibility::Collapsed);
					// PlayerController->ConsoleCommand(TEXT("RestartLevel"));
				},
				ReviveTimerMax,
				false);
			ReviveTimer = 0.0f;
			GetWorld()->GetTimerManager().SetTimer(
				GameOverTimerHandle,
				this,
				&UReviveManager::ReviveBothUIUpdate,
				0.1f,
				true);
		}
	}
	OnGameOver.Broadcast();
}

void UReviveManager::ReviveUIUpdate(UUserWidget* src)
{
	ReviveTimer += 0.1f;
	if (ReviveTimer >= AutoReviveTime)
	{
		UWorld* world = GetWorld();
		GetWorld()->GetTimerManager().ClearTimer(GameOverTimerHandle);
		return;
	}
	// Update the UI here.
	URadialSlider* slider = Cast<URadialSlider>(src->GetWidgetFromName(TEXT("Timer")));
	if (slider)
	{
		slider->SetValue(ReviveTimer / AutoReviveTime);
	}
	UTextBlock* text = Cast<UTextBlock>(src->GetWidgetFromName(TEXT("TimerText")));
	if (text)
	{
		int Time = static_cast<int>(AutoReviveTime - ReviveTimer) + 1;
		// int i = (int) roundf(f * 10.0f);
		// f = ((float) i) / 10.0f;
		FString ToString = FString::FromInt(Time);
		// ToString = string.Mid(0,string.Find(".")+2);
		text->SetText(FText::FromString(ToString));
	}
}

void UReviveManager::ReviveSingleUIUpdate()
{

	ReviveUIUpdate(FadeWidgetInstance);
}
void UReviveManager::ReviveBothUIUpdate()
{
	ReviveUIUpdate(GameOverInstance);
}

void UReviveManager::FadeToBlack(APlayerController* PlayerController)
{
	if (FadeWidget)
	{
		// Create the widget and add it to the viewport
		FadeWidgetInstance = UDuoQBlueprintFunctionLibrary::GetHUDInstance(GetWorld())->FadeScreen;
		// CreateWidget<UUserWidget>(PlayerController, FadeWidget);
		if (FadeWidgetInstance)
		{
			FadeWidgetInstance->Construct();
			FadeWidgetInstance->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			FOutputDeviceNull OutputDevice;
			FadeWidgetInstance->CallFunctionByNameWithArguments(TEXT("StartFadeAnimation"), OutputDevice, nullptr, true);

			if (StarlightState == EState::Alive)
			{
				GetWorld()->GetTimerManager().SetTimer(
					SwitchCameraHandle,
					[this, PlayerController]() {
						SwitchCamera(PlayerController);
					},
					1.0f,
					false);
			}

			ReviveTimer = 0.0f;
			ReviveTimerMax = AutoReviveTime;
			GetWorld()->GetTimerManager().SetTimer(
				GameOverTimerHandle,
				this,
				&UReviveManager::ReviveSingleUIUpdate,
				0.1f,
				true);
		}
	}
}

void UReviveManager::HandleDeath(ADuoQCharacter* Character)
{
	if (!Character)
	{
		return;
	}

	// Determine whether the character is Player or Starlight
	if (Character->IsA(ADuoQPlayerCharacter::StaticClass()))
	{
		// hide gameplay hud
		UDuoQBlueprintFunctionLibrary::GetHUDInstance(GetWorld())->GameplayHUD->SetVisibility(ESlateVisibility::Collapsed);
		PlayerState = EState::Dead;
		GetWorld()->GetTimerManager().SetTimer(AutoPlayerReviveTimerHandle, this, &UReviveManager::AutoPlayerReviveWrapper, AutoReviveTime, false);

		if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0))
		{
			FadeToBlack(PlayerController);

			// If we want starlight to autorevive - uncomment below
			// GetWorld()->GetTimerManager().SetTimer(
			//	ReviveTimerHandle,
			//	this,
			//	&UReviveManager::RevivePlayer,
			//	4.0f,
			//	false
			//);
		}
	}
	else if (Character->IsA(AStarlightCharacter::StaticClass()))
	{
		StarlightState = EState::Dead;
		GetWorld()->GetTimerManager().SetTimer(AutoStarlightReviveTimerHandle, this, &UReviveManager::AutoStarlightReviveWrapper, AutoReviveTime, false);
	}

	// Check if both player and Starlight are dead
	if (PlayerState == EState::Dead && StarlightState == EState::Dead)
	{
		GameOver();
	}
}

void UReviveManager::RevivePlayer(bool timer)
{
	if (PlayerState == EState::Alive)
	{
		return;
	}

	PlayerState = EState::Alive;
	GetWorld()->GetTimerManager().ClearTimer(AutoPlayerReviveTimerHandle);
	// RemoveFadeToBlack();
	ADuoQPlayerCharacter* PlayerCharacter = Cast<ADuoQPlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	if (PlayerCharacter)
	{
		if (PlayerSoul)
		{
			FVector PlayerLocation = PlayerSoul->GetActorLocation();
			PlayerSoul->Destroy();
			PlayerSoul = nullptr;
			PlayerCharacter->SetActorLocation(PlayerLocation);
		}

		AActor* PlayerStart = FindPlayerStartForController(PlayerCharacter->GetController());
		if (PlayerStart)
		{
			PlayerCharacter->SetActorLocation(PlayerStart->GetActorLocation());
			PlayerCharacter->SetActorRotation(PlayerStart->GetActorRotation());
		}

		PlayerCharacter->RevivePlayer();

		// If we want starlight to autorevive - uncomment below
		// TeleportStarlightToPlayer();
		UGameplayStatics::PlaySound2D(this, ReviveSound);
		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (PlayerController)
		{
			SwitchCameraBack(PlayerController); // Switch camera back to the player
		}

		// show gameplay hud
		UDuoQBlueprintFunctionLibrary::GetHUDInstance(GetWorld())->GameplayHUD->SetVisibility(ESlateVisibility::Visible);

		if (UAudioSubsystem* Audio = GetWorld()->GetGameInstance()->GetSubsystem<UAudioSubsystem>())
		{
			Audio->OnGameOver();
		}
	}

	if (timer == true)
	{
		// TeleportPlayerToStarlight();
	}
}

void UReviveManager::ReviveStarlight(bool timer)
{
	if (StarlightState == EState::Alive)
	{
		return;
	}
	GetWorld()->GetTimerManager().ClearTimer(AutoStarlightReviveTimerHandle);
	StarlightState = EState::Alive;
	AStarlightCharacter* StarlightCharacter = Cast<AStarlightCharacter>(UGameplayStatics::GetActorOfClass(GetWorld(), AStarlightCharacter::StaticClass()));

	if (StarlightCharacter)
	{
		if (StarlightSoul)
		{
			FVector StarlightLocation = StarlightSoul->GetActorLocation();
			StarlightSoul->Destroy();
			StarlightSoul = nullptr;
		}

		AActor* PlayerStart = FindPlayerStartForController(StarlightCharacter->GetController());
#if WITH_EDITOR
		UE_LOG(LogTemp, Error, TEXT("Moving Starlight To %s at %s"), *PlayerStart->GetActorLabel(), *(PlayerStart->GetActorLocation().ToString()));
#endif
		if (PlayerStart)
		{
			StarlightCharacter->SetActorLocation(PlayerStart->GetActorLocation());
			StarlightCharacter->SetActorRotation(PlayerStart->GetActorRotation());
		}
		UGameplayStatics::PlaySoundAtLocation(this, ReviveSound, PlayerStart->GetActorLocation());
		StarlightCharacter->ReviveStarlight();
		UDuoQBlueprintFunctionLibrary::GetHUDInstance(GetWorld())->GameplayHUD->HideReviveText();
	}

	if (timer)
	{
		// TeleportStarlightToPlayer();
	}
}

void UReviveManager::RemoveFadeToBlack()
{
	if (FadeWidgetInstance)
	{
		FOutputDeviceNull OutputDevice;
		FadeWidgetInstance->CallFunctionByNameWithArguments(TEXT("FadeOutAnimation"), OutputDevice, nullptr, true);

		GetWorld()->GetTimerManager().SetTimer(
			ReviveTimerHandle,
			[this] { FadeWidgetInstance->SetVisibility(ESlateVisibility::Collapsed); },
			AutoReviveTime - 1,
			false,
			AutoReviveTime - 1);
	}
}

void UReviveManager::TeleportStarlightToPlayer()
{
	ADuoQPlayerCharacter* PlayerCharacter = Cast<ADuoQPlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	AStarlightCharacter*  StarlightCharacter = Cast<AStarlightCharacter>(UGameplayStatics::GetActorOfClass(GetWorld(), AStarlightCharacter::StaticClass()));

	if (PlayerCharacter && StarlightCharacter)
	{
		FVector	 PlayerLocation = PlayerCharacter->GetActorLocation();
		FRotator PlayerRotation = PlayerCharacter->GetActorRotation();

		// Define search parameters
		float SearchRadius = 300.0f;   // Radius of the sphere for random location selection
		int	  MaxAttempts = 10;		   // Max number of attempts to find a valid location
		float CollisionRadius = 30.0f; // Radius of the collision sphere check

		FHitResult			  HitResult;
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(PlayerCharacter);
		QueryParams.AddIgnoredActor(StarlightCharacter);

		FVector TeleportLocation;
		bool	bFoundSafeLocation = false;

		for (int i = 0; i < MaxAttempts; i++)
		{
			// Generate a random point within a sphere
			FVector RandomOffset = FMath::VRand() * FMath::FRandRange(0, SearchRadius);
			FVector ProposedLocation = PlayerLocation + RandomOffset;

			// Check for collision at the proposed location
			bool bIsBlocked = GetWorld()->SweepSingleByChannel(
				HitResult,
				ProposedLocation,
				ProposedLocation,
				FQuat::Identity,
				ECC_Visibility, // Adjust collision channel if needed
				FCollisionShape::MakeSphere(CollisionRadius),
				QueryParams);

			if (!bIsBlocked)
			{
				TeleportLocation = ProposedLocation;
				bFoundSafeLocation = true;
				break;
			}
		}

		// If a safe location was found, teleport Starlight
		if (bFoundSafeLocation)
		{
			StarlightCharacter->SetActorLocation(TeleportLocation);

			// Ensure Starlight faces the player
			FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(TeleportLocation, PlayerLocation);
			StarlightCharacter->SetActorRotation(LookAtRotation);
		}
		else
		{
			// Fallback in case no safe location is found
			StarlightCharacter->SetActorLocation(PlayerLocation + PlayerRotation.Vector() * 200.0f);
		}

		// Optional: PlayerCharacter->RevivePlayer();
	}
}

void UReviveManager::TeleportPlayerToStarlight()
{
	ADuoQPlayerCharacter* PlayerCharacter = Cast<ADuoQPlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	AStarlightCharacter*  StarlightCharacter = Cast<AStarlightCharacter>(UGameplayStatics::GetActorOfClass(GetWorld(), AStarlightCharacter::StaticClass()));

	if (PlayerCharacter && StarlightCharacter)
	{
		FVector	 StarlightLocation = StarlightCharacter->GetActorLocation();
		FRotator StarlightRotation = StarlightCharacter->GetActorRotation();
		FVector	 ForwardVector = StarlightRotation.Vector(); // This is the forward direction of the starlight

		FVector PlayerLocation = StarlightLocation + (ForwardVector * 200.0f);

		PlayerCharacter->SetActorLocation(PlayerLocation);

		FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(PlayerLocation, StarlightLocation);
		PlayerCharacter->SetActorRotation(LookAtRotation);
	}
}

void UReviveManager::SwitchCamera(APlayerController* PlayerController)
{
	AStarlightCharacter* StarlightCharacter = Cast<AStarlightCharacter>(UGameplayStatics::GetActorOfClass(GetWorld(), AStarlightCharacter::StaticClass()));

	ADuoQPlayerCharacter* PlayerCharacter = Cast<ADuoQPlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (PlayerCharacter)
	{
		FVector FarAwayLocation = FVector(100000.0f, 100000.0f, 100000.0f);
		PlayerCharacter->SetActorLocation(FarAwayLocation);
	}

	if (StarlightCharacter && StarlightCharacter->GetFollowCamera())
	{
		StarlightCharacter->GetFollowCamera()->Activate();
		// Set the view target to Starlight's camera with a smooth blend
		PlayerController->SetViewTargetWithBlend(StarlightCharacter, FadeTimeCameraSwitch); // Blend over 1 second
		GetWorld()->GetTimerManager().ClearTimer(ReviveTimerHandle);
		RemoveFadeToBlack();
		PlayerCharacter->GetFirstPersonCameraComponent()->Deactivate();
	}
}

void UReviveManager::SwitchCameraBack(APlayerController* PlayerController)
{
	ADuoQPlayerCharacter* PlayerCharacter = Cast<ADuoQPlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	if (PlayerController && PlayerCharacter)
	{
		if (ReviveWidget)
		{
			// Create the widget and add it to the viewport
			ReviveInstance = UDuoQBlueprintFunctionLibrary::GetHUDInstance(GetWorld())->FadeToBlack;
			// CreateWidget<UUserWidget>(PlayerController, ReviveWidget);
			if (ReviveInstance)
			{
				// ReviveInstance->AddToViewport();
				ReviveInstance->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
				FOutputDeviceNull OutputDevice;
				ReviveInstance->CallFunctionByNameWithArguments(TEXT("FadeOutAnimation"), OutputDevice, nullptr, true);
			}
		}

		PlayerCharacter->GetFirstPersonCameraComponent()->Activate();
		// Set the view target back to the player's camera with a smooth blend
		PlayerController->SetViewTarget(PlayerCharacter); // Blend over 1 second
		AStarlightCharacter* StarlightCharacter = Cast<AStarlightCharacter>(UGameplayStatics::GetActorOfClass(GetWorld(), AStarlightCharacter::StaticClass()));
		if (StarlightCharacter)
		{
			StarlightCharacter->GetFollowCamera()->Deactivate();
		}
	}
}

void UReviveManager::AutoStarlightReviveWrapper()
{
	ReviveStarlight(true);
}

void UReviveManager::AutoPlayerReviveWrapper()
{
	RevivePlayer(true);
}

AActor* UReviveManager::FindPlayerStartForController(AController* Controller)
{
	if (!Controller)
	{
		return nullptr;
	}

	AGameModeBase* GameModeBase = UGameplayStatics::GetGameMode(GetWorld());
	if (GameModeBase)
	{
		// Cast to your BP version
		ADuoQGameMode* BPGameMode = Cast<ADuoQGameMode>(GameModeBase);
		if (BPGameMode)
		{
			BPGameMode->bDiedViaCombat = true;
			AActor* returnVal = BPGameMode->FindPlayerStart(Controller);
			BPGameMode->bDiedViaCombat = false;
			return returnVal;
		}
	}

	return nullptr;
}