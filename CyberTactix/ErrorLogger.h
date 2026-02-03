#pragma once

#include <vector>
#include<unordered_map>
#include <string>
#include"Uncopyable.h"

namespace pix
{

	// The ErrorLogger singleton is a lightweight global error logging facility.
	// 
	// Philosophy:
    // Errors are expected to be the absolute exception, not the rule.
    // The game should continue running whenever possible, even if non-critical errors occur.
    //
    // To reduce overhead, repeated occurrences of the same error can be capped:
    // once an error is known, further logging attempts are ignored.
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
		// Returns true if initialization succeeds or if the ErrorLogger is already initialized, false otherwise.
		bool Init(const std::string& outputDirectory, int maxCountPerError);

		// Logs the last SDL error on the current thread, if present and not already logged
		void LogSDLError(const std::string& errorName);

		// Logs a custom error message under the given error name
		void LogError(const std::string& errorName, const std::string& errorMessage);

		// Clears the log file and all in-memory error state
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

		static constexpr int maxLogEntries_ = 10000;

		ErrorLogger();
		~ErrorLogger() = default;

		void WriteToFile(const std::string& input);
		std::string FormatError(const std::string& errorName, const std::string& errorMessage) const;

		std::vector<std::pair<std::string, std::string>> errors_;
		std::unordered_map<std::string, int> errorCounts_;
		std::string outputPath_;
		int maxCountPerError_;
		bool isLoggingEnabled_;
		bool isInitialized_;
	};

}
