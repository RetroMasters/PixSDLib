#pragma once

#include<vector>
#include "InputPumps.h"

namespace pix
{
	// ObjectInput encapsulates a set of customizable input bindings and query functionality.
    // It achieves this by internally managing input pumps and virtual axes.
	// 
	// Philosophy:
	// ObjectInput provides an isolated, configurable input context per consumer.
    // For example, a menu and a gameplay object may each maintain their own ObjectInput
    // instance with independent bindings and behavior. Virtual bindings enable custom or AI-driven input. 
	class ObjectInput
	{

	public:

		// ######################################## INITIALIZATION ###################################################

		ObjectInput() = default;
		~ObjectInput() = default;

		ObjectInput(const ObjectInput& other);
		ObjectInput& operator= (const ObjectInput& other);

		// ################################### UPDATE ##################################

		// Updates the input state. An object should check input AFTER Update() is called to react to the most current input state.
		// Requires the underlying Input singletons to have up-to-date SDL state(SDL_PollEvent / SDL_PumpEvents already processed).
		void Update();

		//################################################################## CONFIGURE INPUT ####################################################

		// All binding methods return true if the binding has been added/removed, false otherwise 
		// Adding a binding immediately sets all axis state to the currently polled one to prevent transient state (no "Became" actions are possible this update).
		
		bool AddKeyboardBinding(SDL_Scancode sourceKey, const std::string& axisName, AbstractInputPump::PumpFunction pumpFunction = nullptr);

		bool RemoveKeyboardBinding(SDL_Scancode sourceKey, const std::string& axisName);

		bool AddMouseButtonBinding(MouseInput::Button sourceButton, const std::string& axisName, AbstractInputPump::PumpFunction pumpFunction = nullptr);

		bool RemoveMouseButtonBinding(MouseInput::Button sourceButton, const std::string& axisName);

		bool AddGamepadButtonBinding(int sourceGamepadIndex, SDL_GameControllerButton sourceButton, const std::string& axisName, AbstractInputPump::PumpFunction pumpFunction = nullptr);

		bool RemoveGamepadButtonBinding(int sourceGamepadIndex, SDL_GameControllerButton sourceButton, const std::string& axisName);

		bool AddGamepadAxisBinding(int sourceGamepadIndex, SDL_GameControllerAxis sourceAxis, const std::string& axisName, AbstractInputPump::PumpFunction pumpFunction = nullptr);

		bool RemoveGamepadAxisBinding(int sourceGamepadIndex, SDL_GameControllerAxis sourceAxis, const std::string& axisName);

		bool AddVirtualBinding(int sourceID, const std::string& axisName, AbstractInputPump::PumpFunction pumpFunction = nullptr);

		bool RemoveVirtualBinding(int sourceID, const std::string& axisName);

		bool SetVirtualSourceState(int sourceID, const std::string& axisName, float sourceState);

		// The axis state within the deadzone is zero
		bool SetDeadZone(const std::string& axisName, float value);

		// Resets the previous and current state of all axes to zero
		void ResetAxes();

		// Resets the state of all virtual input sources to zero, not the axis state.
		// The resulting axis state is changed on the next Update().
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

		// Invariant: axes are append-only. VirtualAxis::id == index in virtualAxes_.
        // Cached pump axis IDs are therefore stable for the lifetime of this ObjectInput.
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
		std::vector<MouseButtonInputPump>    mouseInputPumps_;
		std::vector<GamepadInputPump>  gamepadInputPumps_;
		std::vector<VirtualInputPump>  virtualInputPumps_;

	};

}

