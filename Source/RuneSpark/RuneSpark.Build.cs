// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

// ReSharper disable once InconsistentNaming
public class RuneSpark : ModuleRules
{
	public RuneSpark(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "GameplayTags" });

		PrivateDependencyModuleNames.AddRange(new string[] { "GMCCore", "GMCAbilitySystem", "GMCExtended", "GMCExtendedAnimation" });

		PublicIncludePaths.AddRange(new string[]
		{
			Path.Combine(ModuleDirectory, "Public"),
			Path.Combine(ModuleDirectory, "Public/Actors"),
			Path.Combine(ModuleDirectory, "Public/Animation"),
			Path.Combine(ModuleDirectory, "Public/Components")
		});
		PrivateIncludePaths.AddRange(new string[]
		{
			Path.Combine(ModuleDirectory, "Private"),
			Path.Combine(ModuleDirectory, "Private/Actors"),
			Path.Combine(ModuleDirectory, "Private/Animation"),
			Path.Combine(ModuleDirectory, "Private/Components")
		});

	}
}
