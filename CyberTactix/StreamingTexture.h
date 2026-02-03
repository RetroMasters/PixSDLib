#pragma once

#include "Texture.h"

namespace pix
{
	// StreamingTexture is a specialized RGBA8888 texture that allows direct CPU access to pixel data.
	//
	// Technical note:
	// Streaming textures are backed by GPU resources but expose a CPU-writeable interface via SDL_LockTexture().
	// SDL handles synchronization and format conversion internally when the texture is updated.
	//
	// Philosophy:
	// StreamingTexture is intended for dynamic, frequently updated pixel data such as procedural textures and software-rendered games.
	// Unlike TargetTexture, which is updated via GPU operations, StreamingTexture explicitly supports CPU-side pixel manipulation.
	class StreamingTexture : public Texture
	{
	public:

		StreamingTexture() = default;
		StreamingTexture(int width, int height);
		virtual ~StreamingTexture() = default;

		// Realloc() recreates a resized texture annd preserves the previous blend state.
		// If called on an uninitialized texture, default SDL state is applied (SDL_BLENDMODE_BLEND, RGBA = 255,255,255,255).
		// Returns true if the texture is recreated successfully, false otherwise.
		bool Realloc(int width, int height);

		// Locks the texture for direct pixel access.
		// On success, pixels points to the pixel buffer and pitch specifies the number of bytes per row.
		// The texture must be unlocked to be updated.
		// As an optimization, locking does not necessarily preserve the old texture data. This is a write-only operation.
		// Returns true on success, false otherwise.
		bool Lock(void** pixels, int* pitch);

		// Updates the texture on the GPU after pixel modification.
		// Calling Unlock() without a matching Lock() is a no-op.
		void Unlock();
	};
}


// Example usage
/*
if (streamingTexture.Lock(&pixels, &pitch))
{
	Uint32* row = static_cast<Uint32*>(pixels);

	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			row[x] = 0xFF00FFFF; // RGBA8888
		}
		row = reinterpret_cast<Uint32*>(
			reinterpret_cast<Uint8*>(row) + pitch
		);
	}

	streamingTexture.Unlock();
}
*/
