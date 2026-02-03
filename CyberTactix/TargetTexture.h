#pragma once

#include "Texture.h"

namespace pix
{
	// TargetTexture is a specialized RGBA8888 texture with SDL_TEXTUREACCESS_TARGET access.
	// 
	// Philosophy:
	// TargetTexture is a specialized texture that is treated as an additional frame buffer for rendering. 
	// It can be reallocated on demand with flexible size to provide a custom internal rendering resolution. 
	class TargetTexture : public Texture
	{
	public:
		TargetTexture() = default;
		TargetTexture(int width, int height);
		virtual ~TargetTexture() = default;

		// Realloc() recreates a resized texture annd preserves the previous blend state.
        // If called on an uninitialized texture, default SDL state is applied (SDL_BLENDMODE_BLEND, RGBA = 255,255,255,255).
		// Returns true if the texture is recreated successfully, false otherwise.
		bool Realloc(int width, int height);
	};

}