#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FSkillMenuModule : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	/** Register the Skill menu in the main menu bar */
	void RegisterMenus();

	/** Open an Editor Utility Widget by asset path */
	void OpenEditorUtilityWidget(const FString& AssetPath);

	/** Static callback for the Skill Editor menu entry */
	static void OpenSkillEditorStatic();
};
