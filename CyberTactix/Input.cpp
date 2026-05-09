#include "Input.h"
#include "ErrorLogger.h"

namespace pix
{

	KeyboardInput& KeyboardInput::Get() 
	{
		static KeyboardInput keyboardInputSource_;

		return keyboardInputSource_;
	}

	bool KeyboardInput::IsKeyDown(SDL_Scancode key) const
	{
		if (!currentKeyStates_) return false;

		return currentKeyStates_[key] != 0;
	}
	

	KeyboardInput::KeyboardInput()
	{
		currentKeyStates_ = SDL_GetKeyboardState(nullptr);

		if (!currentKeyStates_)
			ErrorLogger::Get().LogError("KeyboardInput::KeyboardInput() failure", "currentKeyStates_ is nullptr!");
	}




	MouseInput& MouseInput::Get() 
	{
		static MouseInput mouseInputSource_;

		return mouseInputSource_;
	}

	void MouseInput::EndUpdate()
	{
		wheelDeltaXInUpdate_ = wheelDeltaYInUpdate_ = 0.0f;
	}

	void MouseInput::EndRender()
	{
		wheelDeltaXInRender_ = wheelDeltaYInRender_ = 0.0f;
	}

	void MouseInput::HandleEvents(const SDL_Event& event)
	{
		if (event.type == SDL_MOUSEWHEEL)
		{
			float wheelX = event.wheel.preciseX;
			float wheelY = event.wheel.preciseY;

			// Normalize flipped wheel events so positive X/Y follow SDL's normal direction convention
			if (event.wheel.direction == SDL_MOUSEWHEEL_FLIPPED)
			{
				wheelX = -wheelX;
				wheelY = -wheelY;
			}

			wheelDeltaXInUpdate_ += wheelX;
			wheelDeltaYInUpdate_ += wheelY;
			wheelDeltaXInRender_ += wheelX;
			wheelDeltaYInRender_ += wheelY;
		}
	}

	void MouseInput::Update() 
	{
		buttonFlags_ = SDL_GetMouseState(&positionX_, &positionY_);
	}

	bool MouseInput::IsButtonDown(Button button) const
	{
		return (buttonFlags_ & SDL_BUTTON(button)) != 0;
	}

	Vec2i MouseInput::GetMousePosition() const
	{
		return Vec2i(positionX_, positionY_);
	}

	int MouseInput::GetMousePositionX() const
	{
		return positionX_;
	}
	
	int MouseInput::GetMousePositionY() const
	{
		return positionY_;
	}

	float MouseInput::GetUpdateWheelDeltaX() const
	{
		return wheelDeltaXInUpdate_;
	}

	float MouseInput::GetUpdateWheelDeltaY() const
	{
		return wheelDeltaYInUpdate_;
	}

	float MouseInput::GetRenderWheelDeltaX() const
	{
		return wheelDeltaXInRender_;
	}

	float MouseInput::GetRenderWheelDeltaY() const
	{
		return wheelDeltaYInRender_;
	}



	GamepadInput& GamepadInput::Get() 
	{
		static GamepadInput gamepadInputSource_;

		return gamepadInputSource_;
	}

	int GamepadInput::AddAllGamepads() 
	{
		RemoveAllGamepads();

		int gamepadCount = SDL_NumJoysticks();

		if (gamepadCount < 0)
		{
			ErrorLogger::Get().LogSDLError("GamepadInput::AddAllGamepads() - SDL_NumJoysticks() failure");
			return 0;
		}

		if (gamepadCount > 32) gamepadCount = 32; // Just a sanity measure to prevent huge numbers

		for (int i = 0; i < gamepadCount; i++)
			AddGamepad(i);
		
		return gamepads_.size();
	}

