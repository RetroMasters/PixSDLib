#pragma once

#include "InputPumps.h"
#include <map>


namespace pix
{

	class ObjectInput
	{

	public:

		ObjectInput() = default;
		~ObjectInput() = default;


		ObjectInput(const ObjectInput& other) :
			virtualAxes_(other.virtualAxes_),
			keyboardInputPumps_(other.keyboardInputPumps_),
			mouseInputPumps_(other.mouseInputPumps_),
			gamepadInputPumps_(other.gamepadInputPumps_),
			virtualInputPumps_(other.virtualInputPumps_)
		{
			RelinkPumpsToAxes();
		}


		ObjectInput& operator= (const ObjectInput& other)
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

		bool AddKeyboardInput(SDL_Scancode sourceKey, const std::string& axisName, AbstractInputPump::PumpFunction pumpFunction = nullptr);

		bool DeleteKeyboardInput(SDL_Scancode sourceKey, const std::string& axisName);

		bool AddMouseInput(MouseInput::Button sourceButton, const std::string& axisName, AbstractInputPump::PumpFunction pumpFunction = nullptr);

		bool DeleteMouseInput(MouseInput::Button sourceButton, const std::string& axisName);

		bool AddGamepadButtonInput(int sourceGamepadIndex, SDL_GameControllerButton sourceButton, std::string axisName, AbstractInputPump::PumpFunction pumpFunction = nullptr);

		bool DeleteGamepadButtonInput(int sourceGamepadIndex, SDL_GameControllerButton sourceButton, const std::string& axisName);

		bool AddGamepadAxisInput(int sourceGamepadIndex, SDL_GameControllerAxis sourceAxis, std::string axisName, AbstractInputPump::PumpFunction pumpFunction = nullptr);

		bool DeleteGamepadAxisInput(int sourceGamepadIndex, SDL_GameControllerAxis sourceAxis, const std::string& axisName);

		bool AddVirtualInput(const std::string& sourceID, const std::string& axisName, AbstractInputPump::PumpFunction pumpFunction = nullptr);

		bool DeleteVirtualInput(const std::string& sourceID, const std::string& axisName);

		bool SetVirtualInput(const std::string& sourceID, const std::string& axisName, float sourceValue);

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

		float GetAxisValue(const std::string& axisName) const;

		bool ContainsAxis(const std::string& axisName) const; // To support manual error handling





	private:

		VirtualAxis* GetOrAddVirtualAxis(const std::string& axisName);

		void UpdatePreviousInput();

		void ResetAllAxisValues() ;

		void PumpInput() ;

		void RelinkPumpsToAxes() ;

		std::map<std::string, VirtualAxis> virtualAxes_;

		std::map<std::string, KeyboardInputPump> keyboardInputPumps_;
		std::map<std::string, MouseInputPump>    mouseInputPumps_;
		std::map<std::string, GamepadInputPump>  gamepadInputPumps_;
		std::map<std::string, VirtualInputPump>  virtualInputPumps_;

	};

}
