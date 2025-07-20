#pragma once
#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"

class FDuoQEditorModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};