#pragma once

#include<string>
#include"PixMath.h"

namespace pix
{
	// A virtual input axis that stores values in the range [-1,1].
	//
	// Philosophy:
	// A virtual axis is a system independent virtual "float button" that can be pushed in the range [-1,1].
	// This button can be set by input systems and then be queried for its state. 
	class VirtualAxis
	{
	public:

		VirtualAxis(const std::string& name, float deadZone = 0.001f); //deadZone is not negative


		~VirtualAxis() = default; 

		// Syncs previous state with the current one.
		// Must be called exactly once per update loop iteration, at its beginning before pumping input.
		void BeginUpdate();

		void SetAxisState(float value);

		float GetAxisState() const;

		void SetDeadZone(float value);

		float GetDeadZone() const;

		// Resets the previous and current axis state to zero (dead zone remains unchanged)
		void ResetState();

		bool IsPositive() const;
		bool IsNowPositive() const;
		bool IsNowZeroFromPositive() const;
		bool IsNegative() const;
		bool IsNowNegative() const;
		bool IsNowZeroFromNegative() const;
		bool IsNowZero() const;


		const std::string& GetName() const;

	protected:

		float axisState_;
		float prevAxisState_;
		std::string name_;

	private:

		float deadZone_;
	};


	// AbstractInputPump is the abstract base class for pumps transferring specific source-input, like a button press on gamepad, to a virtual axis. 
	// The transfer is handled by a pump function that takes the current source and axis state as input.
	// The concrete InputPump types only differ in the source of input. Thus they only have to implement GetSourceValue().
	// 
	// Philosophy:
	// An input pump is the fundamental building block for streaming input. An input pump does not own the virtual axis, it just connects exactly 
	// one source of input to exactly one virtual axis. The virtual axis must outlive the pump. By plugging input pumps in between the sources of input
	// and the virtual axes a full n:n relationship (an input can connect to all axes, and an axis can be connected by all inputs) in connectivity can naturally be achieved.  
	class AbstractInputPump
	{

	public:

		//typedef float(*PumpFunction) (float sourceValue, float axisValue); // A 2D float function taking the current source input and axis value to determine the resulting axis value.
		//Note: If a virtual axis has multiple input sources, the AbsMax() pump function is typically suited to set the input properly.
		using PumpFunction = float(*) (float sourceState, float axisState);

		bool Enabled;

		// If pumpFuntion is nullptr, then AbsMax() is used as default: takes the absolute value of both inputs, and returns the biggest one
		AbstractInputPump(VirtualAxis& virtualAxis, PumpFunction pumpFunction = nullptr);

		virtual ~AbstractInputPump() = default;

		void PumpInput();

		void SetVirtualAxis(VirtualAxis& virtualAxis);



		virtual float GetSourceState() const = 0;  // The input source is what differentiates the pump types 

		PumpFunction GetPumpFunction() const;

		VirtualAxis* GetVirtualAxis() const;

	private:

		PumpFunction pumpFunction_;
		VirtualAxis* virtualAxis_;

	};


}