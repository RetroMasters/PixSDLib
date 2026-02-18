#include "ObjectInput.h"

namespace pix
{
	ObjectInput::ObjectInput(const ObjectInput& other) :
		virtualAxes_(other.virtualAxes_),
		keyboardInputPumps_(other.keyboardInputPumps_),
		mouseInputPumps_(other.mouseInputPumps_),
		gamepadInputPumps_(other.gamepadInputPumps_),
		virtualInputPumps_(other.virtualInputPumps_)
	{
		RelinkPumpsToAxes();
	}


	ObjectInput& ObjectInput::operator= (const ObjectInput& other)
	{
		if (this == &other) return *this;

		virtualAxes_ = other.virtualAxes_;
		keyboardInputPumps_ = other.keyboardInputPumps_;
		mouseInputPumps_ = other.mouseInputPumps_;
		gamepadInputPumps_ = other.gamepadInputPumps_;
		virtualInputPumps_ = other.virtualInputPumps_;

		RelinkPumpsToAxes();

		return *this;
	}



	void ObjectInput::Update()
	{
		SyncPreviousInput();

		ResetAxisState();

		PumpInput();

	}


	bool ObjectInput::AddKeyboardBinding(SDL_Scancode sourceKey, const std::string& axisName, AbstractInputPump::PumpFunction pumpFunction)
	{
		VirtualAxis* targetAxis = GetOrAddVirtualAxis(axisName);
		if (!targetAxis) return false;

		if(GetKeyboardPumpIndex(sourceKey, axisName) >= 0) // Already exists
			return false;

		keyboardInputPumps_.emplace_back(sourceKey, *targetAxis, pumpFunction);
		// Sync transient state to prevent wrong "Now"-Actions during the update of instantiation
		keyboardInputPumps_.back().Pump();
		keyboardInputPumps_.back().GetVirtualAxis().BeginUpdate();

		return true;
	}

	bool ObjectInput::RemoveKeyboardBinding(SDL_Scancode sourceKey, const std::string& axisName)
	{
		int pumpIndex = GetKeyboardPumpIndex(sourceKey, axisName);
		if (pumpIndex < 0) return false;
		
		keyboardInputPumps_.erase(keyboardInputPumps_.begin() + pumpIndex);
		//keyboardInputPumps_[pumpIndex] = keyboardInputPumps_.back();
		//keyboardInputPumps_.pop_back();

		return true;
	}

	bool ObjectInput::AddMouseButtonBinding(MouseInput::Button sourceButton, const std::string& axisName, AbstractInputPump::PumpFunction pumpFunction)
	{
		VirtualAxis* targetAxis = GetOrAddVirtualAxis(axisName);
		if (!targetAxis) return false;

		if (GetMousePumpIndex(sourceButton, axisName) >= 0) // Already exists
			return false;

		mouseInputPumps_.emplace_back(sourceButton, *targetAxis, pumpFunction);
		// Sync transient state to prevent wrong "Now"-Actions during the update of instantiation
		mouseInputPumps_.back().Pump();
		mouseInputPumps_.back().GetVirtualAxis().BeginUpdate();

		return true;
	}

	bool ObjectInput::RemoveMouseButtonBinding(MouseInput::Button sourceButton, const std::string& axisName)
	{
		int pumpIndex = GetMousePumpIndex(sourceButton, axisName);
		if (pumpIndex < 0) return false;

		mouseInputPumps_.erase(mouseInputPumps_.begin() + pumpIndex);
	
		return true;
	}

	bool ObjectInput::AddGamepadButtonBinding(int sourceGamepadIndex, SDL_GameControllerButton sourceButton, const std::string& axisName, AbstractInputPump::PumpFunction pumpFunction)
	{
		if (!GamepadInput::Get().IsValidGamepadIndex(sourceGamepadIndex)) return false;

		VirtualAxis* targetAxis = GetOrAddVirtualAxis(axisName);
		if (!targetAxis) return false;

		if (GetGamepadPumpIndex(sourceGamepadIndex, sourceButton, axisName) >= 0) // Already exists
			return false;

		gamepadInputPumps_.emplace_back(sourceGamepadIndex, sourceButton, *targetAxis, pumpFunction);
		// Sync transient state to prevent wrong "Now"-Actions during the update of instantiation
		gamepadInputPumps_.back().Pump();
		gamepadInputPumps_.back().GetVirtualAxis().BeginUpdate();

		return true;
	}

	bool ObjectInput::RemoveGamepadButtonBinding(int sourceGamepadIndex, SDL_GameControllerButton sourceButton, const std::string& axisName)
	{
		int pumpIndex = GetGamepadPumpIndex(sourceGamepadIndex, sourceButton, axisName);
		if (pumpIndex < 0) return false;
		
		gamepadInputPumps_.erase(gamepadInputPumps_.begin() + pumpIndex);
		return true;
	}

