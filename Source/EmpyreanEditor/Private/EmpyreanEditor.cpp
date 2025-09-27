// Copyright (c) 2025 Pixane.

#include "EmpyreanEditor.h"
#include "EmpyreanSky.h"
#include "EmpyreanEditorStyle.h"

#include <IPlacementModeModule.h>
#include <Modules/ModuleManager.h>

#define LOCTEXT_NAMESPACE "FEmpyreanEditorModule"

void FEmpyreanEditorModule::StartupModule()
{
	// Init the Editor Style.
	FEmpyreanEditorStyle::Startup();

	// Create a new Actor Placement category and register the plugin's actors.
	IPlacementModeModule& placementModeModule = IPlacementModeModule::Get();

	// Create a custom category and register it.
	FPlacementCategoryInfo info(INVTEXT("Empyrean"), FSlateIcon("EmpyreanEditorStyle", "Empyrean.Icon"), "Empyrean", TEXT("PMEmpyrean"), 32);
	placementModeModule.RegisterPlacementCategory(info);

	// Add actor classes to the category.
	placementModeModule.RegisterPlaceableItem(info.UniqueHandle, MakeShareable(new FPlaceableItem(nullptr, FAssetData(AEmpyreanSky::StaticClass()))));
}

void FEmpyreanEditorModule::ShutdownModule()
{
	// Clean the Editor Style.
	FEmpyreanEditorStyle::Shutdown();

	// Unregister the custom Actor Placement category.
	IPlacementModeModule::Get().UnregisterPlacementCategory("Empyrean");

	// Clean the module.
	FModuleManager::Get().OnModulesChanged().RemoveAll(this);
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FEmpyreanEditorModule, EmpyreanEditor)
