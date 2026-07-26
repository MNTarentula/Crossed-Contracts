// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class K7 : ModuleRules
{
	public K7(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "UMG","Slate",
"SlateCore","AIModule",
    "NavigationSystem" });
	}
}
