#pragma once

#include<string>
#include "Texture.h"

namespace pix
{
	// ImageTexture is a specialized texture class for loading and storing static image-based textures.
	//
	// Philosophy:
	// The ImageTexture is typically used for sprites and static visual assets, as their texture is not modified at runtime.
	class ImageTexture : public Texture
	{
	public:
		ImageTexture() = default;
		explicit ImageTexture(const std::string& path);
		virtual ~ImageTexture() = default;

		// Reload() reloads a new image and preserves the previous blend state.
		// If called on an uninitialized texture, default SDL state is applied (SDL_BLENDMODE_BLEND, RGBA = 255,255,255,255).
		// Returns true if the texture is reloaded successfully, false otherwise.
		bool Reload(const std::string& path);
	};
}