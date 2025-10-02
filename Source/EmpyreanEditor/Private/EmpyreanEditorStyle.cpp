// Copyright (c) 2025 Ampere Games.

#include "EmpyreanEditorStyle.h"

#include <Interfaces/IPluginManager.h>
#include <Styling/SlateStyle.h>
#include <Styling/SlateStyleRegistry.h>

TSharedPtr<FSlateStyleSet> FEmpyreanEditorStyle::StyleSet = nullptr;

void FEmpyreanEditorStyle::Startup()
{
	// Create the new Style.
	StyleSet = MakeShareable(new FSlateStyleSet("EmpyreanEditorStyle"));

	// Assign the content root of this Style.
	StyleSet->SetContentRoot(IPluginManager::Get().FindPlugin(TEXT("Empyrean"))->GetBaseDir());

	// Register icon for Empyrean actor placement class.
	FSlateImageBrush* empyreanIconBrush = new FSlateImageBrush(StyleSet->RootToContentDir(TEXT("Resources/EmpyreanIcon"), TEXT(".png")), FVector2D(16.0f));
	StyleSet->Set("Empyrean.Icon", empyreanIconBrush);

	// Register icon for the EmpyreanSky class (for now uses the same as the placement icon).
	FSlateImageBrush* skyIconBrush = new FSlateImageBrush(StyleSet->RootToContentDir(TEXT("Resources/SkyIcon"), TEXT(".png")), FVector2D(16.0f));
	StyleSet->Set("ClassIcon.EmpyreanSky", skyIconBrush);

	// Register thumbnail for the EmpyreanSky class  (for now uses the same as the placement icon).
	FSlateImageBrush* skyClassBrush = new FSlateImageBrush(StyleSet->RootToContentDir(TEXT("Resources/SkyThumbnail"), TEXT(".png")), FVector2D(256.0f));
	StyleSet->Set("ClassThumbnail.EmpyreanSky", skyClassBrush);

	// Register the Empyrean Editor Styling.
	FSlateStyleRegistry::RegisterSlateStyle(*StyleSet.Get());
}

void FEmpyreanEditorStyle::Shutdown()
{
	// Unregister the Editor Style.
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleSet.Get());

	// Reset the pointer.
	StyleSet.Reset();
}
