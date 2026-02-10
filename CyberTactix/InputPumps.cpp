#include "InputPumps.h"
#include "PixMath.h"

namespace pix
{

		KeyboardInputPump::KeyboardInputPump(SDL_Scancode sourceKey, VirtualAxis& targetAxis, AbstractInputPump::PumpFunction pumpFunction) : AbstractInputPump(targetAxis, pumpFunction), 
			sourceKey_(sourceKey)
		{
		}

		void KeyboardInputPump::SetSourceKey(SDL_Scancode sourceKey)
		{
			sourceKey_ = sourceKey;
		}

		float KeyboardInputPump::GetSourceState() const
		{
			return KeyboardInput::Get().IsKeyDown(sourceKey_) ? 1.0f : 0.0f;
		}

		SDL_Scancode KeyboardInputPump::GetSourceKey() const 
		{
			return sourceKey_; 
		}



		MouseButtonInputPump::MouseButtonInputPump(MouseInput::Button sourceButton, VirtualAxis& targetAxis, AbstractInputPump::PumpFunction pumpFunction) : AbstractInputPump(targetAxis, pumpFunction), 
			sourceButton_(sourceButton)
		{
		}

		void MouseButtonInputPump::SetSourceButton(MouseInput::Button sourceButton)
		{
			sourceButton_ = sourceButton;
		}

		float MouseButtonInputPump::GetSourceState() const
		{
			return MouseInput::Get().IsButtonDown(sourceButton_) ? 1.0f : 0.0f;
		}

		MouseInput::Button MouseButtonInputPump::GetSourceButton() const
		{
			return sourceButton_;
		}




		GamepadInputPump::GamepadInputPump(int sourceGamepadIndex, SDL_GameControllerButton sourceButton, VirtualAxis& targetAxis, AbstractInputPump::PumpFunction pumpFunction) : AbstractInputPump(targetAxis, pumpFunction),
			sourceGamepadIndex_(sourceGamepadIndex),
			sourceButton_(sourceButton),
			sourceAxis_(SDL_CONTROLLER_AXIS_INVALID)
		{
		}

		GamepadInputPump::GamepadInputPump(int sourceGamepadIndex, SDL_GameControllerAxis sourceAxis, VirtualAxis& targetAxis, AbstractInputPump::PumpFunction pumpFunction) : AbstractInputPump(targetAxis, pumpFunction),
			sourceGamepadIndex_(sourceGamepadIndex),
			sourceButton_(SDL_CONTROLLER_BUTTON_INVALID),
			sourceAxis_(sourceAxis)
		{
		}

		void GamepadInputPump::SetSourceGamepadIndex(int sourceGamepadIndex)
		{
			sourceGamepadIndex_ = sourceGamepadIndex;
		}

		void GamepadInputPump::SetSourceButton(SDL_GameControllerButton sourceButton)
		{
			sourceButton_ = sourceButton;
			sourceAxis_ = SDL_CONTROLLER_AXIS_INVALID;
		}

		void GamepadInputPump::SetSourceAxis(SDL_GameControllerAxis sourceAxis)
		{
			sourceAxis_ = sourceAxis;
			sourceButton_ = SDL_CONTROLLER_BUTTON_INVALID;
		}

		float GamepadInputPump::GetSourceState() const 
		{
			if (sourceButton_ != SDL_CONTROLLER_BUTTON_INVALID)
				return GamepadInput::Get().IsButtonDown(sourceGamepadIndex_, sourceButton_) ? 1.0f : 0.0f;
			else if (sourceAxis_ != SDL_CONTROLLER_AXIS_INVALID)
				return GamepadInput::Get().GetAxisValue(sourceGamepadIndex_, sourceAxis_);

			return 0.0f;
		}

		int GamepadInputPump::GetSourceGamepadIndex() const 
		{ 
			return sourceGamepadIndex_; 
		}

		SDL_GameControllerButton GamepadInputPump::GetSourceButton() const 
		{
			return sourceButton_; 
		}

		SDL_GameControllerAxis  GamepadInputPump::GetSourceAxis() const 
		{ 
			return sourceAxis_; 
		}

	



		VirtualInputPump::VirtualInputPump(int sourceID, VirtualAxis& targetAxis, PumpFunction pumpFunction) : AbstractInputPump(targetAxis, pumpFunction), 
			sourceState_(0.0f),
			sourceID_(sourceID)
		{
		}

		void VirtualInputPump::SetSourceState(float state)
		{
			sourceState_ = GetClampedValue(state, -1.0f, 1.0f);
		}

		void VirtualInputPump::SetSourceID(int sourceID)
		{
			sourceID_ = sourceID;
		}

		float VirtualInputPump::GetSourceState() const 
		{
			return sourceState_;
		}

		int VirtualInputPump::GetSourceID() const
		{
			return sourceID_;
		}

}
