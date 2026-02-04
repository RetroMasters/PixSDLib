#pragma once

#include <vector>
#include "Texture.h"
//#include "SpriteMesh.h"
#include "pixMath.h"
//#include "MathTypes3D.h"

namespace pix
{

	struct Vertex2DEx
	{
		Vertex2DEx()  = default;

		Vertex2DEx(const Vector2f& position, const SDL_Color& color, const Vector2f& texCoordinates, const Vector3f& normal)  :
			Position(position),
			Color(color),
			TexCoords(texCoordinates),
			Normal(normal)
		{
		}

		Vector2f  Position;
		SDL_Color Color;
		Vector2f  TexCoords;
		Vector3f  Normal;
	};


	/// <summary>
	/// The mesh for MeshSprite
	/// </summary>
	struct TriangleMesh2D
	{
		TriangleMesh2D()  = default;

		std::vector<Vertex2DEx> Vertices; //defines the "sprite-model" in model space

		const Texture* MeshTexture = nullptr; //TODO: remove
	};

}
