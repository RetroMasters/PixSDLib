#pragma once

#include "AbstractInputPump.h"
#include "Input.h"



namespace pix
{

	class KeyboardInputPump : public AbstractInputPump
	{
	public:

		KeyboardInputPump(SDL_Scancode sourceKey, VirtualAxis& targetAxis, AbstractInputPump::PumpFunction pumpFunction);

		~KeyboardInputPump()  = default;

		float GetSourceState() const  override;

		SDL_Scancode GetSourceKey() const;

	private:

		SDL_Scancode sourceKey_;

	};




	class MouseInputPump : public AbstractInputPump
	{
	public:

		MouseInputPump(MouseInput::Button sourceButton, VirtualAxis& targetAxis, AbstractInputPump::PumpFunction pumpFunction = nullptr);

		~MouseInputPump()  = default;

		float GetSourceState() const  override;

		MouseInput::Button GetSourceButton() const;

	private:

		MouseInput::Button sourceButton_;
	};




	class GamepadInputPump : public AbstractInputPump
	{
	public:

		GamepadInputPump(int sourceGamepadIndex, SDL_GameControllerButton sourceButton, VirtualAxis& targetAxis, AbstractInputPump::PumpFunction pumpFunction = nullptr);

		GamepadInputPump(int sourceGamepadIndex, SDL_GameControllerAxis sourceAxis, VirtualAxis& targetAxis, AbstractInputPump::PumpFunction pumpFunction = nullptr);

		~GamepadInputPump() = default;

		float GetSourceState() const override;

		int GetSourceGamepadIndex() const;
		SDL_GameControllerButton GetSourceBoutton() const;
		SDL_GameControllerAxis  GetSourceAxis() const;

	private:

		int sourceGamepadIndex_;
		SDL_GameControllerButton sourceButton_;
		SDL_GameControllerAxis   sourceAxis_;
	};




	class VirtualInputPump : public AbstractInputPump
	{
	public:

		VirtualInputPump(VirtualAxis& axis, PumpFunction pumpFunction = nullptr);

		~VirtualInputPump() = default;

		void SetSourceValue(float value);

		float GetSourceState() const override;

	private:

		float sourceState_;
	};


}
