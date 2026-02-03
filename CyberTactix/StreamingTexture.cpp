#include "StreamingTexture.h"
#include "Renderer.h"
#include "ErrorLogger.h"

namespace pix
{

	StreamingTexture::StreamingTexture(int width, int height) : Texture()
	{
		Realloc(width, height);
	}

	bool StreamingTexture::Realloc(int width, int height)
	{
		SDL_BlendMode blendMode = SDL_BLENDMODE_BLEND;
		Uint8 r = 255; Uint8 g = 255; Uint8 b = 255; Uint8 a = 255;

		// Cache blend state to restore it in the new texture
		if (sdlTexture_)
		{
			blendMode = GetBlendMode();
			GetRGBAMod(r, g, b, a);
		}

		SDL_Texture* newTexture = SDL_CreateTexture(Renderer::Get().GetSDLRenderer(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, width, height);
		if (newTexture == nullptr)
		{
			ErrorLogger::Get().LogSDLError("StreamingTexture::Realloc() failure");
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
		if (!sdlTexture_)
		{
			ErrorLogger::Get().LogError("StreamingTexture::Lock() failure", "Texture is not initialized!");
			return false;
		}

		if (!pixels || !pitch)
		{
			ErrorLogger::Get().LogError("StreamingTexture::Lock() - nullptr failure", "pixels or pitch is a nullptr!");
			return false;
		}

		if (SDL_LockTexture(sdlTexture_, nullptr, pixels, pitch) != 0)
		{
			ErrorLogger::Get().LogSDLError("StreamingTexture::Lock - SDL_LockTexture() failure");
			return false;
		}

		return true;
	}

	void StreamingTexture::Unlock()
	{
		if (!sdlTexture_)
		{
			ErrorLogger::Get().LogError("StreamingTexture::Unlock() failure", "Texture is not initialized!");
			return;
		}

		SDL_UnlockTexture(sdlTexture_);
	}

}