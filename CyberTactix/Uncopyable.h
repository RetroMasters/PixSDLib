#pragma once

namespace pix
{
	/// <summary>
	/// Derived class objects of this abstract class cannot be copied.
	/// </summary>
	class Uncopyable
	{
	public:
		Uncopyable(const Uncopyable&) = delete;                     // Prevent copying by delete (alternatively by being private without definition)
		Uncopyable& operator= (const Uncopyable&) = delete;
		//Uncopyable(Uncopyable&&) = delete;
		//Uncopyable& operator= (Uncopyable&&) = delete;

	protected:                    // abstract: allow construction/destruction of derived classes only
		Uncopyable()  = default;
		~Uncopyable()  = default;  // Not supposed to be handled by a base pointer -> no need for virtual
	};

}