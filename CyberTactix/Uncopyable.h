#pragma once

namespace pix
{

	// Philosophy:
	// Derived class objects of this (effectively abstract) class cannot be copied
	class Uncopyable
	{
	public:

		// Prevent copying by delete (alternatively by being private without definition)
		Uncopyable(const Uncopyable&) = delete;                    
		Uncopyable& operator= (const Uncopyable&) = delete;

	protected:

		// Allow construction/destruction of derived classes only
		Uncopyable()  = default;

		// Not supposed to be handled by a base pointer -> no need for virtual
		~Uncopyable()  = default; 
	};

}