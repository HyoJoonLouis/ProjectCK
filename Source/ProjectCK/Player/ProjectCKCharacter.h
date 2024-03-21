#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "../Interfaces/DamagableInterface.h"
#include "ProjectCKCharacter.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UENUM(BlueprintType)
enum class EPlayerStates : uint8
{
	PASSIVE		UMETA(DisplayName = "Passive"),
	ATTACKING	UMETA(DisplayName = "Attacking")
};

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
	class UInputAction* LeftAttackAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* RightAttackAction;

	// FSM
	EPlayerStates CurrentState;

	// Attack
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Attack)
	int AttackIndex;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Attack)
	AActor* TargetActor;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Attack)
	bool isAttacking;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Attack)
	FDamageInfo CurrentDamageInfo;
	UPROPERTY(VisibleAnywhere, Category = Attack)
	TArray<AActor*> AlreadyHitActors;
	UPROPERTY(EditAnywhere, Category = Attack)
	TArray<UAnimMontage*> LeftAttackMontages;
	UPROPERTY(EditAnywhere, Category = Attack)
	class UAnimMontage* TakeDamageMontage;

	UPROPERTY(EditAnywhere, Category = Attack)
	class UAnimMontage* MotionWarpingMontage;


	// RIght Attack Variables
	bool isRightAttacking;
	UPROPERTY(EditAnywhere, Category = RightAttack)
	float RightAttackTimeDelataion;
	UPROPERTY(EditAnywhere, Category = RightAttack)
	float RightAttackFOV;


	// Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UDamageSystemComponent* DamageSystemComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UMotionWarpingComponent* MotionWarpingComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UNiagaraComponent* NiagaraComponent;

	// UI
	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> HUDClass;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UUserWidget* HUD;

public:
	AProjectCKCharacter();

	// DamagableInterface
	FORCEINLINE virtual float GetCurrentHealth_Implementation() override;
	FORCEINLINE virtual float GetMaxHealth_Implementation() override;
	FORCEINLINE virtual void Heal_Implementation(float Amount) override;
	virtual bool TakeDamage_Implementation(AActor* CauseActor, FDamageInfo DamageInfo) override;
	FORCEINLINE virtual bool IsDead_Implementation() override;
	FORCEINLINE virtual bool IsAttacking_Implementation() override;

	FORCEINLINE virtual bool ReserveAttackToken_Implementation(int Amount) override;
	FORCEINLINE virtual void ReturnAttackToken_Implementation(int Amount) override;

protected:
	// FSM
	UFUNCTION(BlueprintCallable)
	void ChangeState(EPlayerStates NewState);
	UFUNCTION(BlueprintCallable)
	FORCEINLINE EPlayerStates GetState() const { return CurrentState; }
	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool CheckCurrentState(EPlayerStates State) const {return CurrentState == State;};

	// Action Functions
	UFUNCTION()
	void Move(const struct FInputActionValue& Value);
	UFUNCTION()
	void Look(const struct FInputActionValue& Value);
	UFUNCTION()
	void LeftAttack(const struct FInputActionValue& Value);
	UFUNCTION()
	void RightAttackStart(const struct FInputActionValue& Value);
	UFUNCTION()
	void RightAttackEnd(const struct FInputActionValue& Value);

	// Attack
	UFUNCTION(BlueprintCallable)
	void StartWeaponCollision();
	UFUNCTION(BlueprintCallable)
	void TickWeaponCollision();
	UFUNCTION(BlueprintCallable)
	void EndWeaponCollision();
	UFUNCTION(BlueprintCallable)
	void ChangeAttackTarget(AActor* NewTargetActor);

	// Camera
	UFUNCTION(BlueprintCallable)
	void ChangeToContollerDesiredRotation();
	void ChangeToRotationToMovement();
			
protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void BeginPlay();
	virtual void Tick(float DeltaTime);

public:
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

