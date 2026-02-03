#include "TargetTexture.h"
#include "ErrorLogger.h"
#include "Renderer.h"

namespace pix
{

	TargetTexture::TargetTexture(int width, int height) : Texture()
	{
	   Realloc(width, height);
	}

	bool TargetTexture::Realloc(int width, int height) 
	{
		SDL_BlendMode blendMode = SDL_BLENDMODE_BLEND;
		Uint8 r = 255; Uint8 g = 255; Uint8 b = 255; Uint8 a = 255;

		// Cache blend state to restore it in the new texture
		if (sdlTexture_)
		{
			blendMode = GetBlendMode();
			GetRGBAMod(r, g, b, a);
		}

		SDL_Texture* newTexture = SDL_CreateTexture(Renderer::Get().GetSDLRenderer(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height);
		if (!newTexture)
		{
			ErrorLogger::Get().LogSDLError("TargetTexture::Realloc() - SDL_CreateTexture() failure");
			return false;
		}

		DeleteSDLTexture();
		sdlTexture_ = newTexture;
		
		// Restore blend state
		SetBlendMode(blendMode); 
		SetRGBAMod(r,g,b,a);

		return true;
	}

}
