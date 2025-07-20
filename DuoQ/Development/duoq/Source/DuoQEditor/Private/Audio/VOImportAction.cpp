// Copyright 2024

#ifdef WITH_EDITOR
	#include "DuoQEditor/Public/Audio/VOImportAction.h"
#endif

#include "AssetRegistry/IAssetRegistry.h"
#include "GameSystems/Dialogue/DialogueSubsystem.h"

bool UVOImportAction::Apply_Implementation(UFactory* Factory, UObject* CreatedObject)
{
#ifdef WITH_EDITOR
	if (!IsValid(CreatedObject))
	{
		return false;
	}
	bool result = false;
	if (USoundWave* sound = Cast<USoundWave>(CreatedObject))
	{
		sound->SoundClassObject = VoiceClass;
		sound->SoundGroup = ESoundGroup::SOUNDGROUP_Voice;
		sound->PreEffectBusSends = PreEffectBusSends;
		sound->BusSends = BusSends;
		sound->SourceEffectChain = VoiceFX;

		//result = true;
		
		FString tableName = "";
		FString dollarName = "";
		if (CreatedObject->GetName().Contains(TEXT("-")))
		{
			CreatedObject->GetName().Split(TEXT("-"), &tableName, &dollarName);
			result = SetVoiceOver(sound, tableName, dollarName);
		}
		else
		{
			dollarName = CreatedObject->GetName();
			result = SetVoiceOver(sound, dollarName);
		}
	}
	if (!result)
	{
		UE_LOG(LogTemp, Warning, TEXT("Could not assign %s"), *CreatedObject->GetName());
	}
	return result;
#endif
}

bool UVOImportAction::SetVoiceOver(USoundWave* sound, FString tableName, FString dollarName)
{
#ifdef WITH_EDITOR
	if (tableName == "")
	{
		return false;
	}
	// FName tableFName = FName(*tableName, EFindName::FNAME_Find);
	FName dollarFName = FName(*dollarName, EFindName::FNAME_Find);

	TArray<FAssetData> assets;
	IAssetRegistry::Get()->GetAssetsByPath(FName(TEXT("/Game/DataTables/Shipping")), assets, true);

	for (FAssetData asset : assets)
	{
		if (UDataTable* table = Cast<UDataTable>(asset.GetAsset()))
		{
			if (table->GetName() == tableName)
			{
				if (FDollar* dollar = table->FindRow<FDollar>(dollarFName, FString(TEXT("VO Import Action"))))
				{
					dollar->VoiceOver = sound;
					FDollar refDollar = *dollar;
					table->AddRow(dollarFName, refDollar);
					table->Modify();
					return true;
				}
			}
		}
	}

	return false;
#endif
}

bool UVOImportAction::SetVoiceOver(USoundWave* sound, FString dollarName)
{
#ifdef WITH_EDITOR
	// FName tableFName = FName(*tableName, EFindName::FNAME_Find);
	FName dollarFName = FName(*dollarName, EFindName::FNAME_Find);

	TArray<FAssetData> assets;
	IAssetRegistry::Get()->GetAssetsByPath(FName(TEXT("/Game/DataTables/Shipping")), assets, true);

	bool result = false;
	// Do not know table name, so go through every table
	// Can assign to multiple tables
	for (FAssetData asset : assets)
	{
		if (UDataTable* table = Cast<UDataTable>(asset.GetAsset()))
		{
			if (FDollar* dollar = table->FindRow<FDollar>(dollarFName, FString(TEXT("VO Import Action"))))
			{
				dollar->VoiceOver = sound;
				FDollar refDollar = *dollar;
				table->AddRow(dollarFName, refDollar);
				table->Modify();
				result = true;
			}
		}
	}

	return result;
#endif
}
