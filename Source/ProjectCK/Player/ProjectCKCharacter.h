#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "../Interfaces/DamagableInterface.h"
#include "ProjectCKCharacter.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class AProjectCKCharacter : public ACharacter, public IDamagableInterface
{
	GENERATED_BODY()
protected:
	// Camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
	
	// Actions
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* JumpAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* WeakAttackAction;

	// Montages
	UPROPERTY(EditAnywhere, Category = Montages)
	class UAnimMontage* WeakAttackMontage;
	UPROPERTY(EditAnywhere, Category = Montages)
	class UAnimMontage* TakeDamageMontage;

	// Components
	UPROPERTY(EditAnywhere)
	class UDamageSystemComponent* DamageSystemComponent;

	// UI
	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> HUDClass;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UUserWidget* HUD;

public:
	AProjectCKCharacter();

	// DamagableInterface
	virtual float GetCurrentHealth_Implementation() override;
	virtual float GetMaxHealth_Implementation() override;
	virtual void Heal_Implementation(float Amount) override;
	virtual bool TakeDamage_Implementation(FDamageInfo DamageInfo) override;
	virtual bool IsDead_Implementation() override;
	virtual bool IsAttacking_Implementation() override;

	virtual bool ReserveAttackToken_Implementation(int Amount) override;
	virtual void ReturnAttackToken_Implementation(int Amount) override;

protected:
	// Action Functions
	UFUNCTION()
	void Move(const struct FInputActionValue& Value);
	UFUNCTION()
	void Look(const struct FInputActionValue& Value);
	UFUNCTION()
	void WeakAttack(const struct FInputActionValue& Value);
			
protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void BeginPlay();

public:
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

