#pragma once

#include "AbstractInputPump.h"
#include "Input.h"



namespace pix
{

	class KeyboardInputPump : public AbstractInputPump
	{
	public:

		KeyboardInputPump(SDL_Scancode sourceKey, VirtualAxis& targetAxis, AbstractInputPump::PumpFunction pumpFunction)  : AbstractInputPump(targetAxis, pumpFunction), sourceKey_(sourceKey)
		{
			// Prevent wrong "Now"-Actions during the Update of instantiation
			PumpInput();
			GetVirtualAxis()->BeginUpdate();
		}

		~KeyboardInputPump()  = default;

		float GetSourceState() const  override
		{
			return KeyboardInput::Get().IsKeyDown(sourceKey_) ? 1.0f : 0.0f;
		}

		SDL_Scancode GetSourceKey() const  { return sourceKey_; }

	private:

		SDL_Scancode sourceKey_;

	};




	class MouseInputPump : public AbstractInputPump
	{
	public:

		MouseInputPump(MouseInput::Button sourceButton, VirtualAxis& targetAxis, AbstractInputPump::PumpFunction pumpFunction = nullptr)  : AbstractInputPump(targetAxis, pumpFunction), sourceButton_(sourceButton)
		{
			// Prevent wrong "Now"-Actions during the Update of instantiation
			PumpInput();
			GetVirtualAxis()->BeginUpdate();
		}

		~MouseInputPump()  = default;

		float GetSourceState() const  override
		{
			return MouseInput::Get().IsButtonDown(sourceButton_) ? 1.0f : 0.0f;
		}

		MouseInput::Button GetSourceButton() const 
		{ 
			return sourceButton_; 
		}

	private:

		MouseInput::Button sourceButton_;
	};




	class GamepadInputPump : public AbstractInputPump
	{
	public:

		GamepadInputPump(int sourceGamepadIndex, SDL_GameControllerButton sourceButton, VirtualAxis& targetAxis, AbstractInputPump::PumpFunction pumpFunction = nullptr) : AbstractInputPump(targetAxis, pumpFunction),
			sourceGamepadIndex_(sourceGamepadIndex),
			sourceButton_(sourceButton),
			sourceAxis_(SDL_CONTROLLER_AXIS_INVALID)
		{
			// Prevent wrong "Now"-Actions during the Update of instantiation
			PumpInput();
			GetVirtualAxis()->BeginUpdate();
		}

		GamepadInputPump(int sourceGamepadIndex, SDL_GameControllerAxis sourceAxis, VirtualAxis& targetAxis, AbstractInputPump::PumpFunction pumpFunction = nullptr) : AbstractInputPump(targetAxis, pumpFunction),
			sourceGamepadIndex_(sourceGamepadIndex),
			sourceButton_(SDL_CONTROLLER_BUTTON_INVALID),
			sourceAxis_(sourceAxis)
		{
			// Prevent wrong "Now"-Actions during the Update of instantiation
			PumpInput();
			GetVirtualAxis()->BeginUpdate();
		}

		~GamepadInputPump() = default;

		float GetSourceState() const override
		{
			if (sourceButton_ != SDL_CONTROLLER_BUTTON_INVALID)
				return GamepadInput::Get().IsButtonDown(sourceGamepadIndex_, sourceButton_) ? 1.0f : 0.0f;
			else if (sourceAxis_ != SDL_CONTROLLER_AXIS_INVALID)
				return GamepadInput::Get().GetAxisValue(sourceGamepadIndex_, sourceAxis_);

			return 0.0f;
		}

		int GetSourceGamepadIndex() const { return sourceGamepadIndex_; }
		SDL_GameControllerButton GetSourceBoutton() const { return sourceButton_; }
		SDL_GameControllerAxis  GetSourceAxis() const { return sourceAxis_; }


	private:

		int sourceGamepadIndex_;
		SDL_GameControllerButton sourceButton_;
		SDL_GameControllerAxis   sourceAxis_;
	};




	class VirtualInputPump : public AbstractInputPump
	{
	public:
		VirtualInputPump(VirtualAxis& axis, PumpFunction pumpFunction = nullptr) : AbstractInputPump(axis, pumpFunction), sourceValue_(0.0f)
		{
			PumpInput();
			GetVirtualAxis()->BeginUpdate();
		}

		~VirtualInputPump() = default;

		void SetSourceValue(float value)
		{
			sourceValue_ = value;
		}

		float GetSourceState() const override
		{
			return sourceValue_;
		}

	private:

		float sourceValue_;
	};


}
