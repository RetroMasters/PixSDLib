#pragma once

#include <SDL_pixels.h>
#include "PixMath.h"
#include "UV.h"
#include "SpriteMesh.h"

namespace pix
{
	// Returns an axis-aligned SpriteMesh from two corner positions in model space and a UVRect.
	// The vertex color of all vertices is set to color.
	// Positions are not validated; inverted corners produce inverted geometry.
	SpriteMesh GetSpriteMesh(Vec2f topLeftPosition, Vec2f bottomRightPosition, UVRect uvRect = UVRect(), SDL_Color color = { 255, 255, 255, 255 });

	// Returns a SpriteMesh from four corner positions in model space and a UVQuad.
    // The vertex color of all vertices is set to color.
	SpriteMesh GetSpriteMesh(Vec2f topLeftPosition, Vec2f topRightPosition, Vec2f bottomRightPosition, Vec2f bottomLeftPosition, const UVQuad& uvQuad = UVQuad(), SDL_Color color = { 255, 255, 255, 255 });

	// Returns the width and height of the mesh's axis-aligned bounding box
	Vec2f GetBoundsSize(const SpriteMesh& mesh);

	// Sets the UV coordinates of the mesh from a UVRect.
    // mesh and uvRect are not validated (no clamping or corner-order enforcement); corners are mapped by name.
	void SetUV(SpriteMesh& mesh, UVRect uvRect);

	// Sets the UV coordinates of the mesh from a UVQuad.
	// mesh and uvQuad are not validated (no clamping or corner-order enforcement); corners are mapped by name.
	void SetUV(SpriteMesh& mesh, const UVQuad& uvQuad);
}
