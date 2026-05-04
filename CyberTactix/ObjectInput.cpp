#include "ObjectInput.h"

namespace pix
{
	// ######################################## INITIALIZATION ###################################################

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

	// ################################################################## CONFIGURE INPUT ####################################################

	
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
		
		// Preserve binding order because pump order may affect the resulting axis state
		keyboardInputPumps_.erase(keyboardInputPumps_.begin() + pumpIndex);

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

		// Preserve binding order because pump order may affect the resulting axis state
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
		
		// Preserve binding order because pump order may affect the resulting axis state
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

		// Preserve binding order because pump order may affect the resulting axis state
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

		// Preserve binding order because pump order may affect the resulting axis state
		virtualInputPumps_.erase(virtualInputPumps_.begin() + pumpIndex);

		return true;	
	}

	bool ObjectInput::SetDeadZone(const std::string& axisName, float value)
	{
		VirtualAxis* axis = GetAxis(axisName);
		if (!axis) return false;

		axis->SetDeadZone(value);
		return true;
	}

	// ################################################################## SET INPUT ####################################################


	void ObjectInput::Update()
	{
		SyncPreviousInput();

		ClearCurrentAxisState();

		PumpInput();

	}

	bool ObjectInput::SetVirtualSourceState(int sourceID, const std::string& axisName, float sourceState)
	{
		return SetVirtualSourceState(sourceID, GetAxisID(axisName), sourceState);
	}

	bool ObjectInput::SetVirtualSourceState(int sourceID, int axisID, float sourceState)
	{
		int pumpIndex = GetVirtualPumpIndex(sourceID, axisID);
		if (pumpIndex < 0) return false;

		virtualInputPumps_[pumpIndex].SetSourceState(sourceState);
		return true;
	}

	void ObjectInput::ClearAxisState()
	{
		int axisCount = virtualAxes_.size();

		for (int i = 0; i < axisCount; i++)
			virtualAxes_[i].ClearState();
	}

	void ObjectInput::ResetVirtualSourceState()
	{
		int pumpCount = virtualInputPumps_.size();

		for (int i = 0; i < pumpCount; i++)
			virtualInputPumps_[i].SetSourceState(0.0f);
	}


	// ################################################################## CHECK INPUT ####################################################


	bool ObjectInput::IsPositive(const std::string& axisName) const
	{
		return IsPositive(GetAxisID(axisName));
	}

	bool ObjectInput::BecamePositive(const std::string& axisName) const
	{
		return BecamePositive(GetAxisID(axisName));
	}

	bool ObjectInput::BecameZeroFromPositive(const std::string& axisName) const
	{
		return BecameZeroFromPositive(GetAxisID(axisName));
	}

	bool ObjectInput::IsNegative(const std::string& axisName) const
	{
		return IsNegative(GetAxisID(axisName));
	}

	bool ObjectInput::BecameNegative(const std::string& axisName) const
	{
		return BecameNegative(GetAxisID(axisName));
	}

	bool ObjectInput::BecameZeroFromNegative(const std::string& axisName) const
	{
		return BecameZeroFromNegative(GetAxisID(axisName));
	}

	bool ObjectInput::BecameZero(const std::string& axisName) const
	{
		return BecameZero(GetAxisID(axisName));
	}

	float ObjectInput::GetAxisState(const std::string& axisName) const
	{
		return GetAxisState(GetAxisID(axisName));
	}

	bool ObjectInput::IsPositive(int axisID) const
	{
		return IsValidAxisID(axisID) ? virtualAxes_[axisID].IsPositive() : false;
	}

	bool ObjectInput::BecamePositive(int axisID) const
	{
		return IsValidAxisID(axisID) ? virtualAxes_[axisID].BecamePositive() : false;
	}

	bool ObjectInput::BecameZeroFromPositive(int axisID) const
	{
		return IsValidAxisID(axisID) ? virtualAxes_[axisID].BecameZeroFromPositive() : false;
	}

	bool ObjectInput::IsNegative(int axisID) const
	{
		return IsValidAxisID(axisID) ? virtualAxes_[axisID].IsNegative() : false;
	}

	bool ObjectInput::BecameNegative(int axisID) const
	{
		return IsValidAxisID(axisID) ? virtualAxes_[axisID].BecameNegative() : false;
	}

	bool ObjectInput::BecameZeroFromNegative(int axisID) const
	{
		return IsValidAxisID(axisID) ? virtualAxes_[axisID].BecameZeroFromNegative() : false;
	}

	bool ObjectInput::BecameZero(int axisID) const
	{
		return IsValidAxisID(axisID) ? virtualAxes_[axisID].BecameZero() : false;
	}

	float ObjectInput::GetAxisState(int axisID) const
	{
		return IsValidAxisID(axisID) ? virtualAxes_[axisID].GetAxisState() : 0.0f;
	}

	int ObjectInput::GetAxisID(const std::string& axisName) const
	{
		const int axisCount = virtualAxes_.size();

		for (int i = 0; i < axisCount; i++)
		{
			if (virtualAxes_[i].GetName() == axisName)
				return i;
		}

		return -1;
	}
	
	std::string ObjectInput::GetAxisName(int axisID) const
	{
		if (!IsValidAxisID(axisID))
			return std::string();

		return virtualAxes_[axisID].GetName();
	}


	// ################################################################## PRIVATE ####################################################


	void ObjectInput::SyncPreviousInput()
	{
		const int axisCount = virtualAxes_.size();

		for (int i = 0; i < axisCount; i++)
			virtualAxes_[i].BeginUpdate();
	}

	void ObjectInput::ClearCurrentAxisState()
	{
		const int axisCount = virtualAxes_.size();

		for (int i = 0; i < axisCount; i++)
			virtualAxes_[i].SetAxisState(0.0f);
	}

	void ObjectInput::PumpInput()
	{
		int pumpCount = keyboardInputPumps_.size();

		for (int i = 0; i < pumpCount; i++)
			keyboardInputPumps_[i].Pump();

		pumpCount = mouseInputPumps_.size();

		for (int i = 0; i < pumpCount; i++)
			mouseInputPumps_[i].Pump();

		pumpCount = gamepadInputPumps_.size();

		for (int i = 0; i < pumpCount; i++)
			gamepadInputPumps_[i].Pump();

		pumpCount = virtualInputPumps_.size();

		for (int i = 0; i < pumpCount; i++)
			virtualInputPumps_[i].Pump();
	}

	void ObjectInput::RelinkPumpsToAxes()
	{
		int pumpCount = keyboardInputPumps_.size();

		for (int i = 0; i < pumpCount; i++)
			keyboardInputPumps_[i].SetVirtualAxis(virtualAxes_[keyboardInputPumps_[i].GetCachedAxisID()]);

		pumpCount = mouseInputPumps_.size();

		for (int i = 0; i < pumpCount; i++)
			mouseInputPumps_[i].SetVirtualAxis(virtualAxes_[mouseInputPumps_[i].GetCachedAxisID()]);

		pumpCount = gamepadInputPumps_.size();

		for (int i = 0; i < pumpCount; i++)
			gamepadInputPumps_[i].SetVirtualAxis(virtualAxes_[gamepadInputPumps_[i].GetCachedAxisID()]);

		pumpCount = virtualInputPumps_.size();

		for (int i = 0; i < pumpCount; i++)
			virtualInputPumps_[i].SetVirtualAxis(virtualAxes_[virtualInputPumps_[i].GetCachedAxisID()]);
	}

	VirtualAxis* ObjectInput::GetOrAddVirtualAxis(const std::string& axisName)
	{
		if (axisName.empty()) return nullptr;

		VirtualAxis* axis = GetAxis(axisName);

		if (!axis)
		{
			virtualAxes_.emplace_back(axisName, virtualAxes_.size());
			axis = &(virtualAxes_.back());

			RelinkPumpsToAxes(); // In case the axis vector store gets relocated, update the axis binding
		}

		return axis;
	}

	VirtualAxis* ObjectInput::GetAxis(const std::string& axisName)
	{
		const int axisCount = virtualAxes_.size();

		for (int i = 0; i < axisCount; i++)
		{
			if (virtualAxes_[i].GetName() == axisName)
				return &(virtualAxes_[i]);
		}

		return nullptr;
	}

	const VirtualAxis* ObjectInput::GetAxis(const std::string& axisName) const
	{
		const int axisCount = virtualAxes_.size();

		for (int i = 0; i < axisCount; i++)
		{
			if (virtualAxes_[i].GetName() == axisName)
				return &(virtualAxes_[i]);
		}

		return nullptr;
	} 
	
    int ObjectInput::GetKeyboardPumpIndex(SDL_Scancode sourceKey, const std::string& axisName) const
	{
		const int pumpCount = keyboardInputPumps_.size();

		for (int i = 0; i < pumpCount; i++)
		{
			if (keyboardInputPumps_[i].GetSourceKey() == sourceKey && keyboardInputPumps_[i].GetVirtualAxis().GetName() == axisName)
				return i;
		}

		return -1;
	}

	int ObjectInput::GetMousePumpIndex(MouseInput::Button sourceButton, const std::string& axisName) const
	{
		const int pumpCount = mouseInputPumps_.size();

		for (int i = 0; i < pumpCount; i++)
		{
			if (mouseInputPumps_[i].GetSourceButton() == sourceButton && mouseInputPumps_[i].GetVirtualAxis().GetName() == axisName)
				return i;
		}

		return -1;
	}

	int ObjectInput::GetGamepadPumpIndex(int sourceGamepadIndex, SDL_GameControllerButton sourceButton, const std::string& axisName) const
	{
		const int pumpCount = gamepadInputPumps_.size();

		for (int i = 0; i < pumpCount; i++)
		{
			if (gamepadInputPumps_[i].GetSourceGamepadIndex() == sourceGamepadIndex && gamepadInputPumps_[i].GetSourceButton() == sourceButton && gamepadInputPumps_[i].GetVirtualAxis().GetName() == axisName)
				return i;
		}

		return -1;
	}

	int ObjectInput::GetGamepadPumpIndex(int sourceGamepadIndex, SDL_GameControllerAxis sourceAxis, const std::string& axisName) const
	{
		const int pumpCount = gamepadInputPumps_.size();

		for (int i = 0; i < pumpCount; i++)
		{
			if (gamepadInputPumps_[i].GetSourceGamepadIndex() == sourceGamepadIndex && gamepadInputPumps_[i].GetSourceAxis() == sourceAxis && gamepadInputPumps_[i].GetVirtualAxis().GetName() == axisName)
				return i;
		}

		return -1;
	}

	int ObjectInput::GetVirtualPumpIndex(int sourceID, const std::string& axisName) const
	{
		const int pumpCount = virtualInputPumps_.size();

		for (int i = 0; i < pumpCount; i++)
		{
			if (virtualInputPumps_[i].GetSourceID() == sourceID && virtualInputPumps_[i].GetVirtualAxis().GetName() == axisName)
				return i;
		}

		return -1;
	}

	int ObjectInput::GetVirtualPumpIndex(int sourceID, int axisID) const
	{
		if (!IsValidAxisID(axisID)) return -1;

		int pumpCount = virtualInputPumps_.size();

		for (int i = 0; i < pumpCount; i++)
		{
			if (virtualInputPumps_[i].GetSourceID() == sourceID && virtualInputPumps_[i].GetCachedAxisID() == axisID)
				return i;
		}

		return -1;
	}

	bool ObjectInput::IsValidAxisID(int axisID) const
	{
		return axisID >= 0 && axisID < virtualAxes_.size();
	}

}