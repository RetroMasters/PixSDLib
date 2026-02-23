#pragma once

#include <SDL_rect.h>
#include "UV.h"

namespace pix
{

	//########################################### UVRECT OPERATIONS #################################

	// Returns a UVRect from the texture region provided in pixels by rect.
	// texWidth/texHeight is used for normalization and has to be the size of the texture in pixels.
	// GetUVRect() just transforms the rect into the UVRect format without assuming its validity, 
	// so negative width or height will flip the BottomRight point respectively. 
	// If one of the texSize dimensions is zero or smaller, a zero-initialized UVRect is returned.
	UVRect GetUVRect(int texWidth, int texHeight, SDL_Rect rect);

	// Returns the width and the height of the rect
	Vec2f GetSize(UVRect rect);

	// Returns a copy of rect with flipped X-coordinates
	UVRect GetFlippedX(UVRect rect);

	// Returns a copy of rect with flipped Y-coordinates
	UVRect GetFlippedY(UVRect rect);

	// Returns a copy of rect with flipped XY-coordinates
	UVRect GetFlippedXY(UVRect rect);

	// Returns true if rect lies fully inside a texture of size (texWidth, texHeight).
    // Returns false if texture dimensions are non-positive, or if rect has negative position or negative width/height.
	bool IsRectInTexBounds(int texWidth, int texHeight, SDL_Rect rect);

	//########################################### UVQUAD OPERATIONS #################################

    // Returns a UVQuad from a UVRect
	UVQuad GetUVQuad(UVRect rect);

	// Returns a UVQuad from the texture region provided in pixels by rect.
	// texWidth/texHeight is used for normalization and has to be the size of the texture in pixels.
	UVQuad GetUVQuad(int texWidth, int texHeight, SDL_Rect rect);

	// Returns the width and the height of quad's bounding box
	Vec2f GetBoundsSize(const UVQuad& quad);
	
	// Returns a copy of quad with flipped X-coordinates
	UVQuad GetFlippedX(const UVQuad& quad);

	// Returns a copy of quad with flipped Y-coordinates
	UVQuad GetFlippedY(const UVQuad& quad);

	// Returns a copy of quad with flipped XY-coordinates
	UVQuad GetFlippedXY(const UVQuad& quad);
}