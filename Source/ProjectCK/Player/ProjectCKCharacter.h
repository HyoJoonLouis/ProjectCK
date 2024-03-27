#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "../Interfaces/DamagableInterface.h"
#include <Components/TimelineComponent.h>
#include "ProjectCKCharacter.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UENUM(BlueprintType)
enum class EPlayerStates : uint8
{
	PASSIVE		UMETA(DisplayName = "Passive"),
	ATTACKING	UMETA(DisplayName = "Attacking"),
	DODGE		UMETA(DisplayName = "Dodge")
};


UCLASS(config=Game)
class AProjectCKCharacter : public ACharacter, public IDamagableInterface
{
	GENERATED_BODY()
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon)
	class UStaticMeshComponent* Weapon;
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
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* DodgeAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* TargetingAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* SprintAction;

	// FSM
	EPlayerStates CurrentState;

	// Movement
	TMap<enum EMovementSpeed, float> MovementSpeed;

	// Attack
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Attack)
	int AttackIndex;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Attack)
	AActor* TargetActor;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Attack)
	AActor* SoftTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attack)
	UCurveFloat* TargetRotateCurve;
	FTimeline TargetRotateTimeline;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Attack)
	FDamageInfo CurrentDamageInfo;
	UPROPERTY(VisibleAnywhere, Category = Attack)
	TArray<AActor*> AlreadyHitActors;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	TSubclassOf<class UCameraShakeBase> SmallCameraShake;

	TArray<AActor*> DilationActors;
	FTimerHandle HitStopTimer;
	FTimerHandle DodgeStopTimer;

	// Montages
	UPROPERTY(EditAnywhere, Category = Attack)
	TArray<UAnimMontage*> LeftAttackMontages;
	UPROPERTY(EditAnywhere, Category = Attack)
	TArray<UAnimMontage*> RightAttackMontages;
	UPROPERTY(EditAnywhere, Category = Attack)
	class UAnimMontage* DodgeMontage;
	UPROPERTY(EditAnywhere, Category = Attack)
	class UAnimMontage* TakeDamageMontage;

	UPROPERTY(VisibleAnywhere, Category = Attack)
	FAttackInfo AttackInfo;

	bool bSaveDodge;
	bool bActivateCollision;

	// Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UDamageSystemComponent* DamageSystemComponent;

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
	FORCEINLINE bool CheckCurrentState(TArray<EPlayerStates> State) const {return State.Contains(CurrentState);};

	// Action Functions
	UFUNCTION()
	void Move(const struct FInputActionValue& Value);
	UFUNCTION()
	void Look(const struct FInputActionValue& Value);
	UFUNCTION()
	void LeftMouse(const struct FInputActionValue& Value);
	UFUNCTION()
	void RightMouse(const struct FInputActionValue& Value);
	UFUNCTION()
	void Dodge(const struct FInputActionValue& Value);
	UFUNCTION()
	void Targeting(const struct FInputActionValue& Value);
	UFUNCTION()
	void Sprint(const struct FInputActionValue& Value);
	UFUNCTION()
	void StopSprint(const struct FInputActionValue& Value);
	
	// Attack
	UFUNCTION(BlueprintCallable)
	void Attack(EAttackType AttackType);
	
	UFUNCTION(BlueprintCallable)
	void PerformLeftAttack(int Index);
	UFUNCTION(BlueprintCallable)
	void PerformRightAttack(int Index);
	UFUNCTION(BlueprintCallable)
	void PerformDodge();

	UFUNCTION(BlueprintCallable)
	void SaveAttack();
	UFUNCTION(BlueprintCallable)
	void SaveDodge();
	
	UFUNCTION(BlueprintCallable)
	void SoftLock();
	UFUNCTION(BlueprintCallable)
	void RotateToTarget();
	UFUNCTION()
	void RotateToTargetTimelineFunction(float Value);

	UFUNCTION(BlueprintCallable)
	void ResetState();

	UFUNCTION(BlueprintCallable)
	void StartWeaponCollision();
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

