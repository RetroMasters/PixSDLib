#pragma once

#include "Uncopyable.h"

namespace pix
{
	// The data structure to initialize the LaunchConfig singleton, provided to the GameLoop constructor.  
	struct LaunchConfigData
	{
		int MaxCountPerError = 4; // // Max number of errors with the same name logged by ErrorLogger
 		int SoundChannelCount = 100; // Sound settings are all used for the Audio singleton
		float MasterVolume = 0.3f;
		float SoundVolume = 1.0f;
		float MusicVolume = 1.0f;
		float UpdatesPerSecond = 60.0f;
		int LogicalScreenWidth = 910; // Logical resolution for the SDL renderer
		int LogicalScreenHeight = 512;
		bool IsLinearFilter = false; // For image scaling
		bool IsIntegerScale = true;
		bool IsVsync = true;
		bool IsFullscreen = false;
	};

	// Immutable singleton for storing common launch settings.
	//
	// Note: A change in vsync takes effect only after a program restart. 
	// 
	// Philosophy:
	// These are the common settings that are set within the GameLoop constructor.
	// Referencing launch configuration data at runtime can be useful to restore default state.
	// Therefore, this data is exposed as a global, immutable singleton.
	class LaunchConfig : private Uncopyable
	{

	public:

		// Returns the LaunchConfig instance
		static LaunchConfig& Get();

		void Init(const LaunchConfigData& data);

		const LaunchConfigData& GetData() const;

		bool IsInitialized() const;

	private:

		LaunchConfig();
		~LaunchConfig() = default;

		LaunchConfigData data_;
		bool isInitialized_;
	};

}