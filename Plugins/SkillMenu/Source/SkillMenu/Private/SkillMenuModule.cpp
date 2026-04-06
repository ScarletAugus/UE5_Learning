#include "SkillMenuModule.h"
#include "ToolMenus.h"
#include "LevelEditor.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "Framework/Notifications/NotificationManager.h"
#include "EditorUtilityWidget.h"
#include "EditorUtilitySubsystem.h"
#include "EditorUtilityWidgetBlueprint.h"

#define LOCTEXT_NAMESPACE "FSkillMenuModule"

void FSkillMenuModule::StartupModule()
{
	// Delay menu registration until ToolMenus is ready
	UToolMenus::RegisterStartupCallback(
		FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FSkillMenuModule::RegisterMenus));
}

void FSkillMenuModule::ShutdownModule()
{
	UToolMenus::UnRegisterStartupCallback(this);
	UToolMenus::UnregisterOwner(this);
}

void FSkillMenuModule::OpenEditorUtilityWidget(const FString& AssetPath)
{
	UEditorUtilityWidgetBlueprint* WidgetBP = LoadObject<UEditorUtilityWidgetBlueprint>(nullptr, *AssetPath);
	if (WidgetBP)
	{
		UEditorUtilitySubsystem* Subsystem = GEditor->GetEditorSubsystem<UEditorUtilitySubsystem>();
		if (Subsystem)
		{
			Subsystem->SpawnAndRegisterTab(WidgetBP);
		}
	}
	else
	{
		FNotificationInfo Info(FText::Format(
			LOCTEXT("WidgetNotFound", "未找到编辑器组件: {0}\n请先在 Content/Skills/Editor/ 下创建 EUW_SkillEditor"),
			FText::FromString(AssetPath)));
		Info.ExpireDuration = 5.0f;
		FSlateNotificationManager::Get().AddNotification(Info);
	}
}

void FSkillMenuModule::RegisterMenus()
{
	FToolMenuOwnerScoped OwnerScoped(this);

	// Extend the main menu bar of the Level Editor
	UToolMenu* MainMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu");
	if (!MainMenu)
	{
		return;
	}

	// Add a top-level "Skill" pull-down sub-menu to the menu bar
	FToolMenuSection& MainSection = MainMenu->FindOrAddSection("MainMenu");

	MainSection.AddSubMenu(
		"SkillMenu",
		LOCTEXT("SkillMenuLabel", "技能"),
		LOCTEXT("SkillMenuTooltip", "技能系统菜单"),
		FNewToolMenuDelegate::CreateLambda([](UToolMenu* SubMenu)
		{
			FToolMenuSection& Section = SubMenu->FindOrAddSection("SkillActions");

			// 技能列表
			Section.AddMenuEntry(
				"SkillList",
				LOCTEXT("SkillListLabel", "技能列表"),
				LOCTEXT("SkillListTooltip", "查看所有可用技能"),
				FSlateIcon(),
				FUIAction(FExecuteAction::CreateLambda([]()
				{
					FNotificationInfo Info(LOCTEXT("SkillListNotification", "技能列表功能 - 开发中"));
					Info.ExpireDuration = 3.0f;
					FSlateNotificationManager::Get().AddNotification(Info);
				}))
			);

			// 添加技能
			Section.AddMenuEntry(
				"AddSkill",
				LOCTEXT("AddSkillLabel", "添加技能"),
				LOCTEXT("AddSkillTooltip", "添加一个新的技能"),
				FSlateIcon(),
				FUIAction(FExecuteAction::CreateLambda([]()
				{
					FNotificationInfo Info(LOCTEXT("AddSkillNotification", "添加技能功能 - 开发中"));
					Info.ExpireDuration = 3.0f;
					FSlateNotificationManager::Get().AddNotification(Info);
				}))
			);

			// 技能编辑器
			Section.AddMenuEntry(
				"SkillEditor",
				LOCTEXT("SkillEditorLabel", "技能编辑器"),
				LOCTEXT("SkillEditorTooltip", "打开技能编辑器窗口"),
				FSlateIcon(),
				FUIAction(FExecuteAction::CreateStatic(&FSkillMenuModule::OpenSkillEditorStatic))
			);

			// 技能设置
			Section.AddMenuEntry(
				"SkillSettings",
				LOCTEXT("SkillSettingsLabel", "技能设置"),
				LOCTEXT("SkillSettingsTooltip", "配置技能系统参数"),
				FSlateIcon(),
				FUIAction(FExecuteAction::CreateLambda([]()
				{
					FNotificationInfo Info(LOCTEXT("SkillSettingsNotification", "技能设置功能 - 开发中"));
					Info.ExpireDuration = 3.0f;
					FSlateNotificationManager::Get().AddNotification(Info);
				}))
			);
		}),
		false,  // bInSubMenu
		FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelEditor.GameSettings")
	);
}

void FSkillMenuModule::OpenSkillEditorStatic()
{
	FSkillMenuModule& Module = FModuleManager::GetModuleChecked<FSkillMenuModule>("SkillMenu");
	Module.OpenEditorUtilityWidget(TEXT("/Game/Skills/Editor/EUW_SkillEditor.EUW_SkillEditor"));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FSkillMenuModule, SkillMenu)
