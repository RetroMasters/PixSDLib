#pragma once

#include <SDL.h>
#include "Uncopyable.h"

namespace pix
{
	// Texture is a resource-managing abstract base class for SDL_Texture. It provides common properties and functionality for all concrete texture types.
	//
	// Technical note:
	// SDL_Textures are GPU resources that know their internal pixel format, and it may differ from the format used to create or load them. 
	// SDL guarantees correct blending and rendering across textures regardless of their origin since it creates SDL_Textures in a format that the GPU accepts.
	// 
	// Philosophy:
	// Conceptually, a texture is just raw pixel data. However, in SDL2, blending state is bound to the texture
	// rather than the renderer. Texture follows this model to avoid unnecessary abstraction friction.
	// Texture uniquely owns its SDL_Texture, but can expose it to interoperate with the C API.
	// Since multiple texture types share common state and behavior, Texture serves as a common base class for concrete texture types.
	// 
	// Note: On failure, getters tend to return semantically invalid values to make misuse visible.
	class Texture : private Uncopyable        
	{
	public:

		// RAII: Destroys SDL_Texture
		virtual ~Texture(); // virtual: Ensure proper destruction from a base class pointer

		void SetBlendMode(SDL_BlendMode blendMode); 

		// Modulates the color channels for a blending operation
		void SetColorMod(Uint8 r, Uint8 g, Uint8 b); 

		// Modulates the alpha channel for a blending operation
		void SetAlphaMod(Uint8 alpha);

		// Modulates all channels for a blending operation
		void SetRGBAMod(Uint8 r, Uint8 g, Uint8 b, Uint8 alpha); // Convenience function; avoids multiple SDL calls



		// Returns SDL_BLENDMODE_INVALID if the texture is not initialized
		SDL_BlendMode GetBlendMode() const;

		// rgb not modified if the texture is not initialized
		void GetRGBMod(Uint8& r, Uint8& g, Uint8& b) const;

		// Returns 0 if the texture is not initialized
		Uint8 GetAlphaMod() const;

		// rgba not modified if the texture is not initialized
		void GetRGBAMod(Uint8& r, Uint8& g, Uint8& b, Uint8& a) const; // Convenience function; avoids multiple SDL calls

		// width and height not modified if texture is not initialized
		void GetSize(int& width, int& height) const;

		// Ownership remains with Texture; destroying the exposed SDL_Texture results in undefined behavior.
		SDL_Texture* GetSDLTexture() const; // Get the resource for the C API

	protected:

		// Sets sdlTexture_ to nullptr
		Texture(); // Allow construction of derived classes only

		// Destroys SDL_Texture and sets sdlTexture_ to nullptr.
		// The command is ignored if sdlTexture_ is already a nullptr.
		void DeleteSDLTexture(); 

		SDL_Texture* sdlTexture_;
	};

}