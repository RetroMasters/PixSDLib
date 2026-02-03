#include "Texture.h"
#include "ErrorLogger.h"

namespace pix
{
	Texture::~Texture()
	{
		DeleteSDLTexture();
	}



	void Texture::SetBlendMode(SDL_BlendMode blendMode) 
	{
		if (!sdlTexture_)
		{
			ErrorLogger::Get().LogError("Texture::SetBlendMode() failure", "sdlTexture_ is nullptr!");
			return;
		}

		if (SDL_SetTextureBlendMode(sdlTexture_, blendMode) != 0)
			ErrorLogger::Get().LogSDLError("Texture::SetBlendMode() - SDL_SetTextureBlendMode() failure");
	}

	void Texture::SetColorMod(Uint8 r, Uint8 g, Uint8 b)
	{
		if (!sdlTexture_)
		{
			ErrorLogger::Get().LogError("Texture::SetColorMod() failure", "sdlTexture_ is nullptr!");
			return;
		}

		if (SDL_SetTextureColorMod(sdlTexture_, r, g, b) != 0)
			ErrorLogger::Get().LogSDLError("Texture::SetColorMod() - SDL_SetTextureColorMod() failure");
	}

	void Texture::SetAlphaMod(Uint8 alpha)
	{
		if (!sdlTexture_)
		{
			ErrorLogger::Get().LogError("Texture::SetAlphaMod() failure", "sdlTexture_ is nullptr!");
			return;
		}

		if (SDL_SetTextureAlphaMod(sdlTexture_, alpha) != 0)
			ErrorLogger::Get().LogSDLError("Texture::SetAlphaMod() - SDL_SetTextureAlphaMod() failure");
	}

	void Texture::SetRGBAMod(Uint8 r, Uint8 g, Uint8 b, Uint8 a)
	{
		if (!sdlTexture_)
		{
			ErrorLogger::Get().LogError("Texture::SetRGBAMod() failure", "sdlTexture_ is nullptr!");
			return;
		}

		if (SDL_SetTextureColorMod(sdlTexture_, r, g, b) != 0)
			ErrorLogger::Get().LogSDLError("Texture::SetRGBAMod() - SDL_SetTextureColorMod() failure");

		if (SDL_SetTextureAlphaMod(sdlTexture_, a) != 0)
			ErrorLogger::Get().LogSDLError("Texture::SetRGBAMod() - SDL_SetTextureAlphaMod() failure");
	}



	SDL_BlendMode Texture::GetBlendMode() const 
	{
		if (!sdlTexture_)
		{
			ErrorLogger::Get().LogError("Texture::GetBlendMode() failure", "sdlTexture_ is nullptr!");
			return  SDL_BLENDMODE_INVALID;
		}

		SDL_BlendMode blendMode = SDL_BLENDMODE_INVALID;
		if (SDL_GetTextureBlendMode(sdlTexture_, &blendMode) != 0)
			ErrorLogger::Get().LogSDLError("Texture::GetBlendMode() - SDL_GetTextureBlendMode() failure");

		return blendMode;
	}

	void Texture::GetRGBMod(Uint8& r, Uint8& g, Uint8& b) const
	{
		if (!sdlTexture_)
		{
			ErrorLogger::Get().LogError("Texture::GetColorMod() failure", "sdlTexture_ is nullptr!");
			return;
		}

		if (SDL_GetTextureColorMod(sdlTexture_, &r, &g, &b) != 0)
			ErrorLogger::Get().LogSDLError("Texture::GetColorMod() - SDL_GetTextureColorMod() failure");
	}

	Uint8 Texture::GetAlphaMod() const 
	{
		if (!sdlTexture_)
		{
			ErrorLogger::Get().LogError("Texture::GetAlphaMod() failure", "sdlTexture_ is nullptr!");
			return 0;
		}

		Uint8 alphaMod = 0;
		if (SDL_GetTextureAlphaMod(sdlTexture_, &alphaMod) != 0)
			ErrorLogger::Get().LogSDLError("Texture::GetAlphaMod() - SDL_GetTextureAlphaMod() failure");

		return alphaMod;
	}

	void Texture::GetRGBAMod(Uint8& r, Uint8& g, Uint8& b, Uint8& a) const
	{
		if (!sdlTexture_)
		{
			ErrorLogger::Get().LogError("Texture::GetRGBAMod() failure", "sdlTexture_ is nullptr!");
			return;
		}

		if (SDL_GetTextureColorMod(sdlTexture_, &r, &g, &b) != 0)
			ErrorLogger::Get().LogSDLError("Texture::GetRGBAMod() - SDL_GetTextureColorMod() failure");

		if (SDL_GetTextureAlphaMod(sdlTexture_, &a) != 0)
			ErrorLogger::Get().LogSDLError("Texture::GetRGBAMod() - SDL_GetTextureAlphaMod() failure");
	}


	void Texture::GetSize(int& width, int& height) const
	{
		if (!sdlTexture_)
		{
			ErrorLogger::Get().LogError("Texture::GetSize() failure", "sdlTexture_ is nullptr!");
			return;
		}

		if (SDL_QueryTexture(sdlTexture_, nullptr, nullptr, &width, &height) != 0)
			ErrorLogger::Get().LogSDLError("Texture::GetSize() - SDL_QueryTexture() failure");
	}

	SDL_Texture* Texture::GetSDLTexture() const
	{
		return sdlTexture_;
	}



	Texture::Texture():
		sdlTexture_(nullptr)
	{
	}

	void Texture::DeleteSDLTexture()
	{
		if (sdlTexture_)
		{
			SDL_DestroyTexture(sdlTexture_);
			sdlTexture_ = nullptr;
		}
	}

}






