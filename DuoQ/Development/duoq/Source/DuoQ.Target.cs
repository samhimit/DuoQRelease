// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class DuoQTarget : TargetRules
{
    public DuoQTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Game;
        DefaultBuildSettings = BuildSettingsVersion.Latest;
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
        ExtraModuleNames.Add("DuoQ");
        ExtraModuleNames.Add("AdvancedWidgets");


        // overriding to be backwards compatible
        bLegacyParentIncludePaths = false; // => Omits module parent folders from include paths to reduce compiler command line length. (Previously: true).
        CppStandard = CppStandardVersion.Default;       // => Updates C++ Standard to C++20 (Previously: CppStandardVersion.Cpp17).
        WindowsPlatform.bStrictConformanceMode = true;  // => Updates MSVC strict conformance mode to true (Previously: false).
        bValidateFormatStrings = true; // => Enables compile-time validation of strings+args passed to UE_LOG. (Previously: false).
    }
}
