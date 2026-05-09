#pragma once

#include <SDL.h>
#include <SDL_gamecontroller.h>
#include <SDL_haptic.h>
#include <vector>
#include "Uncopyable.h"
#include "PixMath.h"

namespace pix
{
	// The KeyboardInput singleton is a low-level wrapper for the keyboard input functionality.
	// 
	// Technical Note:
	// KeyboardInput relies on SDL_PollEvent() or SDL_PumpEvents() that update the internal state of the keyboard.
	// This class doesn't take into account whether shift has been pressed or not.
	// 
	// Philosophy:
	// The purpose of the KeyboardInput singleton is to have the keyboard input functionality centralized. 
	// Higher-level input management is built on top of that.
	class KeyboardInput : private Uncopyable
	{
	public:

		// Returns the KeyboardInput instance
		static KeyboardInput& Get();

		// Returns true if the key is pressed, false otherwise
		bool IsKeyDown(SDL_Scancode key) const;

	private:

		KeyboardInput();
		~KeyboardInput() = default;

		const Uint8* currentKeyStates_;
	};


	// The MouseInput singleton is a low-level wrapper for the most common mouse input functionality.
	// 
	// Technical note:
	// Mouse position and button state rely on SDL_PollEvent() or SDL_PumpEvents() updating SDL's internal mouse state.
    // Mouse wheel input is event-based and requires HandleEvents() to receive SDL_MOUSEWHEEL events.
	// 
	// In general, transient input state (Became* input actions) is either update-based or render-frame-based, and must not be mixed.
    // That is why transient mouse wheel input maintains two separate retrieval paths: one update-based, one render-frame-based.
	// 
	// Usage pattern inside the Gameloop to handle mousewheel input correctly:
	// 1) Call HandleEvents() for each SDL_Event during event polling.
    // 2) Call EndUpdate() after each fixed-update call.
    // 3) Call EndRender() after the render call.
	// 
	// Philosophy:
	// The purpose of the MouseInput singleton is to have most common mouse input functionality centralized. 
	// Higher-level input management is built on top of that.
	class MouseInput : private Uncopyable
	{
	public:

		enum Button
		{
			LEFT = SDL_BUTTON_LEFT,
			MIDDLE = SDL_BUTTON_MIDDLE,
			RIGHT = SDL_BUTTON_RIGHT,
			X1 = SDL_BUTTON_X1,
			X2 = SDL_BUTTON_X2,
			BUTTON_MAX = SDL_BUTTON_X2 + 1
		};

		// Returns the MouseInput instance
		static MouseInput& Get();
		
		// Ends the current fixed-update input lifetime by clearing update-based wheel delta.
		void EndUpdate();

		// Ends the current render-frame input lifetime by clearing frame-based wheel delta.
		void EndRender();

		// Handles SDL_MOUSEWHEEL events and accumulates normalized wheel delta (traditionally +/-1.0 per notch)
		void HandleEvents(const SDL_Event& event);

		// Updates cursor position and button state
		void Update();



		// Returns true if the button is pressed, false otherwise
		bool IsButtonDown(Button button) const;

		// Convenience function returning both mouse position components
		Vec2i GetMousePosition() const; 

		int GetMousePositionX() const;

		int GetMousePositionY() const;

		// Wheel direction after normalization: 
		// scroll right -> positive X, scroll left -> negative X, 
		// scroll up/away from user -> positive Y, scroll down/toward user -> negative Y.

		// Only valid in fixed-update calls of the Gameloop, according to the usage pattern
		float GetUpdateWheelDeltaX() const;
		float GetUpdateWheelDeltaY() const;  

		// Only valid in render calls of the Gameloop, according to the usage pattern
		float GetRenderWheelDeltaX() const;
		float GetRenderWheelDeltaY() const;

	private:

		MouseInput() = default;
		~MouseInput() = default;

		int positionX_ = 0;
		int positionY_ = 0;
		float wheelDeltaXInUpdate_ = 0.0f;
		float wheelDeltaYInUpdate_ = 0.0f;
		float wheelDeltaXInRender_ = 0.0f;
		float wheelDeltaYInRender_ = 0.0f;
		Uint32 buttonFlags_ = 0;
	};


	// The GamepadInput singleton is a low-level wrapper for the gamepad input functionality.
	// 
	// Technical Note:
    // GamepadInput relies on SDL_PollEvent() or SDL_PumpEvents() that update internal controller state (buttons and axes).
	// 
	// Philosophy:
	// The GamepadInput singleton is the centralized low-level source for gamepad input functionality. 
	// Haptics only expose a simple rumble effect, but that is sufficient for most use cases.
	// Higher-level input management is built on top of that.
	class GamepadInput : private Uncopyable
	{

	public:

		// Returns the GamepadInput instance
		static GamepadInput& Get();

		// Enumerates and opens all currently connected game controllers.
        // Safe to call at startup or when rebuilding controller state.
        // Controller add/remove events are handled separately.
		int AddAllGamepads();

		// Closes all active gamepads and returns the number of closed gamepads.
        // Also called by the destructor.
        // Prefer explicit RemoveAllGamepads() during shutdown before SDL_Quit().
		int RemoveAllGamepads();

		


		// Called from SDL_CONTROLLERDEVICEADDED events: event.cdevice.which is the device index
		void AddGamepad(int deviceIndex);

		// Called from SDL_CONTROLLERDEVICEREMOVED events: event.cdevice.which is the joystickID
		void RemoveGamepad(SDL_JoystickID joystickID);


		// Runs a simple rumble effect on a haptic device.
		// The force of the rumble to play is clamped in range [0.0f,1.0f].
		// The duration of the effect is measured in milliseconds (a negative value means "infinite" duration).
		void StartRumble(int gamepadIndex, float force, int duration);

		void StopRumble(int gamepadIndex);

		bool HasRumble(int gamepadIndex) const;

		// Returns the number of active gamepads.
        // Active gamepad indices are not guaranteed to be contiguous.
		int GetActiveGamepadCount() const;

		// Returns the number of gamepad slots, including inactive slots.
	    // Iterate from 0 to GetGamepadSlotCount() - 1 and use IsValidGamepadIndex() to test each slot.
		int GetGamepadSlotCount() const;

		bool IsValidGamepadIndex(int gamepadIndex) const;

		bool IsButtonDown(int gamepadIndex, SDL_GameControllerButton button) const;
	
		// Returns a normalized axis value. Sticks are in range [-1.0f,1.0f]. Triggers are in range [0.0f,1.0f].
		float GetAxisValue(int gamepadIndex, SDL_GameControllerAxis axis) const;




	private:

		struct Gamepad
		{
			Gamepad(SDL_GameController* gameController, SDL_Haptic* rumbleHandle, SDL_JoystickID joystickID, bool isInitialized);

			void Set(SDL_GameController* gameController, SDL_Haptic* rumbleHandle, SDL_JoystickID joystickID, bool isInitialized);

			SDL_GameController* GameController;
			SDL_Haptic* RumbleHandle;
			SDL_JoystickID JoystickID; // Ensures stable identity for hot-plugging
			bool IsInitialized;
		};

		GamepadInput() = default;
		~GamepadInput();

		SDL_Haptic* TryOpenRumble(SDL_Joystick* joystick);

		std::vector<Gamepad> gamepads_;
	};

}
