#pragma once

#include "Texture.h"

namespace pix
{
	// StreamingTexture is a texture with SDL_TEXTUREACCESS_STREAMING access and RGBA32 format for direct CPU access to pixel data.
	// 
	// Technical note:
	// Streaming textures are backed by GPU resources but expose a CPU-writeable interface via SDL_LockTexture().
	// Locking is a write-only operation, and SDL applies the written pixel changes when the texture is unlocked.
	//
	// Philosophy:
	// StreamingTexture is intended for dynamic, frequently updated pixel data such as procedural textures and software-rendered games.
	// Unlike TargetTexture, which is updated via GPU operations, StreamingTexture explicitly supports CPU-side pixel manipulation.
	class StreamingTexture : public Texture
	{
	public:

		StreamingTexture() = default;
		StreamingTexture(int width, int height);
		~StreamingTexture() override = default;

		// Realloc() recreates a resized texture and preserves the previous blend state.
		// If called on an uninitialized texture, default SDL state is applied (SDL_BLENDMODE_BLEND, RGBA = 255, 255, 255, 255).
		// Realloc() fails if the texture is already locked.
		// Returns true if the texture is recreated successfully, false otherwise.
		bool Realloc(int width, int height);

		// Locks the texture for direct pixel access.
		// On success, *pixels points to the pixel buffer, and *pitch specifies the number of bytes per row.
		// The texture must be unlocked to apply the written pixel changes.
		// As an optimization, locking does not necessarily preserve the old texture data.
		// Lock() fails if the texture is already locked.
		// Returns true on success, false otherwise.
		bool Lock(void** pixels, int* pitch);

		// Applies the written pixel changes to the texture.
		// Calling Unlock() without a matching successful Lock() is a no-op.
		void Unlock();

	private:

		bool isLocked_ = false;
	};
}


// Example usage 1: Operate per color channel:
// Fills the texture with opaque cyan in RGBA byte order.
/*
void* pixels = nullptr;
int pitch = 0;

if (streamingTexture.Lock(&pixels, &pitch))
{
	Uint8* row = (Uint8*)pixels;

	for (int y = 0; y < height; ++y)
	{
		Uint8* pixel = row;

		for (int x = 0; x < width; ++x)
		{
			pixel[0] = 0x00; // R
			pixel[1] = 0xFF; // G
			pixel[2] = 0xFF; // B
			pixel[3] = 0xFF; // A

			pixel += 4;
		}

		row += pitch;
	}

	streamingTexture.Unlock();
}
*/


// Example usage 2: Operate per pixel:
// Fills the texture with opaque cyan in RGBA byte order.
/*
void* pixels = nullptr;
int pitch = 0;

if (streamingTexture.Lock(&pixels, &pitch))
{
	Uint8* rowBytes = (Uint8*)pixels;

	for (int y = 0; y < height; ++y)
	{
		SDL_Color* row = (SDL_Color*)rowBytes;

		for (int x = 0; x < width; ++x)
			row[x] = SDL_Color{ 0, 255, 255, 255 };

		rowBytes += pitch;
	}

	streamingTexture.Unlock();
}
*/