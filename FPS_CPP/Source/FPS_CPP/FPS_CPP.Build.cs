// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class FPS_CPP : ModuleRules
{
	public FPS_CPP(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay" });
	}
}
