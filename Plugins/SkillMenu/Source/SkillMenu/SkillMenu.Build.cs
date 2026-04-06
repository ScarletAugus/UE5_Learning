using UnrealBuildTool;

public class SkillMenu : ModuleRules
{
	public SkillMenu(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"SkillRuntime",
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"CoreUObject",
			"Engine",
			"Slate",
			"SlateCore",
			"UnrealEd",
			"LevelEditor",
			"ToolMenus",
			"Blutility",
			"UMG",
			"UMGEditor",
		});
	}
}
