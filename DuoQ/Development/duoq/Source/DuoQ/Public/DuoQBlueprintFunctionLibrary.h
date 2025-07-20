// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/Player/DuoQPlayerCharacter.h"
#include "GameSystems/DuoQGameInstance.h"
#include "GameSystems/DuoQGameMode.h"
#include "Characters/Partner/StarlightCharacter.h"
#include "Characters/Partner/StarlightGOAPController.h"
#include "Characters/Player/DuoQPlayerController.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "UI/DuoQHUD.h"
#include "DuoQBlueprintFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class DUOQ_API UDuoQBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	public:
		UFUNCTION(BlueprintPure, Category = "Utilities", meta=(WorldContext="WorldContextObject"))
		static UDuoQGameInstance* GetDuoQGameInstance(const UObject* WorldContextObject)
		{
			return Cast<UDuoQGameInstance>(UGameplayStatics::GetGameInstance(WorldContextObject));
		}

		UFUNCTION(BlueprintPure, Category = "Utilities", meta=(WorldContext="WorldContextObject"))
		static ADuoQGameMode* GetDuoQGameMode(const UObject* WorldContextObject)
		{
			return Cast<ADuoQGameMode>(UGameplayStatics::GetGameMode(WorldContextObject));
		}

		UFUNCTION(BlueprintPure, Category = "Utilities", meta=(WorldContext="WorldContextObject"))
		static ADuoQPlayerCharacter* GetDuoQPlayer(const UObject* WorldContextObject)
		{
			if(ADuoQGameMode* DuoQGameMode = GetDuoQGameMode(WorldContextObject))
			{
				return DuoQGameMode->Player;
			}
			return nullptr;
		}

		UFUNCTION(BlueprintPure, Category = "Utilities", meta=(WorldContext="WorldContextObject"))
		static ADuoQPlayerController* GetDuoQPlayerController(const UObject* WorldContextObject, int32 PlayerIndex)
		{
			return Cast<ADuoQPlayerController>(UGameplayStatics::GetPlayerController(WorldContextObject,PlayerIndex));
		}

		UFUNCTION(BlueprintPure, Category = "Utilities", meta=(WorldContext="WorldContextObject"))
		static AStarlightCharacter* GetStarlightCharacter(const UObject* WorldContextObject)
		{
			if(ADuoQGameMode* DuoQGameMode = GetDuoQGameMode(WorldContextObject))
			{
				return DuoQGameMode->Starlight;
			}
			return nullptr;
		}

		UFUNCTION(BlueprintPure, Category = "Utilities", meta=(WorldContext="WorldContextObject"))
		static AStarlightGOAPController* GetStarlightGOAPController(const UObject* WorldContextObject)
		{
			if(AStarlightCharacter* StarlightCharacter = UDuoQBlueprintFunctionLibrary::GetStarlightCharacter(WorldContextObject))
			{
				return Cast<AStarlightGOAPController>(StarlightCharacter->GetController());
			}
			return nullptr;
		}

		UFUNCTION(BlueprintCallable, Category = "Utilities", meta=(WorldContext="WorldContextObject"))
		static ETeamAttitude::Type GetTeamAttitudeTowards(const AActor* Other, const AActor* This)
		{
			const IGenericTeamAgentInterface* OtherTeamAgent = Cast<const IGenericTeamAgentInterface>(Other);
			return OtherTeamAgent ? OtherTeamAgent->GetTeamAttitudeTowards(*This)
				: ETeamAttitude::Neutral;
		}

		UFUNCTION(BlueprintPure, Category = "Utilities", meta=(WorldContext="WorldContextObject"))
		static UDuoQHUD* GetHUDInstance(const UObject* WorldContextObject)
		{
			if(ADuoQPlayerCharacter* Character = GetDuoQPlayer(WorldContextObject))
			{
				return Character->PlayerHUD;
			}
			return nullptr;
		}
		
};
