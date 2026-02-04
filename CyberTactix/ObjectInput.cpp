#include "ObjectInput.h"

namespace pix
{
	
	void ObjectInput::Update() 
	{
		UpdatePreviousInput();

		ResetAllAxisValues();

		PumpInput();

		//Debug.LogException(e);
	}


	bool ObjectInput::AddKeyboardInput(SDL_Scancode sourceKey, const std::string& axisName, AbstractInputPump::PumpFunction pumpFunction) 
	{
		VirtualAxis* targetAxis = GetOrAddVirtualAxis(axisName);
		if (targetAxis == nullptr)
			return false;

		std::string key = std::to_string(sourceKey) + axisName;

		if (keyboardInputPumps_.find(key) != keyboardInputPumps_.end())
			return false;

		return keyboardInputPumps_.emplace(key, KeyboardInputPump(sourceKey, *targetAxis, pumpFunction)).second;
	}

	bool ObjectInput::DeleteKeyboardInput(SDL_Scancode sourceKey, const std::string& axisName) 
	{
		std::string key = std::to_string(sourceKey) + axisName;

		return keyboardInputPumps_.erase(key) > 0;
	}

	bool ObjectInput::AddMouseInput(MouseInput::Button sourceButton, const std::string& axisName, AbstractInputPump::PumpFunction pumpFunction) 
	{
		VirtualAxis* targetAxis = GetOrAddVirtualAxis(axisName);
		if (targetAxis == nullptr) return false;

		std::string key = std::to_string(sourceButton) + axisName;

		if (mouseInputPumps_.find(key) != mouseInputPumps_.end())
			return false;

		return mouseInputPumps_.emplace(key, MouseInputPump(sourceButton, *targetAxis, pumpFunction)).second;
	}

	bool ObjectInput::DeleteMouseInput(MouseInput::Button sourceButton, const std::string& axisName) 
	{
		if (axisName.empty()) return false;

		std::string key = std::to_string(sourceButton) + axisName;

		return mouseInputPumps_.erase(key) > 0;
	}

	bool ObjectInput::AddGamepadButtonInput(int sourceGamepadIndex, SDL_GameControllerButton sourceButton, std::string axisName, AbstractInputPump::PumpFunction pumpFunction) 
	{
		if (!GamepadInput::Get().IsValidGamepadIndex(sourceGamepadIndex)) return false;

		VirtualAxis* targetAxis = GetOrAddVirtualAxis(axisName);
		if (targetAxis == nullptr) return false;

		std::string key = std::to_string(sourceGamepadIndex) + std::to_string(sourceButton) + axisName;

		if (gamepadInputPumps_.find(key) != gamepadInputPumps_.end())
			return false;

		return gamepadInputPumps_.emplace(key, GamepadInputPump(sourceGamepadIndex, sourceButton, *targetAxis, pumpFunction)).second;
	}

	bool ObjectInput::DeleteGamepadButtonInput(int sourceGamepadIndex, SDL_GameControllerButton sourceButton, const std::string& axisName) 
	{
		std::string key = std::to_string(sourceGamepadIndex) + std::to_string(sourceButton) + axisName;

		return gamepadInputPumps_.erase(key) > 0;
	}

	bool ObjectInput::AddGamepadAxisInput(int sourceGamepadIndex, SDL_GameControllerAxis sourceAxis, std::string axisName, AbstractInputPump::PumpFunction pumpFunction) 
	{
		if (!GamepadInput::Get().IsValidGamepadIndex(sourceGamepadIndex)) return false;

		VirtualAxis* targetAxis = GetOrAddVirtualAxis(axisName);
		if (targetAxis == nullptr) return false;

		std::string key = std::to_string(sourceGamepadIndex) + std::to_string(sourceAxis) + axisName;

		if (gamepadInputPumps_.find(key) != gamepadInputPumps_.end())
			return false;

		return gamepadInputPumps_.emplace(key, GamepadInputPump(sourceGamepadIndex, sourceAxis, *targetAxis, pumpFunction)).second;

	}

	bool ObjectInput::DeleteGamepadAxisInput(int sourceGamepadIndex, SDL_GameControllerAxis sourceAxis, const std::string& axisName) 
	{
		std::string key = std::to_string(sourceGamepadIndex) + std::to_string(sourceAxis) + axisName;

		return gamepadInputPumps_.erase(key) > 0;
	}

	bool ObjectInput::AddVirtualInput(const std::string& sourceID, const std::string& axisName, AbstractInputPump::PumpFunction pumpFunction) 
	{
		VirtualAxis* targetAxis = GetOrAddVirtualAxis(axisName);
		if (targetAxis == nullptr) //|| sourceID.empty())
			return false;

		std::string key = sourceID + axisName;

		return virtualInputPumps_.emplace(key, VirtualInputPump(*targetAxis, pumpFunction)).second;

	}

