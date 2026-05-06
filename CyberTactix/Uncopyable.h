#pragma once

namespace pix
{
	// Philosophy:
	// Non-instantiable base class that prevents derived classes from being copied
	class Uncopyable
	{
	public:

		// Prevent copying (alternatively by being private without definition)
		Uncopyable(const Uncopyable&) = delete;                    
		Uncopyable& operator= (const Uncopyable&) = delete;

	protected:

		// Allow construction/destruction through derived classes only.
		Uncopyable() = default;

		// Not intended to be handled through a base pointer -> no need for virtual destructor
		~Uncopyable() = default; 
	};
}