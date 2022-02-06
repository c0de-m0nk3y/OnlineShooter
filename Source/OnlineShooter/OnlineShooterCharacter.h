// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "OnlineShooterCharacter.generated.h"

UCLASS()
class ONLINESHOOTER_API AOnlineShooterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AOnlineShooterCharacter();

	/** Property replication */
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		class USpringArmComponent* springArm_1p;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		class UCameraComponent* camera_1p;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool invertedLookUp = false;



	const float MAX_HEALTH = 100;

	UPROPERTY(ReplicatedUsing = OnRep_Health)
		float health;

	UFUNCTION()
		void OnRep_Health();

	void OnHealthUpdate();



	UPROPERTY(ReplicatedUsing = OnRep_IsMoving)
		bool IsMoving = false;

	UFUNCTION()
		void OnRep_IsMoving();

	void OnIsMovingUpdate();



	UPROPERTY(ReplicatedUsing = OnRep_IsCrouching)
		bool IsCrouching = false;

	UFUNCTION()
		void OnRep_IsCrouching();

	void OnIsCrouchingUpdate();

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerRPC_IsCrouchingUpdate(bool b);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void MoveForwards(float val);
	void MoveRight(float val);
	void LookUp(float val);
	void LookRight(float val);

	void Action_Jump_Pressed();
	void Action_Jump_Released();
	void Action_Crouch_Pressed();
	void Action_Crouch_Released();


};
