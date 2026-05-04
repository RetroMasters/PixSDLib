#include "TargetTexture.h"
#include "Renderer.h"
#include "ErrorLogger.h"

namespace pix
{

	TargetTexture::TargetTexture(int width, int height)
	{
	   Realloc(width, height);
	}

	bool TargetTexture::Realloc(int width, int height) 
	{
		if (width <= 0 || height <= 0)
		{
			ErrorLogger::Get().LogError("TargetTexture::Realloc() failure", "width and height must be positive!");
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

		SDL_Texture* newTexture = SDL_CreateTexture(Renderer::Get().GetSDLRenderer(), SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, width, height);
		if (!newTexture)
		{
			ErrorLogger::Get().LogSDLError("TargetTexture::Realloc() - SDL_CreateTexture() failure");
			return false;
		}

		DeleteSDLTexture();
		sdlTexture_ = newTexture;
		
		// Restore blend state
		SetBlendMode(blendMode); 
		SetRGBAMod(r, g, b, a);

		return true;
	}

}
