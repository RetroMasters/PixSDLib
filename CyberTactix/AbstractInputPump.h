#pragma once

#include <string>


namespace pix
{
	// A virtual input axis that stores values in the range [-1, 1].
	//
	// Philosophy:
	// VirtualAxis represents a system-independent virtual analog axis that can be pushed in the range [-1, 1].
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

		// Sets the current axis state within the clamped range [-1, 1]
		// Note that within the dead zone, the axis state is set to zero
		void SetAxisState(float state);

		// Sets the dead zone within the clamped range [0, 1]
		void SetDeadZone(float value);

		// Clears current and previous axis state. Deadzone, ID, and name remain unchanged.
		void ClearState();
		
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

		float axisState_ = 0.0f;
		float prevAxisState_ = 0.0f;
		float deadZone_ = 0.001f;

		std::string name_;
		int id_ = -1;
	};


	// AbstractInputPump is the abstract base class for pumps transferring specific source input, such as a button press on a gamepad, to a virtual axis.
    // The transfer is handled by a pump function that takes the current source state and axis state as input.
    // Concrete pump types differ only in the source input, so they only need to implement GetSourceState().
    // A valid axis that outlives the pump must be provided by its owner so that Pump() can operate.
    // The axis ID is cached primarily as an index-like identity for owners that store axes in vectors, for example to relink after reallocation.
    // The validity of the cached axis ID depends on the owner.
	// 
	// Philosophy:
    // An input pump is the fundamental building block for streaming input.
    // It does not own the virtual axis; it only connects exactly one source of input to exactly one virtual axis.
    // The virtual axis must outlive the pump.
    // By placing input pumps between input sources and virtual axes, a full n:n relationship can be achieved:
    // one input can connect to multiple axes, and one axis can receive input from multiple sources.
	class AbstractInputPump
	{

	public:

		// A float function taking the current source state and current axis state and returning the resulting axis state.
		// Note: If a virtual axis has multiple input sources, DefaultPumpFunction() is typically suited to set the input properly.
		using PumpFunction = float(*) (float sourceState, float axisState);

		//######################## INITIALIZATION ################################

		// virtualAxis must be valid. 
		// DefaultPumpFunction() is set if pumpFunction is nullptr.
		AbstractInputPump(VirtualAxis& virtualAxis, PumpFunction pumpFunction = nullptr);

		virtual ~AbstractInputPump() = default;

		//###################### FUNCTIONALITY #############################

		bool Enabled = true;

		// Pumps the current source state to the axis by using the pump function.
		// If Enabled is set to false, Pump() is a no-op. 
		void Pump();

		// Rebinds the pump to a different axis and updates the cached axis ID.
		// virtualAxis must be valid. 
		void SetVirtualAxis(VirtualAxis& virtualAxis);

		// If pumpFunction is nullptr, DefaultPumpFunction() is set.
		void SetPumpFunction(PumpFunction pumpFunction);

		virtual float GetSourceState() const = 0;  // The input source is what differentiates the pump types 

		//################################# GETTERS ##########################################

		PumpFunction GetPumpFunction() const;

		// Philosophy: Returns a non-const reference because the pump does not own the bound VirtualAxis.
		// Constness of the pump does not restrict mutation of externally owned objects.
		VirtualAxis& GetVirtualAxis() const; 

		int GetCachedAxisID() const;

	private:

		// Returns the value with the biggest magnitude, so the biggest contributor wins.
		// On equal magnitude, the source value wins (later pumps override earlier ones).
		static float DefaultPumpFunction(float sourceState, float axisState);

		PumpFunction pumpFunction_ = DefaultPumpFunction;
		VirtualAxis* virtualAxis_ = nullptr;
		int cachedAxisID_ = -1;
	};

}