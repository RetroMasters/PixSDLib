#pragma once

#include "Texture.h"
#include <string>
#include <SDL_ttf.h>

namespace pix
{
	// TextTexture is a specialized texture class for rendering and storing text in textures.
	//
	// Philosophy:
	// TextTexture is typically used like a sprite to display static text. 
	// By reloading the texture, text can also be updated at runtime.
	// This is recommended only in moderate amounts for performance reasons, such as updating a few score numbers.
	// 
	// Note: TextTexture does not take ownership of the TTF_Font.
	class TextTexture : public Texture
	{
	public:
		TextTexture() = default;
		TextTexture(const std::string& text, TTF_Font* font, bool renderBlended = true, SDL_Color color = { 255, 255, 255, 255 });
		~TextTexture() override = default;

		// Reload() creates a new texture from the provided text and preserves the previous blend state.
		// If called on an uninitialized texture, default SDL state is applied (SDL_BLENDMODE_BLEND, RGBA = 255, 255, 255, 255).
		//
		// There are two rendering modes:
		//   renderBlended = true  -> smooth, anti-aliased fonts with variable scaling;
		//                            suitable for modern UI.
		//   renderBlended = false -> no anti-aliasing, sharp edges;
		//                            suitable for pixel-art rendering.
		//
		// Returns true if the texture is reloaded successfully, false otherwise.
	    bool Reload(const std::string& text, TTF_Font* font, bool renderBlended = true, SDL_Color color = { 255, 255, 255, 255 });

	};

}