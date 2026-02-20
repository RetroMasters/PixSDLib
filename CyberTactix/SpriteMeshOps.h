#pragma once

#include<SDL_pixels.h>
#include "SpriteMesh.h"
#include "UV.h"

namespace pix
{
	// Returns an axis-aligned SpriteMesh from two corner positions (in model space) and the uvRect.
	// The vertex color of all vertices is set to color.
	// Positions are not validated; inverted corners produce inverted geometry.
	SpriteMesh GetSpriteMesh(Vector2f topLeftPosition, Vector2f bottomRightPosition, UVRect uvRect = UVRect(), SDL_Color color = {255,255,255,255});

	// Returns a SpriteMesh from four corner positions (in model space) and the uvQuad.
    // The vertex color of all vertices is set to color.
	SpriteMesh GetSpriteMesh(Vector2f topLeftPosition, Vector2f topRightPosition, Vector2f bottomRightPosition, Vector2f bottomLeftPosition, const UVQuad& uvQuad = UVQuad(), SDL_Color color = { 255,255,255,255 });

	// Returns the size (width,height) of the mesh's axis-aligned bounding box
	Vector2f GetBoundsSize(const SpriteMesh& mesh);

	// Sets the UV coordinates of the mesh from uvRect.
    // mesh and uvRect are not validated (no clamping or corner-order enforcement); corners are mapped by name.
	void SetUV(SpriteMesh& mesh, UVRect uvRect);

	// Sets the UV coordinates of the mesh from uvQuad.
	// mesh and uvQuad are not validated (no clamping or corner-order enforcement); corners are mapped by name.
	void SetUV(SpriteMesh& mesh, const UVQuad& uvQuad);
}
