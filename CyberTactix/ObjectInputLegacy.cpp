#include "ObjectInputLegacy.h"

namespace pix
{
	
	void ObjectInputLegacy::Update() 
	{
		SyncPreviousInput();

		ResetAxisState();

		PumpInput();

	}


	bool ObjectInputLegacy::AddKeyboardBinding(SDL_Scancode sourceKey, const std::string& axisName, AbstractInputPump::PumpFunction pumpFunction) 
	{
		VirtualAxis* targetAxis = GetOrAddVirtualAxis(axisName);
		if (targetAxis == nullptr)
			return false;

		std::string key = std::to_string(sourceKey) + "|" + axisName; // separator "_" guarantees unique keys (problem example: "4" + "45" = "44" + "5")

		if (keyboardInputPumps_.find(key) != keyboardInputPumps_.end())
			return false;

		return keyboardInputPumps_.emplace(key, KeyboardInputPump(sourceKey, *targetAxis, pumpFunction)).second;
	}

	bool ObjectInputLegacy::RemoveKeyboardBinding(SDL_Scancode sourceKey, const std::string& axisName) 
	{
		std::string key = std::to_string(sourceKey) + "|" + axisName;

		return keyboardInputPumps_.erase(key) > 0;
	}

	bool ObjectInputLegacy::AddMouseBinding(MouseInput::Button sourceButton, const std::string& axisName, AbstractInputPump::PumpFunction pumpFunction) 
	{
		VirtualAxis* targetAxis = GetOrAddVirtualAxis(axisName);
		if (targetAxis == nullptr) return false;

		std::string key = std::to_string(sourceButton) + "|" + axisName;

		if (mouseInputPumps_.find(key) != mouseInputPumps_.end())
			return false;

		return mouseInputPumps_.emplace(key, MouseInputPump(sourceButton, *targetAxis, pumpFunction)).second;
	}

	bool ObjectInputLegacy::RemoveMouseBinding(MouseInput::Button sourceButton, const std::string& axisName) 
	{
		if (axisName.empty()) return false;

		std::string key = std::to_string(sourceButton) + "|" + axisName;

		return mouseInputPumps_.erase(key) > 0;
	}

	bool ObjectInputLegacy::AddGamepadButtonBinding(int sourceGamepadIndex, SDL_GameControllerButton sourceButton, const std::string& axisName, AbstractInputPump::PumpFunction pumpFunction) 
	{
		if (!GamepadInput::Get().IsValidGamepadIndex(sourceGamepadIndex)) return false;

		VirtualAxis* targetAxis = GetOrAddVirtualAxis(axisName);
		if (targetAxis == nullptr) return false;

		std::string key = std::to_string(sourceGamepadIndex) + "|" + std::to_string(sourceButton) + "|" + axisName;

		if (gamepadInputPumps_.find(key) != gamepadInputPumps_.end())
			return false;

		return gamepadInputPumps_.emplace(key, GamepadInputPump(sourceGamepadIndex, sourceButton, *targetAxis, pumpFunction)).second;
	}

	bool ObjectInputLegacy::RemoveGamepadButtonBinding(int sourceGamepadIndex, SDL_GameControllerButton sourceButton, const std::string& axisName) 
	{
		std::string key = std::to_string(sourceGamepadIndex) + "|" + std::to_string(sourceButton) + "|" + axisName;

		return gamepadInputPumps_.erase(key) > 0;
	}

	bool ObjectInputLegacy::AddGamepadAxisBinding(int sourceGamepadIndex, SDL_GameControllerAxis sourceAxis, const std::string& axisName, AbstractInputPump::PumpFunction pumpFunction) 
	{
		if (!GamepadInput::Get().IsValidGamepadIndex(sourceGamepadIndex)) return false;

		VirtualAxis* targetAxis = GetOrAddVirtualAxis(axisName);
		if (targetAxis == nullptr) return false;

		std::string key = std::to_string(sourceGamepadIndex) + "|" + std::to_string(sourceAxis) + "|" + axisName;

		if (gamepadInputPumps_.find(key) != gamepadInputPumps_.end())
			return false;

		return gamepadInputPumps_.emplace(key, GamepadInputPump(sourceGamepadIndex, sourceAxis, *targetAxis, pumpFunction)).second;

	}

	bool ObjectInputLegacy::RemoveGamepadAxisBinding(int sourceGamepadIndex, SDL_GameControllerAxis sourceAxis, const std::string& axisName) 
	{
		std::string key = std::to_string(sourceGamepadIndex) + "|" + std::to_string(sourceAxis) + "|" + axisName;

		return gamepadInputPumps_.erase(key) > 0;
	}

	bool ObjectInputLegacy::AddVirtualBinding(int sourceID, const std::string& axisName, AbstractInputPump::PumpFunction pumpFunction) 
	{
		VirtualAxis* targetAxis = GetOrAddVirtualAxis(axisName);
		if (targetAxis == nullptr) //|| sourceID.empty())
			return false;

		std::string key = std::to_string(sourceID) + "|" + axisName;

		return virtualInputPumps_.emplace(key, VirtualInputPump(sourceID, *targetAxis, pumpFunction)).second;

	}

