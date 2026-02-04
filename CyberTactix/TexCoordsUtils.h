#pragma once

#include "TexCoordsTypes.h"

namespace pix
{

	inline UVRect GetUVRect(Vector2f texSize, const PixelRect& rect) 
	{
		if (texSize.X < 1.0f || texSize.Y < 1.0f)	return UVRect(0.0f, 0.0f, 0.0f, 0.0f);

		return UVRect(rect.Position / texSize, rect.GetBottomRight() / texSize);
	}

}