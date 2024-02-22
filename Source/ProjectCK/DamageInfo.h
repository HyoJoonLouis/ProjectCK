#pragma once

#include "CoreMinimal.h"
#include "DamageInfo.generated.h"

UENUM()
enum class EDamageType : uint8
{
	NONE	UMETA(DisplayName = "None"),
	MELEE	UMETA(DisplayName = "Melee"),
	PROJECTILE	UMETA(DisplayName = "Projectile")
};

UENUM()
enum class EDamageResponse : uint8
{
	NONE	UMETA(DisplayName = "None"),
	DEFAULT	UMETA(DisplayName = "Default"),
	STUN	UMETA(DisplayName = "Stun")
};

USTRUCT(BlueprintType)
struct FDamageInfo
{
public:
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	float Amount;

	UPROPERTY(EditAnywhere)
	EDamageType DamageType;

	UPROPERTY(EditAnywhere)
	EDamageResponse DamageResponse;

	UPROPERTY(EditAnywhere)
	bool ShouldDamageInvincible;

	UPROPERTY(EditAnywhere)
	bool CanParry;

	UPROPERTY(EditAnywhere)
	bool CanDodge;

	UPROPERTY(EditAnywhere)
	bool ForceInterrupt;
};