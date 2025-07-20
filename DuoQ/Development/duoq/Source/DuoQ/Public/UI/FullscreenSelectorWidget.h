// Copyright 2024

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FullscreenSelectorWidget.generated.h"

class UComboBoxString;
/**
 * 
 */
UCLASS()
class DUOQ_API UFullscreenSelectorWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UComboBoxString> FullscreenComboBox;

	UFUNCTION()
	void OnFullscreenSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

protected:
	virtual void NativeConstruct() override;
	
};
