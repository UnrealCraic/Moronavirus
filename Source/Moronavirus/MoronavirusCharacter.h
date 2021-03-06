// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MoronavirusCharacter.generated.h"

UCLASS(config=Game)
class AMoronavirusCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(EditDefaultsOnly, Category = "FX")
	class USoundCue* GrindRailSound;

	UPROPERTY(EditDefaultsOnly, Category = "FX")
	class UNiagaraSystem* GrindRailEffect;

	UPROPERTY(EditDefaultsOnly, Category = Weapons)
	TSubclassOf<class AMVWeaponActor> WeaponClass = nullptr;

	class UNiagaraComponent* GrindRailEffectComponent;

	class UAudioComponent* AudioLoopComponent;
public:
	AMoronavirusCharacter();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	class AMVWeaponActor* GetCurrentWeapon() const { return CurrentWeapon; }

	UFUNCTION(BlueprintCallable)
	int32 GetMoney() const { return Money; }

	UFUNCTION(BlueprintCallable)
	void SetMoney(int32 val) { Money = val; }

	UFUNCTION(BlueprintCallable)
	void AddMoney(int32 val) { Money += val; }

	virtual void Jump() override;

	UFUNCTION(BlueprintPure)
	float GetGrindRailSpeed() const { return GrindRailSpeed; }

protected:

	/** Resets HMD orientation in VR. */
	void OnResetVR();

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

	void ActivateGrindEffects(bool Activate);

	virtual void BeginPlay() override;

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface


protected:

	UPROPERTY(EditDefaultsOnly, Category = Grinding)
	float GrindRailSpeed = 3.0f;

	bool IsGrinding = false;

	int32 Money;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	bool GetIsGrinding() const { return IsGrinding; }
	void SetIsGrinding(bool val);

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = "true"))
	class AMVWeaponActor* CurrentWeapon = nullptr;
};

