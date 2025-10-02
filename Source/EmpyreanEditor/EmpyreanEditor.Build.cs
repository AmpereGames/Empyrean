// Copyright (c) 2025 Ampere Games.

using UnrealBuildTool;

public class EmpyreanEditor : ModuleRules
{
	public EmpyreanEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
                "Empyrean",
            }
		);
			
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
                "PlacementMode",
                "Projects",
                "SlateCore",
            }
		);
    }
}
