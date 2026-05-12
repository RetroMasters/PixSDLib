#include "TextureOps.h"
#include "Renderer.h"
#include "ErrorLogger.h"


namespace pix
{

	bool BlendTextureRegion(Texture& sourceTexture, const SDL_Rect* sourceRect, const Texture& modifierTexture, const SDL_Rect* modifierRect, TargetTexture& outputTexture)
	{
		return false; // TODO
	}

	bool BlendTextureRegion(Texture& texture, const SDL_Rect* sourceRect, const SDL_Rect* modifierRect, TargetTexture& outputTexture)
	{
		return false; // TODO
	}

}