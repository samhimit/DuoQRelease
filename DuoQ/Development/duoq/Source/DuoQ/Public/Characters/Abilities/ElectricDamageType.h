#pragma once
#include "CoreMinimal.h"
#include "GameFramework/DamageType.h"
#include "ElectricDamageType.generated.h"


UCLASS(const, BluePrintable, BlueprintType) 
class UElectricDamageType : public UDamageType
{
	GENERATED_BODY()

public:
	UElectricDamageType(const FObjectInitializer& initializer);
};