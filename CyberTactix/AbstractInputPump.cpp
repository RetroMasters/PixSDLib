#include "AbstractInputPump.h"
#include <cmath>

namespace pix
{
	
		VirtualAxis::VirtualAxis(const std::string& name, int id, float deadZone) :
			axisState_(0.0f),
			prevAxisState_(0.0f),
			name_(name),
			id_(id)
		{
			SetDeadZone(deadZone);
		}

		void VirtualAxis::BeginUpdate()
		{
			prevAxisState_ = axisState_;
		}
		
		void VirtualAxis::SetAxisState(float state)
		{
			if (state > 1.0f) state = 1.0f;
			else if (state < -1.0f) state = -1.0f;

			axisState_ = std::abs(state) > deadZone_ ? state : 0.0f;
		}

		float VirtualAxis::GetAxisState() const
		{
			return axisState_;
		}

		float VirtualAxis::GetEffectiveAxisState() const
		{
			// When deadZone_ == 1, axisState_ is 0, so division by zero cannot occur
			float effectiveRange = 1.0f - deadZone_;

			if (axisState_ > 0.0f)
				return (axisState_ - deadZone_) / effectiveRange;

			if (axisState_ < 0.0f)
				return (axisState_ + deadZone_) / effectiveRange;

			return 0.0f;
		}

		void VirtualAxis::SetDeadZone(float value)
		{
			if (value > 1.0f) value = 1.0f;
			else if (value < 0.0f) value = 0.0f;

			deadZone_ = value;
		}

		float VirtualAxis::GetDeadZone() const
		{
			return deadZone_;
		}

		void VirtualAxis::Reset()
		{
			axisState_ = 0.0f;
			prevAxisState_ = 0.0f;
		}

		bool VirtualAxis::IsPositive() const { return axisState_ > 0.0f; }
		bool VirtualAxis::BecamePositive() const { return prevAxisState_ <= 0.0f && axisState_ > 0.0f; }
		bool VirtualAxis::BecameZeroFromPositive() const { return prevAxisState_ > 0.0f && axisState_ == 0.0f; }
		bool VirtualAxis::IsNegative() const { return axisState_ < 0.0f; }
		bool VirtualAxis::BecameNegative() const { return prevAxisState_ >= 0.0f && axisState_ < 0.0f; }
		bool VirtualAxis::BecameZeroFromNegative() const { return prevAxisState_ < 0.0f && axisState_ == 0.0f; }
		bool VirtualAxis::BecameZero() const { return prevAxisState_ != 0.0f && axisState_ == 0.0f; }


		const std::string& VirtualAxis::GetName() const
		{
			return name_;
		}

		int VirtualAxis::GetID() const
		{
			return id_;
		}

		AbstractInputPump::AbstractInputPump(VirtualAxis& virtualAxis, PumpFunction pumpFunction) :
			Enabled(true),
			pumpFunction_(pumpFunction ? pumpFunction : DefaultPumpFunction), 
			virtualAxis_(&virtualAxis),
			cachedAxisID_(virtualAxis.GetID())
		{
		}

		void AbstractInputPump::Pump()
		{
			if (Enabled)
				virtualAxis_->SetAxisState(pumpFunction_(GetSourceState(), virtualAxis_->GetAxisState()));
		}

		void AbstractInputPump::SetVirtualAxis(VirtualAxis& virtualAxis)
		{
			virtualAxis_ = &virtualAxis;
			cachedAxisID_ = virtualAxis.GetID();
		}

		void AbstractInputPump::SetPumpFunction(PumpFunction pumpFunction)
		{
			pumpFunction_ = pumpFunction ? pumpFunction : DefaultPumpFunction;
		}

		AbstractInputPump::PumpFunction AbstractInputPump::GetPumpFunction() const
		{
			return pumpFunction_;
		}

		VirtualAxis* AbstractInputPump::GetVirtualAxis() const
		{
			return virtualAxis_;
		}

		int AbstractInputPump::GetCachedAxisID() const
		{
			return cachedAxisID_;
		}

		float AbstractInputPump::DefaultPumpFunction(float sourceState, float axisState)
		{
			return (std::abs(sourceState) < std::abs(axisState)) ? axisState : sourceState;
		}

}