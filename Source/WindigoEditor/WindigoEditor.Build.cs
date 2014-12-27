// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class WindigoEditor : ModuleRules
{
	public WindigoEditor(TargetInfo Target)
	{
        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "UnrealEd", "Windigo" });

        PrivateDependencyModuleNames.AddRange(new string[] { });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");
		// if ((Target.Platform == UnrealTargetPlatform.Win32) || (Target.Platform == UnrealTargetPlatform.Win64))
		// {
		//		if (UEBuildConfiguration.bCompileSteamOSS == true)
		//		{
		//			DynamicallyLoadedModuleNames.Add("OnlineSubsystemSteam");
		//		}
		// }
		
		// bIsAutoStartupModule = true;

		// PrivateIncludePaths.AddRange(
			// new string[] {
                // "Runtime/Engine/Private",
			// }
		// );

		// PrivateIncludePathModuleNames.AddRange(
			// new string[] {
				// "TargetPlatform",
				// "DerivedDataCache",
				// "ImageWrapper",
			// }
		// );

		// PrivateDependencyModuleNames.AddRange(
			// new string[] {
				// "Core",
				// "CoreUObject",
				// "Engine",
				// "RenderCore", 
				// "RHI",
				// "ShaderCore",
			// }
		// );

		// SetupModulePhysXAPEXSupport(Target);
		// if (UEBuildConfiguration.bCompilePhysX && UEBuildConfiguration.bRuntimePhysicsCooking)
		// {
			// DynamicallyLoadedModuleNames.Add("PhysXFormats");
			// PrivateIncludePathModuleNames.Add("PhysXFormats");
		// }

		// if (UEBuildConfiguration.bBuildDeveloperTools && Target.Type != TargetRules.TargetType.Server)
		// {
			// PrivateDependencyModuleNames.AddRange(
				// new string[] {
					// "RawMesh"
				// }
			// );
		// }

		// if (UEBuildConfiguration.bBuildEditor == true)
		// {
			// PrivateDependencyModuleNames.AddRange(
				// new string[] {
                    // "UnrealEd", 
                // }
			// );

			// CircularlyReferencedDependentModules.AddRange(
				// new string[] {
                    // "UnrealEd",
                // }
			// );
		// }
	}
}
