#pragma once

#include "Texture.h"
#include <string>

namespace pix
{
	// ImageTexture is a specialized texture class for loading and storing static image textures.
	//
	// Philosophy:
	// ImageTexture is typically used for sprites and other static visual assets whose texture is not modified at runtime.
	class ImageTexture : public Texture
	{
	public:
		ImageTexture() = default;
		explicit ImageTexture(const std::string& imagePath);
	     ~ImageTexture() override = default;

		// Reload() reloads a new image and preserves the previous blend state.
		// If called on an uninitialized texture, default SDL state is applied (SDL_BLENDMODE_BLEND, RGBA = 255, 255, 255, 255).
		// Returns true if the texture is reloaded successfully, false otherwise.
		bool Reload(const std::string& imagePath);
	};
}