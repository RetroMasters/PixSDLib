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

		// Returns true if the key is pressed, 0 otherwise
		bool IsKeyDown(SDL_Scancode key) const;

	private:

		KeyboardInput();
		~KeyboardInput() = default;

		const Uint8* currentKeyStates_;
	};


	// The MouseInput singleton is a low-level wrapper for the most common mouse input functionality.
	// 
	// Technical note:
	// MouseInput relies on SDL_PollEvent() or SDL_PumpEvents() that update the internal state of the mouse.
	// 
	// Usage pattern:
	// 0. If the game is updated each frame, do steps 1,2,3 each frame, otherwise inside fixed-update calls.
    // 1. Call BeginUpdate() once at the start of the loop iteration before event polling.
    // 2. Call HandleEvents() for each SDL_Event during event polling.
    // 3. Call Update() once after all event polling to snapshot button and position state 
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
			X2 = SDL_BUTTON_X2
		};

		// Returns the MouseInput instance
		static MouseInput& Get();
		
		// Zeroes the wheel delta
		void BeginUpdate();

		// Handles SDL_MOUSEWHEEL events: accumulates wheel delta (+-1.0 per notch, traditionally)
		void HandleEvents(const SDL_Event& event);

		// Updates cursor position and button state
		void Update() ;

		

		// Returns true if the button is pressed, false otherwise
		bool IsButtonDown(Button button) const;

		Vector2i GetMousePosition() const; // Convenience function, but left here

		int GetMousePositionX() const;

		int GetMousePositionY() const;

		Vector2f GetWheelDelta() const; // Convenience function, but left here

		float GetWheelDeltaX() const;

		float GetWheelDeltaY() const;  

	private:

		MouseInput();
		~MouseInput() = default;

		int positionX_;
		int positionY_;
		float wheelDeltaX_;
		float wheelDeltaY_;
		Uint32 buttonFlags_;
	};


	// The Gamepad singleton is a low-level wrapper for the gamepad input functionality.
	// 
	// Technical Note:
    // GamepadInput relies on SDL_PollEvent() or SDL_PumpEvents() that update internal controller state (buttons and axes).
	// 
	// Philosophy:
	// The GamepadInput singleton is the centralized low-level source for gamepad input funtionality. 
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

		// Also called by the destructor
		int RemoveAllGamepads();

		


		// Called from SDL_CONTROLLERDEVICEADDED events: event.cdevice.which is the device index
		void AddGamepad(int deviceIndex);

		// Called from SDL_CONTROLLERDEVICEREMOVED events: event.cdevice.which is the joystickID
		void RemoveGamepad(SDL_JoystickID joystickID);


		// Runs a simple rumble effect on a haptic device.
		// The force of the rumble to play is clamped in range [0,1].
		// The duration of the effect is measured in ms (a negative value means "infinite" duration).
		void StartRumble(int gamepadIndex, float force, int duration);

		void StopRumble(int gamepadIndex);



		bool HasRumble(int gamepadIndex) const;

		int GetActiveGamepadCount() const;

		bool IsValidGamepadIndex(int gamepadIndex) const;

		bool IsButtonDown(int gamepadIndex, SDL_GameControllerButton button) const;
	
		// Returns a normalized axis value. Sticks are in range [-1,1]. Triggers are in range [0,1].
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
