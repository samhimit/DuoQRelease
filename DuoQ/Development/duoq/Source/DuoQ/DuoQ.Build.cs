// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using UnrealBuildTool.Rules;

public class DuoQ : ModuleRules
{
    public DuoQ(ReadOnlyTargetRules Target) : base(Target)
    {
        PrivateDependencyModuleNames.AddRange(new string[] { "Niagara", "NNE" });
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;


        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "EnhancedInput", "AdvancedWidgets", "RuntimeSpeechRecognizer", "RuntimeAudioImporter", "VaRest", "AIModule", "GOAPNPC", "NiagaraCore", "NNE", "AudioModulation", "DiscordGame", "OnlineServicesEOS", "PhysicsCore" });

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "GameplayTags", "UMG" });

        PublicDependencyModuleNames.AddRange(new string[] { "NavigationSystem" });
    }

}
