#include "ImageTexture.h"
#include <SDL_image.h>
#include "Renderer.h"
#include "ErrorLogger.h"

namespace pix
{
	ImageTexture::ImageTexture(const std::string& imagePath)
	{
	   Reload(imagePath);
	}

	bool ImageTexture::Reload(const std::string& imagePath) 
	{
		SDL_BlendMode blendMode = SDL_BLENDMODE_BLEND;
		Uint8 r = 255, g = 255, b = 255, a = 255;

		// Cache blend state so it can be restored on the new texture
		if (sdlTexture_)
		{
			blendMode = GetBlendMode();
			GetRGBAMod(r, g, b, a);
		}

		SDL_Texture* newTexture = IMG_LoadTexture(Renderer::Get().GetSDLRenderer(), imagePath.c_str());

		if (!newTexture)
		{
			ErrorLogger::Get().LogSDLError("ImageTexture::Reload() - IMG_LoadTexture() failure");
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