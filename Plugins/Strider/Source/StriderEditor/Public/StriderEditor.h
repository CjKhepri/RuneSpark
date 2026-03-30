//Copyright 2020 - 2025, Vault Break Studios Pty. Ltd.
//Strider - Part of the 'Vault Break Library' (VBL)
//Written by: Ken Claassen

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

/** Editor module for the Strider plugin */
class FStriderEditorModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
