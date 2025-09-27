// Copyright (c) 2025 Pixane.

#pragma once

#include <Modules/ModuleInterface.h>

class FEmpyreanEditorModule : public IModuleInterface
{
public:
	//~ Begin IModuleInterface implementation.
	void StartupModule() override;
	void ShutdownModule() override;
	//~ End IModuleInterface implementation.
};
