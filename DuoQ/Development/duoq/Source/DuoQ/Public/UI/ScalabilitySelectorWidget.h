// Copyright 2024

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ScalabilitySelectorWidget.generated.h"

class UComboBoxString;
/**
 * 
 */
UCLASS()
class DUOQ_API UScalabilitySelectorWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UComboBoxString> ScalabilityComboBox;

	UFUNCTION()
	void OnScalabilitySelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

protected:
	virtual void NativeConstruct() override;

private:
	void SetEngineScalability(FString SelectedItem);
};
