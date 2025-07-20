// Copyright 2024


#include "ImportCSVAssetActionUtility.h"

#include "AssetToolsModule.h"
#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"
#include "VaRestSubsystem.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetRegistry/IAssetRegistry.h"
#include "Factories/DataTableFactory.h"
#include "GameSystems/Dialogue/DialogueSubsystem.h"
#include "UObject/SavePackage.h"

UImportCSVAssetActionUtility::UImportCSVAssetActionUtility()
{
	SupportedClasses.Emplace(UDataTable::StaticClass());
	InitializeVO();
}

void UImportCSVAssetActionUtility::ImportCSVs()
{
	UE_LOG(LogTemp,Display,TEXT("importing CSVs"));

	if(UVaRestSubsystem* VaRestSubsystem = GEngine->GetEngineSubsystem<UVaRestSubsystem>())
	{
		TObjectPtr<UVaRestRequestJSON> SheetsIDRequest;
		SheetsIDRequest = VaRestSubsystem->ConstructVaRestRequestExt(EVaRestRequestVerb::GET,EVaRestRequestContentType::x_www_form_urlencoded_url);
		SheetsIDRequest->OnRequestComplete.AddDynamic(this,&UImportCSVAssetActionUtility::CompleteSheetIDRequest);
		SheetsIDRequest->OnRequestFail.AddDynamic(this,&UImportCSVAssetActionUtility::FailRequest);
		//VaRestRequest->SetHeader(TEXT("key"),TEXT("AIzaSyC_zk0a8H6OBvw4fmNB227IbmIFg8GUs1Y");
		SheetsIDRequest->ProcessURL(TEXT("https://sheets.googleapis.com/v4/spreadsheets/1vuALzCZNSnOiQFttnblOHnhNNOAmKvZJ8rzs1vSCNgQ?&fields=sheets.properties&key=AIzaSyC_zk0a8H6OBvw4fmNB227IbmIFg8GUs1Y"));
	}
}

void UImportCSVAssetActionUtility::ImportSingleSheet(UDataTable* DataTable)
{
	if(UVaRestSubsystem* VaRestSubsystem = GEngine->GetEngineSubsystem<UVaRestSubsystem>())
	{
		TObjectPtr<UVaRestRequestJSON> ContentRequest = VaRestSubsystem->ConstructVaRestRequestExt(EVaRestRequestVerb::GET,EVaRestRequestContentType::x_www_form_urlencoded_url);
		ContentRequest->OnRequestComplete.AddDynamic(this,&UImportCSVAssetActionUtility::CompleteCSVInfoRequest);
		ContentRequest->OnRequestFail.AddDynamic(this,&UImportCSVAssetActionUtility::FailRequest);
		ContentRequest->ProcessURL(TEXT("https://docs.google.com/spreadsheets/d/1vuALzCZNSnOiQFttnblOHnhNNOAmKvZJ8rzs1vSCNgQ/gviz/tq?tqx=out:csv&sheet=")+DataTable->GetName());
	}
}

void UImportCSVAssetActionUtility::CompleteSheetIDRequest(UVaRestRequestJSON* Request)
{
	UE_LOG(LogTemp, Display, TEXT("Google API Succeeded, SheetIDs found"));
	TableNames.Empty();
	if(UVaRestSubsystem* VaRestSubsystem = GEngine->GetEngineSubsystem<UVaRestSubsystem>())
	{
		TArray<UVaRestJsonObject*> ResponseObjects = Request->GetResponseObject()->GetObjectArrayField(TEXT("sheets"));
		for(UVaRestJsonObject* Sheet : ResponseObjects)
		{
			if(UVaRestJsonObject* Properties = Sheet->GetObjectField(TEXT("properties")))
			{
				FString SheetTitle = Properties->GetStringField(TEXT("title"));
				int SheetID = Properties->GetIntegerField(TEXT("sheetId"));
				//UE_LOG(LogTemp, Display, TEXT("%s"), *SheetTitle);

				TableNames.Emplace(SheetID,SheetTitle);

				TObjectPtr<UVaRestRequestJSON> ContentRequest = VaRestSubsystem->ConstructVaRestRequestExt(EVaRestRequestVerb::GET,EVaRestRequestContentType::x_www_form_urlencoded_url);
				ContentRequest->OnRequestComplete.AddDynamic(this,&UImportCSVAssetActionUtility::CompleteCSVInfoRequest);
				ContentRequest->OnRequestFail.AddDynamic(this,&UImportCSVAssetActionUtility::FailRequest);
				ContentRequest->ProcessURL(TEXT("https://docs.google.com/spreadsheets/d/1vuALzCZNSnOiQFttnblOHnhNNOAmKvZJ8rzs1vSCNgQ/gviz/tq?tqx=out:csv&sheet=")+SheetTitle);
			}
		}
	}
}

