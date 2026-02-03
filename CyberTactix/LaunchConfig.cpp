#include "LaunchConfig.h"

namespace pix
{

	LaunchConfig& LaunchConfig::Get() 
	{
		static LaunchConfig config_;

		return config_;
	}

	void LaunchConfig::Init(const LaunchConfigData& configData)
	{
		if (isInitialized_) return;

		data_ = configData;

		isInitialized_ = true;
	}

	const LaunchConfigData& LaunchConfig::GetData() const 
	{
		return data_;
	}

	bool LaunchConfig::IsInitialized() const 
	{
		return isInitialized_;
	}

	LaunchConfig::LaunchConfig():
		data_(),
		isInitialized_(false)
	{
	}

}