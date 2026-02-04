#pragma once

#include "SpriteMesh.h"
#include"TexCoordsTypes.h"


namespace pix
{

	SpriteMesh GetQuadMesh2D(Vector2f topLeftPosition, Vector2f bottomRightPosition, const UVRect& rectUV = UVRect(0.0f, 0.0f, 1.0f, 1.0f)) ;

	Vector2f GetBoundsSize(const SpriteMesh& mesh) ;

	void SetUV(SpriteMesh& mesh, const UVQuad& quadUV) ;

	void SetUV(SpriteMesh& mesh, const UVRect& rectUV) ;

}
