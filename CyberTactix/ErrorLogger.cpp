
#include "ErrorLogger.h"
#include <SDL.h>

namespace pix
{
	ErrorLogger& ErrorLogger::Get() 
	{
		static ErrorLogger errorLogger_;

		return errorLogger_;
	}



	bool ErrorLogger::Init(const std::string& outputDirectory, int maxCountPerError)
	{
		if (isInitialized_) return true;

		if (outputDirectory.empty()) return false;

		std::string outputPath = outputDirectory;
		if (outputPath.back() != '/' && outputPath.back() != '\\')
			outputPath += '/';

		outputPath += "ErrorLog.txt";

		SDL_RWops* file = SDL_RWFromFile(outputPath.c_str(), "w"); // Opens in "w" mode to truncate or create
		if (!file) return false;

		SDL_RWclose(file);

		outputPath_ = outputPath;

		maxCountPerError_ = maxCountPerError;
		if (maxCountPerError_ < 1)
			maxCountPerError_ = 1;
		else if (maxCountPerError_ > MAX_LOG_ENTRIES)
			maxCountPerError_ = MAX_LOG_ENTRIES;

		isInitialized_ = true;

		return true;
	}

	void ErrorLogger::LogSDLError(const std::string& errorName)
	{
		if (!isLoggingEnabled_ || !isInitialized_ || errors_.size() >= MAX_LOG_ENTRIES) return;

		const char* sdlError = SDL_GetError();
		if (!sdlError || (*sdlError) == '\0')
			return;

		std::string errorMessage = sdlError;
		SDL_ClearError();

		int& count = errorCounts_[errorName];
		if (count >= maxCountPerError_) return;

		WriteToFile(FormatError(errorName, errorMessage));
		errors_.emplace_back(errorName, errorMessage);
		++count;
	}

	void ErrorLogger::LogError(const std::string& errorName, const std::string& errorMessage)
	{
		if (!isLoggingEnabled_ || !isInitialized_ || errors_.size() >= MAX_LOG_ENTRIES) return;

		int& count = errorCounts_[errorName]; // Inserts value 0 if there is none
		if (count >= maxCountPerError_) return;

		WriteToFile(FormatError(errorName, errorMessage));
		errors_.emplace_back(errorName, errorMessage);
		++count;
	}

	void ErrorLogger::ClearLog()
	{
		if (!isInitialized_) return;

		SDL_RWops* file = SDL_RWFromFile(outputPath_.c_str(), "w");
		if (file)
			SDL_RWclose(file);

		errors_.clear();
		errorCounts_.clear();
	}



	void ErrorLogger::SetLoggingEnabled(bool value)
	{
		isLoggingEnabled_ = value;
	}


	std::string ErrorLogger::GetErrorByIndex(int index) const 
	{
		if (errors_.empty()) return std::string();

		if (index < 0) index = 0;
		else if (index >= errors_.size())
		  index = errors_.size() - 1;

		return FormatError(errors_[index].Name, errors_[index].Message);
	}

	std::string ErrorLogger::GetErrorByName(const std::string& errorName) const
	{
		std::string foundErrors;

		const int errorCount = errors_.size();

		for (int i = 0; i < errorCount; i++)
		{
			if (errors_[i].Name == errorName)
				foundErrors += FormatError(errors_[i].Name, errors_[i].Message);
		}

		return foundErrors;
	}

	std::string ErrorLogger::GetAllErrors() const
	{
		std::string allErrors;

		const int errorCount = errors_.size();

		for (int i = 0; i < errorCount; i++)
			allErrors += FormatError(errors_[i].Name, errors_[i].Message);

		return allErrors;
	}

	int ErrorLogger::GetTotalErrorCount() const
	{
		return errors_.size();
	}

	int ErrorLogger::GetTotalUniqueErrorCount() const
	{
		return errorCounts_.size();
	}

	int ErrorLogger::GetErrorCount(const std::string& errorName) const
	{
		auto it = errorCounts_.find(errorName);

		if (it != errorCounts_.end()) return it->second;

		return 0;
	}

	const std::string& ErrorLogger::GetOutputPath() const
	{
		return outputPath_;
	}

	int ErrorLogger::GetMaxCountPerError() const
	{
		return maxCountPerError_;
	}

	bool ErrorLogger::IsLoggingEnabled() const
	{
		return isLoggingEnabled_;
	}

	bool ErrorLogger::IsInitialized() const
	{
		return isInitialized_;
	}


	ErrorLogger::Error::Error(const std::string& name, const std::string& message) :
		Name(name),
		Message(message)
	{
	}

	void ErrorLogger::WriteToFile(const std::string& input)
	{
		if (outputPath_.empty())
			return;

		SDL_RWops* file = SDL_RWFromFile(outputPath_.c_str(), "a");
		if (!file)
			return;

		SDL_RWwrite(file, input.c_str(), 1, input.size());
		SDL_RWclose(file);
	}

	std::string ErrorLogger::FormatError(const std::string& errorName, const std::string& errorMessage) const
	{
		return errorName + ": " + errorMessage + "\n\n";
	}

}