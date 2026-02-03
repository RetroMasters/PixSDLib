#include "TextTexture.h"
#include "Renderer.h"
#include "ErrorLogger.h"

namespace pix
{

	TextTexture::TextTexture(const std::string& text, TTF_Font* font, bool renderBlended, SDL_Color color) : Texture()
	{
	    Reload(text, font, renderBlended, color);
	}


	bool TextTexture::Reload(const std::string& text, TTF_Font* font, bool renderBlended, SDL_Color color) 
	{
		if (font == nullptr)
		{
			ErrorLogger::Get().LogError("TextTexture::Reload() failure", "No TTF_Font provided!");
			return false;
		}

		SDL_BlendMode blendMode = SDL_BLENDMODE_BLEND;
		Uint8 r = 255; Uint8 g = 255; Uint8 b = 255; Uint8 a = 255;

		// Cache blend state to restore it in the new texture
		if (sdlTexture_)
		{
			blendMode = GetBlendMode();
			GetRGBAMod(r, g, b, a);
		}

		SDL_Surface* textSurface = nullptr;
		if(renderBlended)
		 textSurface = TTF_RenderUTF8_Blended(font, text.c_str(), color);
		else 
		 textSurface = TTF_RenderText_Solid(font, text.c_str(), color);

		if (!textSurface)
		{
			ErrorLogger::Get().LogSDLError("TextTexture::Reload() - TTF_RenderUTF8... failure");
			return false;
		}

		SDL_Texture* newTexture = SDL_CreateTextureFromSurface(Renderer::Get().GetSDLRenderer(), textSurface);

		SDL_FreeSurface(textSurface);

		if (!newTexture)
		{
			ErrorLogger::Get().LogSDLError("TextTexture::Reload() - SDL_CreateTextureFromSurface() failure");
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