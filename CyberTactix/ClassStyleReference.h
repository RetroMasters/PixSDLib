#pragma once

// Compileable style reference for class/struct member ordering and naming conventions.
// Not intended to be used as a runtime class.
//
// Naming convention:
// - Types, methods, and public data members use PascalCase.
// - Private and protected data members use lowerCamelCase_.
// - Parameters and local variables use lowerCamelCase.
// - Compile-time constants use constexpr and UPPER_CASE_WITH_UNDERSCORES.
namespace pix
{
	class ClassStyleReference
	{
	public:

		// ###################################### TYPES ######################################

		struct StyleConfig
        {
			int Value = 0;
		};

		// ###################################### STATIC #####################################

		static constexpr int COMPILE_TIME_CONSTANT = 64;

		// static void DoSomethingStatic();
		// static StyleConfig Config;

		// ############################## INITIALIZATION / LIFETIME ##########################

		ClassStyleReference() = default;
		~ClassStyleReference() = default;

		// bool Init();
		// void Destroy();

		// ############################ FUNCTIONALITY / STATE CHANGES ########################

		void SetValue(int value)
		{
			int clampedValue = value < 0 ? 0 : value;
			value_ = clampedValue;
		}

		// void Update();
		// void AddItem(int item);

		// ########################## GETTERS / QUERIES / CONST METHODS ######################
		
		int GetValue() const { return value_; }

		// bool IsInitialized() const;
		// bool ContainsItem(int item) const;

		// ##################################### PUBLIC DATA #################################

		int PublicValue = 0;

	protected:

		// TYPES
		// STATIC
		// INITIALIZATION / LIFETIME
		// FUNCTIONALITY / STATE CHANGES
		// GETTERS / QUERIES / CONST METHODS

		// int GetSomethingProtected();

		// PROTECTED DATA

		int protectedValue_ = 0;

	private:

		// TYPES
		// STATIC
		// INITIALIZATION / LIFETIME
		// FUNCTIONALITY / STATE CHANGES
		// GETTERS / QUERIES / CONST METHODS
		
		// int GetSomethingPrivate();

		// PRIVATE DATA

		int value_ = 0;
	};
}