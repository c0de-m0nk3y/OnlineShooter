

// Fill out your copyright notice in the Description page of Project Settings.


#include "OnlineShooterCharacter.h"

#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "CharacterAnimInstance.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"


// Sets default values
AOnlineShooterCharacter::AOnlineShooterCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	springArm_1p = CreateDefaultSubobject<USpringArmComponent>(TEXT("springarm1p"));
	springArm_1p->SetupAttachment(RootComponent);
	springArm_1p->AddRelativeLocation(FVector(0, 0, 70));
	springArm_1p->TargetArmLength = 0.f;

	camera_1p = CreateDefaultSubobject<UCameraComponent>(TEXT("camera1p"));
	camera_1p->SetupAttachment(springArm_1p);

	health = MAX_HEALTH;


}

void AOnlineShooterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AOnlineShooterCharacter, health);
	DOREPLIFETIME(AOnlineShooterCharacter, IsMoving);
	DOREPLIFETIME(AOnlineShooterCharacter, IsCrouching);
}

// Called when the game starts or when spawned
void AOnlineShooterCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocallyControlled())
	{
		USkeletalMeshComponent* mesh = (USkeletalMeshComponent*)GetComponentByClass(USkeletalMeshComponent::StaticClass());
		if (mesh == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("mesh was null"));
			return;
		}

		mesh->SetVisibility(false);
	}
}



// Called every frame
void AOnlineShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsMoving)
	{
		if (GetVelocity().SizeSquared() == 0)
			IsMoving = false;
	}
	else
	{
		if (GetVelocity().SizeSquared() != 0)
			IsMoving = true;
	}
}



// Called to bind functionality to input
void AOnlineShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	InputComponent->BindAxis("MoveForwards", this, &AOnlineShooterCharacter::MoveForwards);
	InputComponent->BindAxis("MoveRight", this, &AOnlineShooterCharacter::MoveRight);
	InputComponent->BindAxis("LookUp", this, &AOnlineShooterCharacter::LookUp);
	InputComponent->BindAxis("LookRight", this, &AOnlineShooterCharacter::LookRight);

	InputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &AOnlineShooterCharacter::Action_Jump_Pressed);
	InputComponent->BindAction("Jump", EInputEvent::IE_Released, this, &AOnlineShooterCharacter::Action_Jump_Released);
	InputComponent->BindAction("Crouch", EInputEvent::IE_Pressed, this, &AOnlineShooterCharacter::Action_Crouch_Pressed);
	InputComponent->BindAction("Crouch", EInputEvent::IE_Released, this, &AOnlineShooterCharacter::Action_Crouch_Released);
}

void AOnlineShooterCharacter::MoveForwards(float val)
{
	AddMovementInput(GetActorForwardVector(), val, true);
}

void AOnlineShooterCharacter::MoveRight(float val)
{
	AddMovementInput(GetActorRightVector(), val, true);
}

void AOnlineShooterCharacter::LookUp(float val)
{
	if (invertedLookUp)
		val *= -1.f;
	springArm_1p->AddRelativeRotation(FRotator(val, 0, 0));

}

void AOnlineShooterCharacter::LookRight(float val)
{
	AddControllerYawInput(val);
}



void AOnlineShooterCharacter::Action_Jump_Pressed()
{
	Jump();
}

void AOnlineShooterCharacter::Action_Jump_Released()
{
	//handle release of jump button here if desired.
}

void AOnlineShooterCharacter::Action_Crouch_Pressed()
{
	Crouch();


	IsCrouching = true;
	USkeletalMeshComponent* mesh = (USkeletalMeshComponent*)GetComponentByClass(USkeletalMeshComponent::StaticClass());
	if (mesh == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("mesh was null"));
		return;
	}

	UCharacterAnimInstance* anim = (UCharacterAnimInstance*)mesh->GetAnimInstance();
	if (anim == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("animInstance was null"));
		return;
	}

	anim->isCrouching = IsCrouching;
	ServerRPC_IsCrouchingUpdate(true);
}

void AOnlineShooterCharacter::Action_Crouch_Released()
{
	UnCrouch();

	IsCrouching = false;
	USkeletalMeshComponent* mesh = (USkeletalMeshComponent*)GetComponentByClass(USkeletalMeshComponent::StaticClass());
	if (mesh == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("mesh was null"));
		return;
	}

	UCharacterAnimInstance* anim = (UCharacterAnimInstance*)mesh->GetAnimInstance();
	if (anim == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("animInstance was null"));
		return;
	}

	anim->isCrouching = IsCrouching;
	ServerRPC_IsCrouchingUpdate(false);
}

bool AOnlineShooterCharacter::ServerRPC_IsCrouchingUpdate_Validate(bool b)
{
	return true;
}

void AOnlineShooterCharacter::ServerRPC_IsCrouchingUpdate_Implementation(bool b)
{
	if (b)
		Crouch();
	else
		UnCrouch();

	IsCrouching = b;
}

void AOnlineShooterCharacter::OnRep_IsCrouching()
{
	OnIsCrouchingUpdate();
}

void AOnlineShooterCharacter::OnRep_Health()
{
	OnHealthUpdate();
}

void AOnlineShooterCharacter::OnRep_IsMoving()
{
	OnIsMovingUpdate();
}

void AOnlineShooterCharacter::OnIsCrouchingUpdate()
{
	USkeletalMeshComponent* mesh = (USkeletalMeshComponent*)GetComponentByClass(USkeletalMeshComponent::StaticClass());
	if (mesh == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("mesh was null"));
		return;
	}

	UCharacterAnimInstance* anim = (UCharacterAnimInstance*)mesh->GetAnimInstance();
	if (anim == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("animInstance was null"));
		return;
	}

	anim->isCrouching = IsCrouching;
}

void AOnlineShooterCharacter::OnHealthUpdate()
{
	//Client-specific functionality
	if (IsLocallyControlled())
	{
		FString healthMessage = FString::Printf(TEXT("You now have %f health remaining."), health);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, healthMessage);

		if (health <= 0)
		{
			FString deathMessage = FString::Printf(TEXT("You have been killed."));
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, deathMessage);
		}
	}

	//Server-specific functionality
	if (GetLocalRole() == ROLE_Authority)
	{
		FString healthMessage = FString::Printf(TEXT("%s now has %f health remaining."), *GetFName().ToString(), health);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, healthMessage);
	}

	//Functions that occur on all machines. 
	/*
		Any special functionality that should occur as a result of damage or death should be placed here.
	*/
}

void AOnlineShooterCharacter::OnIsMovingUpdate()
{
	//Client-specific functionality
	if (IsLocallyControlled())
	{
		FString m = FString::Printf(TEXT("moving status updated."));
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, m);
	}

	//Server-specific functionality
	if (GetLocalRole() == ROLE_Authority)
	{
		FString m = FString::Printf(TEXT("%s moving status updated."), *GetFName().ToString());
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, m);
	}

	//Functions that occur on all machines. 
	/*
		Any special functionality that should occur as a result of variable update should be placed here.
	*/
	USkeletalMeshComponent* mesh = (USkeletalMeshComponent*)GetComponentByClass(USkeletalMeshComponent::StaticClass());
	if (mesh == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("mesh was null"));
		return;
	}

	UCharacterAnimInstance* anim = (UCharacterAnimInstance*)mesh->GetAnimInstance();
	if (anim == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("animInstance was null"));
		return;
	}

	anim->isMoving = IsMoving;
}
