// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class GP3_UEFPS : ModuleRules
{
	public GP3_UEFPS(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput" });
	}
}