	bool ObjectInput::DeleteVirtualInput(const std::string& sourceID, const std::string& axisName) 
	{
		std::string key = sourceID + axisName;

		return virtualInputPumps_.erase(key) > 0;
	}


	bool ObjectInput::SetVirtualInput(const std::string& sourceID, const std::string& axisName, float sourceValue) 
	{
		std::string key = sourceID + axisName;

		auto it = virtualInputPumps_.find(key);
		if (it != virtualInputPumps_.end())
		{
			it->second.SetSourceValue(sourceValue);
			return true;
		}

		return false;
	}

	bool ObjectInput::SetDeadZone(const std::string& axisName, float value) 
	{
		auto it = virtualAxes_.find(axisName);

		if (it != virtualAxes_.end())
		{
			it->second.SetDeadZone(value);
			return true;
		}

		return false;
	}

	void ObjectInput::ResetState() 
	{
		for (auto& pair : virtualAxes_)
			pair.second.ResetState();

		for (auto& pair : virtualInputPumps_)
			pair.second.SetSourceValue(0.0f);
	}


	bool ObjectInput::IsPositive(const std::string& axisName) const 
	{
		auto it = virtualAxes_.find(axisName);

		if (it != virtualAxes_.end())
			return it->second.IsPositive();

		return false;
	}

	bool ObjectInput::IsNowPositive(const std::string& axisName) const 
	{
		auto it = virtualAxes_.find(axisName);

		if (it != virtualAxes_.end())
			return it->second.IsNowPositive();

		return false;
	}

	bool ObjectInput::IsNowZeroFromPositive(const std::string& axisName) const 
	{
		auto it = virtualAxes_.find(axisName);

		if (it != virtualAxes_.end())
			return it->second.IsNowZeroFromPositive();

		return false;
	}

	bool ObjectInput::IsNegative(const std::string& axisName) const 
	{
		auto it = virtualAxes_.find(axisName);

		if (it != virtualAxes_.end())
			return it->second.IsNegative();

		return false;
	}

	bool ObjectInput::IsNowNegative(const std::string& axisName) const 
	{
		auto it = virtualAxes_.find(axisName);

		if (it != virtualAxes_.end())
			return it->second.IsNowNegative();

		return false;
	}

	bool ObjectInput::IsNowZeroFromNegative(const std::string& axisName) const 
	{
		auto it = virtualAxes_.find(axisName);

		if (it != virtualAxes_.end())
			return it->second.IsNowZeroFromNegative();

		return false;
	}

	bool ObjectInput::IsNowZero(const std::string& axisName) const 
	{
		auto it = virtualAxes_.find(axisName);

		if (it != virtualAxes_.end())
			return it->second.IsNowZeroFromPositive() || it->second.IsNowZeroFromNegative();

		return false;
	}

	float ObjectInput::GetAxisValue(const std::string& axisName) const 
	{
		auto it = virtualAxes_.find(axisName);

		if (it != virtualAxes_.end())
			return it->second.GetAxisState();

		return 0.0f;
	}

	bool ObjectInput::ContainsAxis(const std::string& axisName) const  // To support manual error handling
	{
		return virtualAxes_.find(axisName) != virtualAxes_.end();
	}


	VirtualAxis* ObjectInput::GetOrAddVirtualAxis(const std::string& axisName) 
	{
		if (axisName.empty()) return nullptr;

		auto it = virtualAxes_.find(axisName);

		if (it != virtualAxes_.end())
			return &(it->second);

		return &(virtualAxes_.emplace(axisName, axisName).first->second);
	}

	void ObjectInput::UpdatePreviousInput() 
	{
		for (auto& pair : virtualAxes_)
			pair.second.BeginUpdate();
	}

	void ObjectInput::ResetAllAxisValues() 
	{
		for (auto& pair : virtualAxes_)
			pair.second.SetAxisState(0.0f);
	}

	void ObjectInput::PumpInput() 
	{
		for (auto& pair : keyboardInputPumps_)
			pair.second.PumpInput();
		for (auto& pair : gamepadInputPumps_)
			pair.second.PumpInput();
		for (auto& pair : mouseInputPumps_)
			pair.second.PumpInput();
		for (auto& pair : virtualInputPumps_)
			pair.second.PumpInput();
	}

	void ObjectInput::RelinkPumpsToAxes() 
	{
		for (auto& pair : keyboardInputPumps_)
			pair.second.SetVirtualAxis(virtualAxes_.at(pair.second.GetVirtualAxis()->GetName()));
		for (auto& pair : mouseInputPumps_)
			pair.second.SetVirtualAxis(virtualAxes_.at(pair.second.GetVirtualAxis()->GetName()));
		for (auto& pair : gamepadInputPumps_)
			pair.second.SetVirtualAxis(virtualAxes_.at(pair.second.GetVirtualAxis()->GetName()));
		for (auto& pair : virtualInputPumps_)
			pair.second.SetVirtualAxis(virtualAxes_.at(pair.second.GetVirtualAxis()->GetName()));
	}

}