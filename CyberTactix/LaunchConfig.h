#pragma once

#include "Uncopyable.h"

namespace pix
{
	// Data used to initialize the LaunchConfig singleton, provided to the GameLoop constructor
	struct LaunchConfigData
	{
		// Max number of errors with the same name logged by ErrorLogger
		int MaxLogCountPerError = 4;

		// Audio singleton settings
 		int SoundChannelCount = 100; 
		float MasterVolume = 0.3f;
		float SoundVolume = 1.0f;
		float MusicVolume = 1.0f;

		// Update rate setting
		float UpdatesPerSecond = 60.0f;

		// Window / Renderer settings
		int LogicalResolutionWidth = 910; 
		int LogicalResolutionHeight = 512;
		bool IsLinearFilter = false; 
		bool IsIntegerScale = true;  
		bool IsVsync = true;
		bool IsFullscreen = false;
	};

	// Immutable singleton for storing common launch settings.
	// 
	// Philosophy:
	// These are common engine settings applied during GameLoop initialization.
	// Referencing launch configuration data at runtime can be useful to restore default state.
	// Therefore, this data is exposed as a global, immutable singleton.
	class LaunchConfig : private Uncopyable
	{

	public:

		// Returns the LaunchConfig instance
		static LaunchConfig& Get();

		void Init(const LaunchConfigData& data);

		// Returns the initialized launch configuration
		const LaunchConfigData& GetData() const;

		bool IsInitialized() const;

	private:

		LaunchConfig() = default;
		~LaunchConfig() = default;

		LaunchConfigData data_;
		bool isInitialized_ = false;
	};

}