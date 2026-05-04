#include "StreamingTexture.h"
#include "Renderer.h"
#include "ErrorLogger.h"

namespace pix
{

	StreamingTexture::StreamingTexture(int width, int height)
	{
		Realloc(width, height);
	}

	bool StreamingTexture::Realloc(int width, int height)
	{
		if (isLocked_)
		{
			ErrorLogger::Get().LogError("StreamingTexture::Realloc() failure", "Texture must not be reallocated while locked!");
			return false;
		}

		if (width <= 0 || height <= 0)
		{
			ErrorLogger::Get().LogError("StreamingTexture::Realloc() failure", "width and height must be positive!");
			return false;
		}

		SDL_BlendMode blendMode = SDL_BLENDMODE_BLEND;
		Uint8 r = 255, g = 255, b = 255, a = 255;

		// Cache blend state to restore it on the new texture
		if (sdlTexture_)
		{
			blendMode = GetBlendMode();
			GetRGBAMod(r, g, b, a);
		}

		SDL_Texture* newTexture = SDL_CreateTexture(Renderer::Get().GetSDLRenderer(), SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, width, height);
		if (!newTexture)
		{
			ErrorLogger::Get().LogSDLError("StreamingTexture::Realloc() - SDL_CreateTexture() failure");
			return false;
		}
	
		DeleteSDLTexture();
		sdlTexture_ = newTexture;
		
		// Restore blend state
		SetBlendMode(blendMode);
		SetRGBAMod(r, g, b, a);

		return true;
	}

	bool StreamingTexture::Lock(void** pixels, int* pitch)
	{
		if (isLocked_)
		{
			ErrorLogger::Get().LogError("StreamingTexture::Lock() failure", "Texture is already locked!");
			return false;
		}

		if (!sdlTexture_)
		{
			ErrorLogger::Get().LogError("StreamingTexture::Lock() failure", "Texture is not initialized!");
			return false;
		}

		if (!pixels || !pitch)
		{
			ErrorLogger::Get().LogError("StreamingTexture::Lock() failure", "pixels or pitch is nullptr!");
			return false;
		}

		if (SDL_LockTexture(sdlTexture_, nullptr, pixels, pitch) != 0)
		{
			ErrorLogger::Get().LogSDLError("StreamingTexture::Lock() - SDL_LockTexture() failure");
			return false;
		}

		isLocked_ = true;

		return true;
	}

	void StreamingTexture::Unlock()
	{
		if (!isLocked_) return;

		if (!sdlTexture_)
		{
			ErrorLogger::Get().LogError("StreamingTexture::Unlock() failure", "Texture is not initialized!");
			isLocked_ = false;
			return;
		}

		SDL_UnlockTexture(sdlTexture_);

		isLocked_ = false;
	}

}