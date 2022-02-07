// Fill out your copyright notice in the Description page of Project Settings.


#include "OnShooter_GameInstance.h"

#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"


UOnShooter_GameInstance::UOnShooter_GameInstance()
{

}

void UOnShooter_GameInstance::Init()
{
	UE_LOG(LogTemp, Warning, TEXT("INIT"));
	IOnlineSubsystem* subSystem = IOnlineSubsystem::Get();
	if (subSystem == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("ERROR: subSystem was nullptr"));
		return;
	}

	sessionInterface = subSystem->GetSessionInterface();
	if (sessionInterface.IsValid())
	{
		//Bind delegates...
		sessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UOnShooter_GameInstance::OnCreateSessionComplete);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ERROR: sessionInterface was not valid"));
		return;
	}

	



}

void UOnShooter_GameInstance::OnCreateSessionComplete(FName ServerName, bool Succeeded)
{
	UE_LOG(LogTemp, Warning, TEXT("OnCreateSessionComplete success: %d"), Succeeded);
	if (Succeeded)
	{
		GetWorld()->ServerTravel("/Game/Scenes/dev_scene?listen", true);
	}
}

void UOnShooter_GameInstance::CreateServer()
{
	UE_LOG(LogTemp, Warning, TEXT("Creating Server now"));

	FOnlineSessionSettings s;
	s.bAllowJoinInProgress = true;
	s.bIsDedicated = false;
	//s.bIsLANMatch = true;
	s.bShouldAdvertise = true;
	s.bUsesPresence = true;
	s.NumPrivateConnections = 5;

	sessionInterface->CreateSession(1, FName("My Session"), s);
}

void UOnShooter_GameInstance::JoinServer()
{
}
