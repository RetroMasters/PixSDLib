#pragma once

#include <SDL_rect.h>
#include "TargetTexture.h"


namespace pix
{
    // Renders a modified texture region into outputTexture.
    // The source region is copied first, then the modifier region is stretched to the same size
    // and rendered over it using the modifier texture's current blend mode.
    // If sourceRect is nullptr, the full source texture is used.
    // If modifierRect is nullptr, the full modifier texture is used.
    // outputTexture is reallocated only if its size does not match the source region size.
    // The source texture's blend mode is temporarily changed internally and restored before returning.
    bool BlendTextureRegion(Texture& sourceTexture, const SDL_Rect* sourceRect, const Texture& modifierTexture, const SDL_Rect* modifierRect, TargetTexture& outputTexture);

    // Renders a modified texture region into outputTexture using two regions from the same texture.
    // The source region is copied first, then the modifier region is stretched to the same size
    // and rendered over it using the texture's current blend mode.
    // If sourceRect is nullptr, the full texture is used as the source region.
    // If modifierRect is nullptr, the full texture is used as the modifier region.
    // outputTexture is reallocated only if its size does not match the source region size.
    // The texture's blend mode is temporarily changed internally and restored before returning.
    bool BlendTextureRegion(Texture& texture, const SDL_Rect* sourceRect, const SDL_Rect* modifierRect, TargetTexture& outputTexture);

}