	void GamepadInput::AddGamepad(int deviceIndex)
	{
		// 1. Must be a valid game controller
		if (!SDL_IsGameController(deviceIndex))
			return;

		// 2. Get joystick instance id without opening the controller
		SDL_JoystickID joystickID = SDL_JoystickGetDeviceInstanceID(deviceIndex);
		if (joystickID < 0)
		{
			ErrorLogger::Get().LogError("GamepadInput::AddGamepad() - SDL_JoystickGetDeviceInstanceID() failure", "The joystick instance ID is invalid!");
			return;
		}

		// 3. Check if joystick is already in use
		int gamepadSlotCount = gamepads_.size();
		for (int i = 0; i < gamepadSlotCount; i++)
		{
			if (gamepads_[i].IsInitialized && gamepads_[i].JoystickID == joystickID)
				return;
		}

		// 4. Open controller 
		SDL_GameController* gameController = SDL_GameControllerOpen(deviceIndex);
		if (!gameController)
		{
			ErrorLogger::Get().LogSDLError("GamepadInput::AddGamepad() - SDL_GameControllerOpen() failure");
			return;
		}

		// 5. Open haptics (optional)
		SDL_Joystick* joystick = SDL_GameControllerGetJoystick(gameController);
		SDL_Haptic* rumbleHandle = TryOpenRumble(joystick);

		// 6. Reuse an inactive slot if possible (stable indices)
		for (int i = 0; i < gamepadSlotCount; i++)
		{
			if (!gamepads_[i].IsInitialized)
			{
				gamepads_[i].Set(gameController, rumbleHandle, joystickID, true);
				return;
			}
		}

		// 7. Otherwise append a new slot
		gamepads_.emplace_back(gameController, rumbleHandle, joystickID, true);
	}


	int GamepadInput::RemoveAllGamepads() 
	{
		const int slotCount = gamepads_.size();
		int removedCount = 0;

		for (int i = 0; i < slotCount; i++)
		{
			if (gamepads_[i].IsInitialized)
			{
				RemoveGamepad(gamepads_[i].JoystickID);
				removedCount++;
			}
		}

		gamepads_.clear();

		return removedCount;
	}

	void GamepadInput::RemoveGamepad(SDL_JoystickID joystickID)
	{
		int gamepadCount = gamepads_.size();

		for (int i = gamepadCount - 1; i >= 0; i--)
		{
			if (gamepads_[i].IsInitialized && gamepads_[i].JoystickID == joystickID)
			{
				if (gamepads_[i].RumbleHandle)
				{
					SDL_HapticRumbleStop(gamepads_[i].RumbleHandle);
					SDL_HapticClose(gamepads_[i].RumbleHandle);
				}

				if (gamepads_[i].GameController)
					SDL_GameControllerClose(gamepads_[i].GameController);

				// Slot is kept to preserve stable gamepadIndex
				gamepads_[i].Set(nullptr, nullptr, -1, false);

				break;
			}
		}
	}


	int GamepadInput::GetActiveGamepadCount() const
	{
		int gamepadCount = gamepads_.size();
		int count = 0;

		for (int i = 0; i < gamepadCount; i++)
		{
			if (gamepads_[i].IsInitialized)
				count++;
		}

		return count;
	}

	int GamepadInput::GetGamepadSlotCount() const
	{
		return gamepads_.size();
	}

	bool GamepadInput::IsValidGamepadIndex(int gamepadIndex) const 
	{
		return  gamepadIndex >= 0 && gamepadIndex < gamepads_.size() && gamepads_[gamepadIndex].IsInitialized;
	}

	bool GamepadInput::IsButtonDown(int gamepadIndex, SDL_GameControllerButton button) const 
	{
		if (!IsValidGamepadIndex(gamepadIndex)) return false;
		
		return SDL_GameControllerGetButton(gamepads_[gamepadIndex].GameController, button) != 0;

	}

	float GamepadInput::GetAxisValue(int gamepadIndex, SDL_GameControllerAxis axis) const
	{
		if (!IsValidGamepadIndex(gamepadIndex)) return 0.0f;
		
		// SDL_GameControllerGetAxis() returns a value in the range [-32768, 32767]. Triggers, however, range from [0, 32767].
		float axisValue = SDL_GameControllerGetAxis(gamepads_[gamepadIndex].GameController, axis) / float(SDL_JOYSTICK_AXIS_MAX);
		
		return GetClamped(axisValue, -1.0f, 1.0f); // Clamp defensively
	}