	bool ObjectInput::AddGamepadAxisBinding(int sourceGamepadIndex, SDL_GameControllerAxis sourceAxis, const std::string& axisName, AbstractInputPump::PumpFunction pumpFunction)
	{
		if (!GamepadInput::Get().IsValidGamepadIndex(sourceGamepadIndex)) return false;

		VirtualAxis* targetAxis = GetOrAddVirtualAxis(axisName);
		if (!targetAxis) return false;

		if (GetGamepadPumpIndex(sourceGamepadIndex, sourceAxis, axisName) >= 0) // Already exists
			return false;

		gamepadInputPumps_.emplace_back(sourceGamepadIndex, sourceAxis, *targetAxis, pumpFunction);
		// Sync transient state to prevent wrong "Now"-Actions during the update of instantiation
		gamepadInputPumps_.back().Pump();
		gamepadInputPumps_.back().GetVirtualAxis().BeginUpdate();

		return true;
	}

	bool ObjectInput::RemoveGamepadAxisBinding(int sourceGamepadIndex, SDL_GameControllerAxis sourceAxis, const std::string& axisName)
	{
		int pumpIndex = GetGamepadPumpIndex(sourceGamepadIndex, sourceAxis, axisName);
		if (pumpIndex < 0) return false;

		gamepadInputPumps_.erase(gamepadInputPumps_.begin() + pumpIndex);
		return true;
	}

	bool ObjectInput::AddVirtualBinding(int sourceID, const std::string& axisName, AbstractInputPump::PumpFunction pumpFunction)
	{
		VirtualAxis* targetAxis = GetOrAddVirtualAxis(axisName);
		if (!targetAxis) return false;

		if (GetVirtualPumpIndex(sourceID, axisName) >= 0) // Already exists
			return false;

		virtualInputPumps_.emplace_back(sourceID, *targetAxis, pumpFunction);
		// Sync transient state to prevent wrong "Became"-Actions during the update of instantiation
		virtualInputPumps_.back().Pump();
		virtualInputPumps_.back().GetVirtualAxis().BeginUpdate();

		return true;
	}

	bool ObjectInput::RemoveVirtualBinding(int sourceID, const std::string& axisName)
	{
		int pumpIndex = GetVirtualPumpIndex(sourceID, axisName);
		if (pumpIndex < 0) return false;

		virtualInputPumps_.erase(virtualInputPumps_.begin() + pumpIndex);
		return true;	
	}


	bool ObjectInput::SetVirtualSourceState(int sourceID, const std::string& axisName, float sourceState)
	{
		int pumpIndex = GetVirtualPumpIndex(sourceID, axisName);
		if (pumpIndex < 0) return false;

		virtualInputPumps_[pumpIndex].SetSourceState(sourceState);
		return true;
	}

	bool ObjectInput::SetDeadZone(const std::string& axisName, float value)
	{
		VirtualAxis* axis = GetVirtualAxis(axisName);
		if (!axis) return false;
		
		axis->SetDeadZone(value);
		return true;
	}

	void ObjectInput::ResetAxes()
	{
		for (size_t i = 0; i < virtualAxes_.size(); i++)
			virtualAxes_[i].Reset();
	}

	void ObjectInput::ResetVirtualSourceState()
	{
		for (size_t i = 0; i < virtualInputPumps_.size(); i++)
			virtualInputPumps_[i].SetSourceState(0.0f);
	}


	bool ObjectInput::IsPositive(const std::string& axisName) const
	{
		const VirtualAxis* axis = GetVirtualAxis(axisName);
		if (!axis) return false;

		return axis->IsPositive();
	}

	bool ObjectInput::BecamePositive(const std::string& axisName) const
	{
		const VirtualAxis* axis = GetVirtualAxis(axisName);
		if (!axis) return false;

		return axis->BecamePositive();
	}

	bool ObjectInput::BecameZeroFromPositive(const std::string& axisName) const
	{
		const VirtualAxis* axis = GetVirtualAxis(axisName);
		if (!axis) return false;

		return axis->BecameZeroFromPositive();
	}

	bool ObjectInput::IsNegative(const std::string& axisName) const
	{
		const VirtualAxis* axis = GetVirtualAxis(axisName);
		if (!axis) return false;

		return axis->IsNegative();
	}

	bool ObjectInput::BecameNegative(const std::string& axisName) const
	{
		const VirtualAxis* axis = GetVirtualAxis(axisName);
		if (!axis) return false;

		return axis->BecameNegative();
	}

	bool ObjectInput::BecameZeroFromNegative(const std::string& axisName) const
	{
		const VirtualAxis* axis = GetVirtualAxis(axisName);
		if (!axis) return false;

		return axis->BecameZeroFromNegative();
	}

	bool ObjectInput::BecameZero(const std::string& axisName) const
	{
		const VirtualAxis* axis = GetVirtualAxis(axisName);
		if (!axis) return false;

		return axis->BecameZero();
	}

	float ObjectInput::GetAxisState(const std::string& axisName) const
	{
		const VirtualAxis* axis = GetVirtualAxis(axisName);
		if (!axis) return 0.0f;

		return axis->GetAxisState();
	}



