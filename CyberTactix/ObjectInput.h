#pragma once

#include "InputPumps.h"

namespace pix
{

	class ObjectInput
	{

	public:

		// ######################################## INITIALIZATION ###################################################

		ObjectInput() = default;
		~ObjectInput() = default;

		ObjectInput(const ObjectInput& other);
		ObjectInput& operator= (const ObjectInput& other);

		// ################################### UPDATE ##################################

		void Update();

		//################################################################## CONFIGURE INPUT ####################################################

		// All methods return true if the binding has been added/removed, false otherwise 

		bool AddKeyboardBinding(SDL_Scancode sourceKey, const std::string& axisName, AbstractInputPump::PumpFunction pumpFunction = AbsMaxf);

		bool RemoveKeyboardBinding(SDL_Scancode sourceKey, const std::string& axisName);

		bool AddMouseBinding(MouseInput::Button sourceButton, const std::string& axisName, AbstractInputPump::PumpFunction pumpFunction = AbsMaxf);

		bool RemoveMouseBinding(MouseInput::Button sourceButton, const std::string& axisName);

		bool AddGamepadButtonBinding(int sourceGamepadIndex, SDL_GameControllerButton sourceButton, const std::string& axisName, AbstractInputPump::PumpFunction pumpFunction = AbsMaxf);

		bool RemoveGamepadButtonBinding(int sourceGamepadIndex, SDL_GameControllerButton sourceButton, const std::string& axisName);

		bool AddGamepadAxisBinding(int sourceGamepadIndex, SDL_GameControllerAxis sourceAxis, const std::string& axisName, AbstractInputPump::PumpFunction pumpFunction = AbsMaxf);

		bool RemoveGamepadAxisBinding(int sourceGamepadIndex, SDL_GameControllerAxis sourceAxis, const std::string& axisName);

		bool AddVirtualBinding(int sourceID, const std::string& axisName, AbstractInputPump::PumpFunction pumpFunction = AbsMaxf);

		bool RemoveVirtualBinding(int sourceID, const std::string& axisName);

		bool SetVirtualSourceState(int sourceID, const std::string& axisName, float sourceState);

		// The axis state within the deadzone is zero
		bool SetDeadZone(const std::string& axisName, float value);

		// Resets the previous and current state of all axes to zero
		void ResetAxes();

		// Resets the state of all virtual input sources to zero
		void ResetVirtualSourceState();

		//################################################################## CHECK INPUT ####################################################

		// Returns true if current axis state is positive, false otherwise
		bool IsPositive(const std::string& axisName) const;

		// Returns true if axis state was not positive and became positive in the current update iteration, false otherwise
		bool BecamePositive(const std::string& axisName) const;

		// Returns true if axis state was positive and became zero in the current update iteration, false otherwise
		bool BecameZeroFromPositive(const std::string& axisName) const;

		// Returns true if current axis state is negative, false otherwise
		bool IsNegative(const std::string& axisName) const;

		// Returns true if axis state was not negative and became negative in the current update iteration, false otherwise
		bool BecameNegative(const std::string& axisName) const;

		// Returns true if axis state was negative and became zero in the current update iteration, false otherwise
		bool BecameZeroFromNegative(const std::string& axisName) const;

		// Returns true if axis state was not zero and became zero in the current update iteration, false otherwise
		bool BecameZero(const std::string& axisName) const;

		// Returns current axis state
		float GetAxisState(const std::string& axisName) const;

		

	private:

		void SyncPreviousInput();

		void ResetAxisState();

		void PumpInput();

		void RelinkPumpsToAxes();

		VirtualAxis* GetOrAddVirtualAxis(const std::string& axisName);
		
		VirtualAxis* GetVirtualAxis(const std::string& axisName);

		const VirtualAxis* GetVirtualAxis(const std::string& axisName) const;

		int GetKeyboardPumpIndex(SDL_Scancode sourceKey, const std::string& axisName) const;

		int GetMousePumpIndex(MouseInput::Button sourceButton, const std::string& axisName) const;

		int GetGamepadPumpIndex(int sourceGamepadIndex, SDL_GameControllerButton sourceButton, const std::string& axisName) const;

		int GetGamepadPumpIndex(int sourceGamepadIndex, SDL_GameControllerAxis sourceAxis, const std::string& axisName) const;

		int GetVirtualPumpIndex(int sourceID, const std::string& axisName) const;

	
		std::vector<VirtualAxis> virtualAxes_;
		std::vector<KeyboardInputPump> keyboardInputPumps_;
		std::vector<MouseInputPump>    mouseInputPumps_;
		std::vector<GamepadInputPump>  gamepadInputPumps_;
		std::vector<VirtualInputPump>  virtualInputPumps_;

	};

}