	void GamepadInput::StartRumble(int gamepadIndex, float force, int duration) 
	{
		if (!IsValidGamepadIndex(gamepadIndex) || force <= 0.0f || duration == 0) return;

		if (force > 1.0f) force = 1.0f;

		Uint32 convertedDuration = duration < 0 ? SDL_HAPTIC_INFINITY : (Uint32)duration;

		if (gamepads_[gamepadIndex].RumbleHandle != nullptr && SDL_HapticRumblePlay(gamepads_[gamepadIndex].RumbleHandle, force, convertedDuration) != 0)
			ErrorLogger::Get().LogSDLError("GamepadInput::StartRumble() - SDL_HapticRumblePlay() failure");
	}

	void GamepadInput::StopRumble(int gamepadIndex) 
	{
		if (!IsValidGamepadIndex(gamepadIndex)) return;

		if (gamepads_[gamepadIndex].RumbleHandle != nullptr && SDL_HapticRumbleStop(gamepads_[gamepadIndex].RumbleHandle) != 0)
			ErrorLogger::Get().LogSDLError("GamepadInput::StopRumble() - SDL_HapticRumbleStop() failure");

	}

	bool GamepadInput::HasRumble(int gamepadIndex) const
	{
		if (!IsValidGamepadIndex(gamepadIndex)) return false;

		return gamepads_[gamepadIndex].RumbleHandle != nullptr;
	}

	



	GamepadInput::Gamepad::Gamepad(SDL_GameController* gameController, SDL_Haptic* rumbleHandle, SDL_JoystickID joystickID, bool isInitialized) :
		GameController(gameController),
		RumbleHandle(rumbleHandle),
		JoystickID(joystickID),
		IsInitialized(isInitialized)
	{
	}

	void GamepadInput::Gamepad::Set(SDL_GameController* gameController, SDL_Haptic* rumbleHandle, SDL_JoystickID joystickID, bool isInitialized)
	{
		GameController = gameController;
		RumbleHandle = rumbleHandle;
		JoystickID = joystickID;
		IsInitialized = isInitialized;
	}


	GamepadInput::~GamepadInput()
	{
		RemoveAllGamepads();
	}

	SDL_Haptic* GamepadInput::TryOpenRumble(SDL_Joystick* joystick)
	{
		if (!joystick)
		{
			ErrorLogger::Get().LogError("GamepadInput::TryOpenRumble() failure", "joystick is nullptr!");
			return nullptr;
		}

		int isHaptic = SDL_JoystickIsHaptic(joystick);

		if (isHaptic < 0)
		{
			ErrorLogger::Get().LogSDLError("GamepadInput::TryOpenRumble() - SDL_JoystickIsHaptic() failure");
			return nullptr;
		}

		if (isHaptic == 0) return nullptr;

		SDL_Haptic* haptic = SDL_HapticOpenFromJoystick(joystick);

		if (!haptic)
		{
			ErrorLogger::Get().LogSDLError("GamepadInput::TryOpenRumble() - SDL_HapticOpenFromJoystick() failure");
			return nullptr;
		}

		int supportCode = SDL_HapticRumbleSupported(haptic);

		if (supportCode < 0)
		{
			ErrorLogger::Get().LogSDLError("GamepadInput::TryOpenRumble() - SDL_HapticRumbleSupported() failure");
			SDL_HapticClose(haptic);
			return nullptr;
		}
		else if (supportCode == 0)
		{
			SDL_HapticClose(haptic);
			return nullptr;
		}
		else if (SDL_HapticRumbleInit(haptic) != 0)
		{
			ErrorLogger::Get().LogSDLError("GamepadInput::TryOpenRumble() - SDL_HapticRumbleInit() failure");
			SDL_HapticClose(haptic);
			return nullptr;
		}

		return haptic;
	}

}