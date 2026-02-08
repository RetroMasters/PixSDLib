#pragma once

#include "AbstractInputPump.h"
#include "Input.h"

namespace pix
{

	class KeyboardInputPump : public AbstractInputPump
	{
	public:

		KeyboardInputPump(SDL_Scancode sourceKey, VirtualAxis& targetAxis, AbstractInputPump::PumpFunction pumpFunction = AbsMaxf);

		~KeyboardInputPump()  = default;

		void SetSourceKey(SDL_Scancode sourceKey);

		float GetSourceState() const  override;

		SDL_Scancode GetSourceKey() const;


	private:

		SDL_Scancode sourceKey_;

	};




	class MouseInputPump : public AbstractInputPump
	{
	public:

		MouseInputPump(MouseInput::Button sourceButton, VirtualAxis& targetAxis, AbstractInputPump::PumpFunction pumpFunction = AbsMaxf);

		~MouseInputPump()  = default;

		void SetSourceButton(MouseInput::Button sourceButton);

		float GetSourceState() const  override;

		MouseInput::Button GetSourceButton() const;

	private:

		MouseInput::Button sourceButton_;
	};




	class GamepadInputPump : public AbstractInputPump
	{
	public:

		GamepadInputPump(int sourceGamepadIndex, SDL_GameControllerButton sourceButton, VirtualAxis& targetAxis, AbstractInputPump::PumpFunction pumpFunction = AbsMaxf);

		GamepadInputPump(int sourceGamepadIndex, SDL_GameControllerAxis sourceAxis, VirtualAxis& targetAxis, AbstractInputPump::PumpFunction pumpFunction = AbsMaxf);

		~GamepadInputPump() = default;

		void SetSourceGamepadIndex(int sourceGamepadIndex);

		void SetSourceButton(SDL_GameControllerButton sourceButton);

		void SetSourceAxis(SDL_GameControllerAxis sourceAxis);

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

		VirtualInputPump(int sourceID, VirtualAxis& axis, PumpFunction pumpFunction = AbsMaxf);

		~VirtualInputPump() = default;

		void SetSourceState(float state); // The unique feature of VirtualInputPup is to set source state manually

		void SetSourceID(int sourceID);

		float GetSourceState() const override;

		int GetSourceID() const;

	private:

		float sourceState_;
		int sourceID_;
	};


}
