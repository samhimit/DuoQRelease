#pragma once
#include "CoreMinimal.h"
#include "GameFramework/DamageType.h"
#include "RegularDamageType.generated.h"

UCLASS(const, BluePrintable, BlueprintType)
class URegularDamageType : public UDamageType
{
	GENERATED_BODY()

public:
	URegularDamageType(const FObjectInitializer& initializer);
};