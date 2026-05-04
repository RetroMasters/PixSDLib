#pragma once

#include "Texture.h"

namespace pix
{
	// TargetTexture is a texture with SDL_TEXTUREACCESS_TARGET access and RGBA32 format.
    // It is used as an additional render target and can be reallocated on demand to a new size.
	// 
    // Philosophy:
    // TargetTexture is used as an additional framebuffer for rendering.
	class TargetTexture : public Texture
	{
	public:
		TargetTexture() = default;
		TargetTexture(int width, int height);
		~TargetTexture() override = default;

		// Realloc() recreates the texture at a new size and preserves the previous blend state.
        // If called on an uninitialized texture, default SDL state is applied (SDL_BLENDMODE_BLEND, RGBA = 255, 255, 255, 255).
		// Returns true if the texture is recreated successfully, false otherwise.
		bool Realloc(int width, int height);
	};

}