void UImportCSVAssetActionUtility::CompleteCSVInfoRequest(UVaRestRequestJSON* Request)
{
	const FString URL = Request->GetURL();
	TArray<FString> CutUpURL;
	URL.ParseIntoArray(CutUpURL,TEXT("="));
	FString StringID = CutUpURL[2];
	//TableNames.Find(FCString::Atoi(*StringID))
	if(FString* TableName = &StringID)
	{
		FAssetData TableAsset;
		IAssetRegistry::Get()->TryGetAssetByObjectPath(FSoftObjectPath(FString::Printf(TEXT("/Game/DataTables/Shipping/%s.%s"),**TableName,**TableName)),TableAsset);
		UDataTable* Table;
		if (Table = Cast<UDataTable>(TableAsset.GetAsset()); Table)
		{
			Table->CreateTableFromCSVString(Request->GetResponseContentAsString());
			SetVoiceLines(Table);
			Table->Modify();
		}
		else
		{
			// Load necessary modules
			FAssetToolsModule& AssetToolsModule = FModuleManager::Get().LoadModuleChecked<FAssetToolsModule>("AssetTools");
			FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
			FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
			IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

			// Generate a unique asset name
			FString Name, PackageName;
			AssetToolsModule.Get().CreateUniqueAssetName(TEXT("/Game/DataTables/Shipping/")+*TableName, TEXT(""), PackageName, Name);
			const FString PackagePath = FPackageName::GetLongPackagePath(PackageName);

			// Create object and package
			UPackage* package = CreatePackage(*PackageName);
			UDataTableFactory* MyFactory = NewObject<UDataTableFactory>(UDataTableFactory::StaticClass());
			MyFactory->Struct = FDollar::StaticStruct();
			UObject* NewDataTable = AssetToolsModule.Get().CreateAsset(Name, PackagePath, UDataTable::StaticClass(), MyFactory);
			UPackage::Save(package, NewDataTable, *FPackageName::LongPackageNameToFilename(PackageName, FPackageName::GetAssetPackageExtension()),FSavePackageArgs());

			// Inform asset registry
			AssetRegistry.AssetCreated(NewDataTable);

			// Tell content browser to show the newly created asset
			TArray<UObject*> Objects;
			Objects.Add(NewDataTable);
			ContentBrowserModule.Get().SyncBrowserToAssets(Objects);

			Table = Cast<UDataTable>(NewDataTable);
			Table->CreateTableFromCSVString(Request->GetResponseContentAsString());
			SetVoiceLines(Table);
			Table->Modify();
		}
		// validate table
		if(Table)
		{
			TArray<FDollar*> Dollars;
			Table->GetAllRows<FDollar>(TEXT(""), Dollars);
			for(FDollar* Dollar : Dollars)
			{
				if(Dollar)
				{
					for(FString NextDollar : Dollar->NextDollars)
					{
						FDollar* PossibleDollar = Table->FindRow<FDollar>(FName(*NextDollar), "");
						if(!PossibleDollar)
						{
							UE_LOG(LogTemp, Error, TEXT("%s is not found in table %s"), *NextDollar,**TableName);
						}
					}
				}
			}
			
		}
	}
}

void UImportCSVAssetActionUtility::FailRequest(UVaRestRequestJSON* Request)
{
	UE_LOG(LogTemp, Display, TEXT("Google API Failed"));
	UE_LOG(LogTemp, Error, TEXT("%s"),*Request->GetResponseContentAsString());
}

void UImportCSVAssetActionUtility::InitializeVO()
{
	VOAssets.Empty();

	TArray<FAssetData> assets;
	IAssetRegistry::Get()->GetAssetsByPath(FName(TEXT("/Game/Sounds/VO")), assets);
	for (FAssetData asset : assets)
	{
		if (USoundBase* VO = Cast<USoundBase>(asset.GetAsset()))
		{
			VOAssets.Add(VO->GetFName(), VO);
		}
	}

	UE_LOG(LogTemp, Display, TEXT("Voice lines found - %d"), VOAssets.Num());
}

void UImportCSVAssetActionUtility::SetVoiceLines(UDataTable* DataTable)
{
	TArray<FDollar*> Dollars;
	DataTable->GetAllRows<FDollar>(TEXT("VO Import Action"), Dollars);
	for (FDollar* Dollar : Dollars)
	{
		FString DollarName = Dollar->Name;
		FName	DollarFName(DollarName, EFindName::FNAME_Find);
		FString VOName = DataTable->GetName() + TEXT("-") + DollarName;
		FName	VOFName(VOName, EFindName::FNAME_Find);

		if (USoundBase** VO = VOAssets.Find(FName(VOFName, EFindName::FNAME_Find)))
		{
			Dollar->VoiceOver = *VO;
			FDollar refDollar = *Dollar;
			DataTable->AddRow(DollarFName, refDollar);
		}
		else if (USoundBase** oldVO = VOAssets.Find(FName(DollarFName, EFindName::FNAME_Find)))
		{
			Dollar->VoiceOver = *oldVO;
			FDollar refDollar = *Dollar;
			DataTable->AddRow(DollarFName, refDollar);
			UE_LOG(LogTemp,Error,TEXT("VO LINE MISSING USING BACKUP %s"),*VOFName.ToString());
		}
		else
		{
			UE_LOG(LogTemp,Error,TEXT("Fuck that shit is missing - %s"),*VOName);
		}
	}
}
