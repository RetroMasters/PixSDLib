#include "AbstractInputPump.h"

namespace pix
{
	
		VirtualAxis::VirtualAxis(const std::string& name, float deadZone) : //deadZone is not negative
			axisState_(0.0f),
			prevAxisState_(0.0f),
			name_(name)
		{
			SetDeadZone(deadZone);
		}

		// Syncs previous state with the current one.
		// Must be called exactly once per update loop iteration, at its beginning before pumping input.
		void VirtualAxis::BeginUpdate()
		{
			prevAxisState_ = axisState_;
		}

		void VirtualAxis::SetAxisState(float value)
		{
			axisState_ = std::abs(value) > deadZone_ ? value : 0.0f;

			if (axisState_ > 1.0f) axisState_ = 1.0f;
			else if (axisState_ < -1.0f) axisState_ = -1.0f;
		}

		float VirtualAxis::GetAxisState() const
		{
			return axisState_;
		}

		void VirtualAxis::SetDeadZone(float value)
		{
			deadZone_ = value;
			if (deadZone_ > 1.0f) deadZone_ = 1.0f;
			else if (deadZone_ < 0.0f) deadZone_ = 0.0f;
		}

		float VirtualAxis::GetDeadZone() const
		{
			return deadZone_;
		}

		// Resets the previous and current axis state to zero (dead zone remains unchanged)
		void VirtualAxis::ResetState()
		{
			axisState_ = 0.0f;
			prevAxisState_ = 0.0f;
		}

		bool VirtualAxis::IsPositive() const { return axisState_ > 0.0f; }
		bool VirtualAxis::IsNowPositive() const { return prevAxisState_ <= 0.0f && axisState_ > 0.0f; }
		bool VirtualAxis::IsNowZeroFromPositive() const { return prevAxisState_ > 0.0f && axisState_ == 0.0f; }
		bool VirtualAxis::IsNegative() const { return axisState_ < 0.0f; }
		bool VirtualAxis::IsNowNegative() const { return prevAxisState_ >= 0.0f && axisState_ < 0.0f; }
		bool VirtualAxis::IsNowZeroFromNegative() const { return prevAxisState_ < 0.0f && axisState_ == 0.0f; }
		bool VirtualAxis::IsNowZero() const { return prevAxisState_ != 0.0f && axisState_ == 0.0f; }


		const std::string& VirtualAxis::GetName() const
		{
			return name_;
		}

		AbstractInputPump::AbstractInputPump(VirtualAxis& virtualAxis, PumpFunction pumpFunction) :
			Enabled(true),
			pumpFunction_(AbsMax), // AbsMax() takes the absolute value of both inputs, and returns the biggest one
			virtualAxis_(&virtualAxis)
		{
			if (pumpFunction != nullptr) pumpFunction_ = pumpFunction;
		}

		void AbstractInputPump::PumpInput()
		{
			if (Enabled)
				virtualAxis_->SetAxisState(pumpFunction_(GetSourceState(), virtualAxis_->GetAxisState()));
		}

		void AbstractInputPump::SetVirtualAxis(VirtualAxis& virtualAxis)
		{
			virtualAxis_ = &virtualAxis;
		}

		AbstractInputPump::PumpFunction AbstractInputPump::GetPumpFunction() const
		{
			return pumpFunction_;
		}

		VirtualAxis* AbstractInputPump::GetVirtualAxis() const
		{
			return virtualAxis_;
		}

		

}