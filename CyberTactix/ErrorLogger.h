#pragma once

#include <vector>
#include <unordered_map>
#include <string>
#include "Uncopyable.h"

namespace pix
{

	// The ErrorLogger singleton is a lightweight global error logging facility.
	// 
	// Philosophy:
    // Errors are expected to be the absolute exception, not the rule.
    // The game should continue running whenever possible, even if non-critical errors occur.
    //
	// To reduce overhead, repeated occurrences of the same error can be capped.
    // Once the cap is reached, further logging attempts for that error name are ignored.
    // Logging can also be disabled on demand in hot code paths.
    //
    // Performance-critical functions that run in tight loops should not log errors;
    // instead, they should return error codes and leave error handling to the caller.
	class ErrorLogger : private Uncopyable
	{
	public:

		// Returns the ErrorLogger instance
		static ErrorLogger& Get();

	

		// Initializes the ErrorLogger. 
        // Creates or truncates "ErrorLog.txt" inside outputDirectory, which must exist and be writable.
        // Limits the number of logged occurrences per error name to maxCountPerError.
		// Returns true if initialization succeeds, false otherwise.
		// Calling Init() again after successful initialization has no effect and returns true.
		bool Init(const std::string& outputDirectory, int maxCountPerError);

		// Logs the last SDL error on the current thread, if present and allowed by the per-error cap
		void LogSDLError(const std::string& errorName);

		// Logs a custom error message under the given error name
		void LogError(const std::string& errorName, const std::string& errorMessage);

		// Clears all in-memory error state and truncates the log file if possible
		void ClearLog();

		void SetLoggingEnabled(bool enabled);

		// Returns the formatted error at the given index.
        // The index is clamped to a valid range.
        // Returns an empty string if no errors are logged.
		std::string GetErrorByIndex(int index) const;

		// Returns all logged errors with the given name.
        // Returns an empty string if no matching errors exist.
		std::string GetErrorByName(const std::string& errorName) const;

		std::string GetAllErrors() const;

		// Returns the total number of all logged errors
		int GetTotalErrorCount() const;

		// Returns the total number of unique error names logged
		int GetTotalUniqueErrorCount() const;

		// Returns the number of logged occurrences for the given error name
		int GetErrorCount(const std::string& errorName) const;

		// Returns the maximum number of occurrences logged per error name
		int GetMaxCountPerError() const;

		const std::string& GetOutputPath() const;
		
		bool IsLoggingEnabled() const;

		bool IsInitialized() const;

	private:

		struct Error
		{
			Error(const std::string& name, const std::string& message);

			std::string Name;
			std::string Message;
		};

		static constexpr int MAX_LOG_ENTRIES = 10000;

		ErrorLogger() = default;
		~ErrorLogger() = default;

		void WriteToFile(const std::string& input);
		std::string FormatError(const std::string& errorName, const std::string& errorMessage) const;

		std::vector<Error> errors_;  
		std::unordered_map<std::string, int> errorCounts_;
		std::string outputPath_;
		int maxCountPerError_ = MAX_LOG_ENTRIES;
		bool isLoggingEnabled_ = true;
		bool isInitialized_ = false;
	};

}