	VirtualAxis* ObjectInput::GetOrAddVirtualAxis(const std::string& axisName)
	{
		if (axisName.empty()) return nullptr;

		VirtualAxis* axis = GetVirtualAxis(axisName);

		if (!axis)
		{
			virtualAxes_.emplace_back(axisName, virtualAxes_.size());
			axis = &(virtualAxes_.back());

			RelinkPumpsToAxes(); // In case the axis vector store gets relocated, update the axis binding
		}

		return axis;
	}

	
	const VirtualAxis* ObjectInput::GetVirtualAxis(const std::string& axisName) const
	{
		for (size_t i = 0; i < virtualAxes_.size(); i++)
		{
			if (virtualAxes_[i].GetName() == axisName)
				return &(virtualAxes_[i]);
		}

		return nullptr;
	}
	

	VirtualAxis* ObjectInput::GetVirtualAxis(const std::string& axisName) 
	{
		for (size_t i = 0; i < virtualAxes_.size(); i++)
		{
			if (virtualAxes_[i].GetName() == axisName)
				return &(virtualAxes_[i]);
		}

		return nullptr;
	}


    int ObjectInput::GetKeyboardPumpIndex(SDL_Scancode sourceKey, const std::string& axisName) const
	{
		for (size_t i = 0; i < keyboardInputPumps_.size(); i++)
		{
			if (keyboardInputPumps_[i].GetSourceKey() == sourceKey && keyboardInputPumps_[i].GetVirtualAxis().GetName() == axisName)
				return (int)i;
		}

		return -1;
	}

	int ObjectInput::GetMousePumpIndex(MouseInput::Button sourceButton, const std::string& axisName) const
	{
		for (size_t i = 0; i < mouseInputPumps_.size(); i++)
		{
			if (mouseInputPumps_[i].GetSourceButton() == sourceButton && mouseInputPumps_[i].GetVirtualAxis().GetName() == axisName)
				return (int)i;
		}

		return -1;
	}

	int ObjectInput::GetGamepadPumpIndex(int sourceGamepadIndex, SDL_GameControllerButton sourceButton, const std::string& axisName) const
	{
		for (size_t i = 0; i < gamepadInputPumps_.size(); i++)
		{
			if (gamepadInputPumps_[i].GetSourceGamepadIndex() == sourceGamepadIndex && gamepadInputPumps_[i].GetSourceButton() == sourceButton && gamepadInputPumps_[i].GetVirtualAxis().GetName() == axisName)
				return (int)i;
		}

		return -1;
	}

	int ObjectInput::GetGamepadPumpIndex(int sourceGamepadIndex, SDL_GameControllerAxis sourceAxis, const std::string& axisName) const
	{
		for (size_t i = 0; i < gamepadInputPumps_.size(); i++)
		{
			if (gamepadInputPumps_[i].GetSourceGamepadIndex() == sourceGamepadIndex && gamepadInputPumps_[i].GetSourceAxis() == sourceAxis && gamepadInputPumps_[i].GetVirtualAxis().GetName() == axisName)
				return (int)i;
		}

		return -1;
	}

	int ObjectInput::GetVirtualPumpIndex(int sourceID, const std::string& axisName) const
	{
		for (size_t i = 0; i < virtualInputPumps_.size(); i++)
		{
			if (virtualInputPumps_[i].GetSourceID() == sourceID && virtualInputPumps_[i].GetVirtualAxis().GetName() == axisName)
				return (int)i;
		}

		return -1;
	}


	void ObjectInput::SyncPreviousInput()
	{
		for (size_t i = 0; i < virtualAxes_.size(); i++)
			virtualAxes_[i].BeginUpdate();
	}

	void ObjectInput::ResetAxisState()
	{
		for (size_t i = 0; i < virtualAxes_.size(); i++)
			virtualAxes_[i].SetAxisState(0.0f);
	}

	void ObjectInput::PumpInput()
	{
		for (size_t i = 0; i < keyboardInputPumps_.size(); i++)
			keyboardInputPumps_[i].Pump();

		for (size_t i = 0; i < mouseInputPumps_.size(); i++)
			mouseInputPumps_[i].Pump();

		for (size_t i = 0; i < gamepadInputPumps_.size(); i++)
			gamepadInputPumps_[i].Pump();

		for (size_t i = 0; i < virtualInputPumps_.size(); i++)
			virtualInputPumps_[i].Pump();
	}

	void ObjectInput::RelinkPumpsToAxes()
	{
		for (size_t i = 0; i < keyboardInputPumps_.size(); i++)
			keyboardInputPumps_[i].SetVirtualAxis(virtualAxes_[keyboardInputPumps_[i].GetCachedAxisID()]);

		for (size_t i = 0; i < mouseInputPumps_.size(); i++)
			mouseInputPumps_[i].SetVirtualAxis(virtualAxes_[mouseInputPumps_[i].GetCachedAxisID()]);

		for (size_t i = 0; i < gamepadInputPumps_.size(); i++)
			gamepadInputPumps_[i].SetVirtualAxis(virtualAxes_[gamepadInputPumps_[i].GetCachedAxisID()]);

		for (size_t i = 0; i < virtualInputPumps_.size(); i++)
			virtualInputPumps_[i].SetVirtualAxis(virtualAxes_[virtualInputPumps_[i].GetCachedAxisID()]);
	}

}