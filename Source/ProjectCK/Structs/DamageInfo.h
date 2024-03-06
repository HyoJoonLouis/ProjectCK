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

USTRUCT(Blueprintable)
struct FDamageInfo
{
public:
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Amount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EDamageType DamageType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EDamageResponse DamageResponse;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool ShouldDamageInvincible;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool CanParry;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool CanDodge;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool ForceInterrupt;
};