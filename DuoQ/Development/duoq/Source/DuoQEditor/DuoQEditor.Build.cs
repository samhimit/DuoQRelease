// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class DuoQEditor : ModuleRules
{
    public DuoQEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PrivateDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "Slate", "SlateCore", "ImporterRules", "UnrealEd", "UMG","VaRest" });
        PublicDependencyModuleNames.Add("DuoQ");
    }
}
