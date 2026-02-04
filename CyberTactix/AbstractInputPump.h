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

		VirtualAxis(const std::string& name, float deadZone = 0.001f) : //deadZone is not negative
			axisState_(0.0f),
			prevAxisState_(0.0f),
			name_(name)
		{
			SetDeadZone(deadZone);
		}

		~VirtualAxis() = default; 

		// Syncs previous state with the current one.
		// Must be called exactly once per update loop iteration, at its beginning before pumping input.
		void BeginUpdate()
		{
			prevAxisState_ = axisState_;
		}

		void SetAxisState(float value) 
		{
			axisState_ = std::abs(value) > deadZone_ ? value : 0.0f;

			if (axisState_ > 1.0f) axisState_ = 1.0f;
			else if (axisState_ < -1.0f) axisState_ = -1.0f;
		}

		float GetAxisState() const
		{
			return axisState_;
		}

		void SetDeadZone(float value) 
		{
			deadZone_ = value;
			if (deadZone_ > 1.0f) deadZone_ = 1.0f;
			else if (deadZone_ < 0.0f) deadZone_ = 0.0f;
		}

		float GetDeadZone() const 
		{
			return deadZone_;
		}

		// Resets the previous and current axis state to zero (dead zone remains unchanged)
		void ResetState() 
		{
			axisState_ = 0.0f;
			prevAxisState_ = 0.0f;
		}

		bool IsPositive() const { return axisState_ > 0.0f; }
		bool IsNowPositive() const { return prevAxisState_ <= 0.0f && axisState_ > 0.0f; }
		bool IsNowZeroFromPositive() const { return prevAxisState_ > 0.0f && axisState_ == 0.0f; }
		bool IsNegative() const { return axisState_ < 0.0f; }
		bool IsNowNegative() const { return prevAxisState_ >= 0.0f && axisState_ < 0.0f; }
		bool IsNowZeroFromNegative() const { return prevAxisState_ < 0.0f && axisState_ == 0.0f; }
		bool IsNowZero() const { return prevAxisState_ != 0.0f && axisState_ == 0.0f; }


		const std::string& GetName() const
		{
			return name_;
		}

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

		using PumpFunction = float(*) (float sourceState, float axisState);

		bool Enabled;

		AbstractInputPump(VirtualAxis& virtualAxis, PumpFunction pumpFunction = nullptr):
			Enabled(true),
			pumpFunction_(AbsMax), // AbsMax() takes the absolute value of both inputs, and returns the biggest one
			virtualAxis_(&virtualAxis)
		{
			if (pumpFunction != nullptr) pumpFunction_ = pumpFunction;
		}

		virtual ~AbstractInputPump() = default;

		void PumpInput() 
		{
			if (Enabled)
				virtualAxis_->SetAxisState(pumpFunction_(GetSourceState(), virtualAxis_->GetAxisState()));
		}

		virtual float GetSourceState() const = 0;  //the input source is what differentiates the pump types 


		PumpFunction GetPumpFunction() const 
		{
			return pumpFunction_;
		}

		VirtualAxis* GetVirtualAxis() const 
		{
			return virtualAxis_;
		}

		void SetVirtualAxis(VirtualAxis& virtualAxis)
		{
			virtualAxis_ = &virtualAxis;
		}

	private:

		//Note: If a virtual axis has multiple input sources, the AbsMax() pump function is typically suited to set the input properly.

		PumpFunction pumpFunction_;
		VirtualAxis* virtualAxis_;

	};


}