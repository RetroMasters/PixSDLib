#pragma once

#include "InputPumps.h"
#include <map>


namespace pix
{

	class ObjectInputLegacy
	{

	public:

		ObjectInputLegacy() = default;
		~ObjectInputLegacy() = default;


		ObjectInputLegacy(const ObjectInputLegacy& other) :
			virtualAxes_(other.virtualAxes_),
			keyboardInputPumps_(other.keyboardInputPumps_),
			mouseInputPumps_(other.mouseInputPumps_),
			gamepadInputPumps_(other.gamepadInputPumps_),
			virtualInputPumps_(other.virtualInputPumps_)
		{
			RelinkPumpsToAxes();
		}


		ObjectInputLegacy& operator= (const ObjectInputLegacy& other)
		{
			virtualAxes_ = other.virtualAxes_;

			keyboardInputPumps_ = other.keyboardInputPumps_;
			mouseInputPumps_ = other.mouseInputPumps_;
			gamepadInputPumps_ = other.gamepadInputPumps_;
			virtualInputPumps_ = other.virtualInputPumps_;

			RelinkPumpsToAxes();

			return *this;
		}


		void Update();


		//################################################################## CONFIGURE INPUT ####################################################

		bool AddKeyboardBinding(SDL_Scancode sourceKey, const std::string& axisName, AbstractInputPump::PumpFunction pumpFunction = nullptr);

		bool RemoveKeyboardBinding(SDL_Scancode sourceKey, const std::string& axisName);

		bool AddMouseBinding(MouseInput::Button sourceButton, const std::string& axisName, AbstractInputPump::PumpFunction pumpFunction = nullptr);

		bool RemoveMouseBinding(MouseInput::Button sourceButton, const std::string& axisName);

		bool AddGamepadButtonBinding(int sourceGamepadIndex, SDL_GameControllerButton sourceButton, const std::string& axisName, AbstractInputPump::PumpFunction pumpFunction = nullptr);

		bool RemoveGamepadButtonBinding(int sourceGamepadIndex, SDL_GameControllerButton sourceButton, const std::string& axisName);

		bool AddGamepadAxisBinding(int sourceGamepadIndex, SDL_GameControllerAxis sourceAxis, const std::string& axisName, AbstractInputPump::PumpFunction pumpFunction = nullptr);

		bool RemoveGamepadAxisBinding(int sourceGamepadIndex, SDL_GameControllerAxis sourceAxis, const std::string& axisName);

		// To ensure unique input identity, don't use "|" in sourceID and axisName
		bool AddVirtualBinding(int sourceID, const std::string& axisName, AbstractInputPump::PumpFunction pumpFunction = nullptr);

		bool RemoveVirtualBinding(int sourceID, const std::string& axisName);

		bool SetVirtualInput(int sourceID, const std::string& axisName, float sourceValue);

		bool SetDeadZone(const std::string& axisName, float value);

		void ResetState();

		//################################################################## CHECK INPUT ####################################################

		bool IsPositive(const std::string& axisName) const;

		bool IsNowPositive(const std::string& axisName) const;

		bool IsNowZeroFromPositive(const std::string& axisName) const;

		bool IsNegative(const std::string& axisName) const;

		bool IsNowNegative(const std::string& axisName) const;

		bool IsNowZeroFromNegative(const std::string& axisName) const;

		bool IsNowZero(const std::string& axisName) const;

		float GetAxisState(const std::string& axisName) const;

		bool ContainsAxis(const std::string& axisName) const; // To support manual error handling





	private:

		VirtualAxis* GetOrAddVirtualAxis(const std::string& axisName);

		void SyncPreviousInput();

		void ResetAxisState() ;

		void PumpInput() ;

		void RelinkPumpsToAxes() ;

		std::map<std::string, VirtualAxis> virtualAxes_;

		std::map<std::string, KeyboardInputPump> keyboardInputPumps_;
		std::map<std::string, MouseButtonInputPump>    mouseInputPumps_;
		std::map<std::string, GamepadInputPump>  gamepadInputPumps_;
		std::map<std::string, VirtualInputPump>  virtualInputPumps_;

	};

}