	bool ObjectInputLegacy::RemoveVirtualBinding(int sourceID, const std::string& axisName) 
	{
		std::string key = std::to_string(sourceID) + "|" + axisName;

		return virtualInputPumps_.erase(key) > 0;
	}


	bool ObjectInputLegacy::SetVirtualInput(int sourceID, const std::string& axisName, float sourceValue) 
	{
		std::string key = std::to_string(sourceID) + "|" + axisName;

		auto it = virtualInputPumps_.find(key);
		if (it != virtualInputPumps_.end())
		{
			it->second.SetSourceState(sourceValue);
			return true;
		}

		return false;
	}

	bool ObjectInputLegacy::SetDeadZone(const std::string& axisName, float value) 
	{
		auto it = virtualAxes_.find(axisName);

		if (it != virtualAxes_.end())
		{
			it->second.SetDeadZone(value);
			return true;
		}

		return false;
	}

	void ObjectInputLegacy::ResetState() 
	{
		for (auto& pair : virtualAxes_)
			pair.second.Reset();

		for (auto& pair : virtualInputPumps_)
			pair.second.SetSourceState(0.0f);
	}


	bool ObjectInputLegacy::IsPositive(const std::string& axisName) const 
	{
		auto it = virtualAxes_.find(axisName);

		if (it != virtualAxes_.end())
			return it->second.IsPositive();

		return false;
	}

	bool ObjectInputLegacy::IsNowPositive(const std::string& axisName) const 
	{
		auto it = virtualAxes_.find(axisName);

		if (it != virtualAxes_.end())
			return it->second.BecamePositive();

		return false;
	}

	bool ObjectInputLegacy::IsNowZeroFromPositive(const std::string& axisName) const 
	{
		auto it = virtualAxes_.find(axisName);

		if (it != virtualAxes_.end())
			return it->second.BecameZeroFromPositive();

		return false;
	}

	bool ObjectInputLegacy::IsNegative(const std::string& axisName) const 
	{
		auto it = virtualAxes_.find(axisName);

		if (it != virtualAxes_.end())
			return it->second.IsNegative();

		return false;
	}

	bool ObjectInputLegacy::IsNowNegative(const std::string& axisName) const 
	{
		auto it = virtualAxes_.find(axisName);

		if (it != virtualAxes_.end())
			return it->second.BecameNegative();

		return false;
	}

	bool ObjectInputLegacy::IsNowZeroFromNegative(const std::string& axisName) const 
	{
		auto it = virtualAxes_.find(axisName);

		if (it != virtualAxes_.end())
			return it->second.BecameZeroFromNegative();

		return false;
	}

	bool ObjectInputLegacy::IsNowZero(const std::string& axisName) const 
	{
		auto it = virtualAxes_.find(axisName);

		if (it != virtualAxes_.end())
			return it->second.BecameZeroFromPositive() || it->second.BecameZeroFromNegative();

		return false;
	}

	float ObjectInputLegacy::GetAxisState(const std::string& axisName) const 
	{
		auto it = virtualAxes_.find(axisName);

		if (it != virtualAxes_.end())
			return it->second.GetAxisState();

		return 0.0f;
	}

	bool ObjectInputLegacy::ContainsAxis(const std::string& axisName) const  // To support manual error handling
	{
		return virtualAxes_.find(axisName) != virtualAxes_.end();
	}


	VirtualAxis* ObjectInputLegacy::GetOrAddVirtualAxis(const std::string& axisName) 
	{
		if (axisName.empty()) return nullptr;

		auto it = virtualAxes_.find(axisName);

		if (it != virtualAxes_.end())
			return &(it->second);

		return &(virtualAxes_.emplace(axisName, VirtualAxis(axisName, -1)).first->second);
	}

	void ObjectInputLegacy::SyncPreviousInput() 
	{
		for (auto& pair : virtualAxes_)
			pair.second.BeginUpdate();
	}

	void ObjectInputLegacy::ResetAxisState() 
	{
		for (auto& pair : virtualAxes_)
			pair.second.SetAxisState(0.0f);
	}

	void ObjectInputLegacy::PumpInput() 
	{
		for (auto& pair : keyboardInputPumps_)
			pair.second.Pump();
		for (auto& pair : gamepadInputPumps_)
			pair.second.Pump();
		for (auto& pair : mouseInputPumps_)
			pair.second.Pump();
		for (auto& pair : virtualInputPumps_)
			pair.second.Pump();
	}

	
	void ObjectInputLegacy::RelinkPumpsToAxes() 
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
	

	/*
	bool ObjectInput::RelinkPumpsToAxes()
	{
		for (...) 
		{
			auto name = pair.second.GetVirtualAxis()->GetName();
			auto it = virtualAxes_.find(name);
			if (it == virtualAxes_.end()) return false;
			pair.second.SetVirtualAxis(it->second);
		}
		return true;
	}
	*/



}