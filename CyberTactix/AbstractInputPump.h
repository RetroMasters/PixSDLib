#pragma once

#include<string>


namespace pix
{
	// A virtual input axis that stores values in the range [-1,1].
	//
	// Philosophy:
	// VirtualAxis represents a system independent virtual analog axis that can be pushed in the range [-1,1].
	// This virtual analog axis can be set by input systems and then be queried for its state.
	// It can also be used like a button that can be pressed.
	class VirtualAxis
	{
	public:

		//######################## INITIALIZATION ################################

		VirtualAxis(const std::string& name, int id, float deadZone = 0.001f); 

		~VirtualAxis() = default; 

		//############################ FUNCTIONALITY ###################################

		// Syncs previous axis state with the current one.
		// Must be called exactly once per update loop iteration, at its beginning before pumping input.
		// "Became" queries compare current state with the snapshot taken by BeginUpdate().
		void BeginUpdate();

		// Sets the current axis state within the range [-1,1]
		// Note that within the dead zone, the axis state is set to zero
		void SetAxisState(float state);

		// Sets the dead zone within the range [0,1]
		void SetDeadZone(float value);

		// Resets the previous and current axis state to zero 
		void Reset();
		
		//################################# GETTERS ##########################################
		
		// True if the axis is currently positive
		bool IsPositive() const;
		// True if the axis crossed from <= 0 to > 0 this update
		bool BecamePositive() const;
		// True if the axis crossed from > 0 to 0 this update
		bool BecameZeroFromPositive() const;
		// True if the axis is currently negative
		bool IsNegative() const;
		// True if the axis crossed from >= 0 to < 0 this update
		bool BecameNegative() const;
		// True if the axis crossed from < 0 to 0 this update
		bool BecameZeroFromNegative() const;
		// True if the axis crossed from != 0 to 0 this update
		bool BecameZero() const;
		// Returns the current internal axis state. Note that within the dead zone, the internal axis state is zero.
		float GetAxisState() const;
		// Returns the axis state with the dead zone removed and the remaining range rescaled to [-1, 1].
		float GetEffectiveAxisState() const;

		float GetDeadZone() const;

		const std::string& GetName() const;
		int GetID() const;

	private:

		float axisState_;
		float prevAxisState_;
		float deadZone_;

		std::string name_;
		int id_;
	};


	// AbstractInputPump is the abstract base class for pumps transferring specific source-input, like a button press on gamepad, to a virtual axis. 
	// The transfer is handled by a pump function that takes the current source and axis state as input.
	// The concrete InputPump types only differ in the source of input. Thus they only have to implement GetSourceState().
	// A valid axis that outlives the pump must be provided by its owner, so the Pump() method can operate. 
	// The axis ID is cached primarily as an index-like identity for owners that store axes in vectors (to relink after reallocation).
	// But validity of the ID depends on the owner. 
	// 
	// Philosophy:
	// An input pump is the fundamental building block for streaming input. An input pump does not own the virtual axis, it just connects exactly 
	// one source of input to exactly one virtual axis. The virtual axis must outlive the pump. By plugging input pumps in between the sources of input
	// and the virtual axes, a full n:n relationship (an input can connect to all axes, and an axis can be connected by all inputs) in connectivity can be achieved.  
	class AbstractInputPump
	{

	public:

	    // A 2D float function taking the current source state and axis state to determine the resulting axis state.
		// Note: If a virtual axis has multiple input sources, DefaultPumpFunction() is typically suited to set the input properly.
		using PumpFunction = float(*) (float sourceState, float axisState);

		//######################## INITIALIZATION ################################

		// virtualAxis must be valid. 
		// DefaultPumpFunction() is set if pumpFunction is nullptr.
		AbstractInputPump(VirtualAxis& virtualAxis, PumpFunction pumpFunction = nullptr);

		virtual ~AbstractInputPump() = default;

		//###################### FUNCTIONALITY #############################

		bool Enabled;

		// Pumps the current source state to the axis by using the pump function.
		// If Enabled is set to false, Pump() is no-op. 
		void Pump();

		// Rebinds the pump to a different axis and updates the cached axis ID.
		// virtualAxis must be valid. 
		void SetVirtualAxis(VirtualAxis& virtualAxis);

		// If pumpFunction is nullptr, DefaultPumpFunction() is set.
		void SetPumpFunction(PumpFunction pumpFunction);

		virtual float GetSourceState() const = 0;  // The input source is what differentiates the pump types 

		//################################# GETTERS ##########################################

		PumpFunction GetPumpFunction() const;

		VirtualAxis& GetVirtualAxis();

		const VirtualAxis& GetVirtualAxis() const;

		int GetCachedAxisID() const;

	private:

		// Returns the value with the biggest magnitude, so the biggest contributor wins.
		// On equal magnitude, the source value wins (later pumps override earlier ones).
		static float DefaultPumpFunction(float sourceState, float axisState);

		PumpFunction pumpFunction_;
		VirtualAxis* virtualAxis_;
		int cachedAxisID_;
	};

}