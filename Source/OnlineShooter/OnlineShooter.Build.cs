// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class OnlineShooter : ModuleRules
{
	public OnlineShooter(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "OnlineSubsystem", "OnlineSubsystemSteam" });

    }
}
