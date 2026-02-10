#pragma once

#include "AbstractInputPump.h"
#include "Input.h"

// All of these pump classes internally rely on the Input singleton

namespace pix
{

	// KeyboardInputPump has a physical keyboard key as its input source. It connects it to a virtual axis.
	class KeyboardInputPump : public AbstractInputPump
	{
	public:

		KeyboardInputPump(SDL_Scancode sourceKey, VirtualAxis& targetAxis, AbstractInputPump::PumpFunction pumpFunction = nullptr);

		~KeyboardInputPump() override = default;

		void SetSourceKey(SDL_Scancode sourceKey);

		// Returns 1.0f if key is pressed, 0.0f otherwise
		float GetSourceState() const  override;

		SDL_Scancode GetSourceKey() const;


	private:

		SDL_Scancode sourceKey_;

	};



	// MouseButtonInputPump has a physical mouse button as its input source. It connects it to a virtual axis.
	// This pump only handles button state; position/wheel are handled elsewhere.
	class MouseButtonInputPump : public AbstractInputPump
	{
	public:

		MouseButtonInputPump(MouseInput::Button sourceButton, VirtualAxis& targetAxis, AbstractInputPump::PumpFunction pumpFunction = nullptr);

		~MouseButtonInputPump() override = default;

		void SetSourceButton(MouseInput::Button sourceButton);

		// Returns 1.0f if button is pressed, 0.0f otherwise
		float GetSourceState() const  override;

		MouseInput::Button GetSourceButton() const;

	private:

		MouseInput::Button sourceButton_;
	};



	// GamepadInputPump has a physical gamepad button or a physical gamepad axis as its input source. It connects it to a virtual axis.
	class GamepadInputPump : public AbstractInputPump
	{
	public:

		GamepadInputPump(int sourceGamepadIndex, SDL_GameControllerButton sourceButton, VirtualAxis& targetAxis, AbstractInputPump::PumpFunction pumpFunction = nullptr);

		GamepadInputPump(int sourceGamepadIndex, SDL_GameControllerAxis sourceAxis, VirtualAxis& targetAxis, AbstractInputPump::PumpFunction pumpFunction = nullptr);

		~GamepadInputPump() override = default;

		void SetSourceGamepadIndex(int sourceGamepadIndex);

		void SetSourceButton(SDL_GameControllerButton sourceButton);

		void SetSourceAxis(SDL_GameControllerAxis sourceAxis);

		// For axis: returns a value in range [-1.0f,1.0f] 
		// For button: returns 1.0f if pressed, 0.0f otherwise
		float GetSourceState() const override;

		int GetSourceGamepadIndex() const;
		SDL_GameControllerButton GetSourceButton() const;
		SDL_GameControllerAxis  GetSourceAxis() const;

	private:

		int sourceGamepadIndex_;
		SDL_GameControllerButton sourceButton_;
		SDL_GameControllerAxis   sourceAxis_;
	};




	class VirtualInputPump : public AbstractInputPump
	{
	public:

		VirtualInputPump(int sourceID, VirtualAxis& targetAxis, PumpFunction pumpFunction = nullptr);

		~VirtualInputPump() override = default;

		// Clamps and sets the virtual source state in range [-1.0f,1.0f].
		// VirtualInputPump is driven by user code via SetSourceState().
		void SetSourceState(float state);        

		void SetSourceID(int sourceID);

		// Returns the value set by SetSourceState(), in range [-1.0f,1.0f]
		float GetSourceState() const override;

		int GetSourceID() const;

	private:

		float sourceState_;
		int sourceID_;
	};


}
