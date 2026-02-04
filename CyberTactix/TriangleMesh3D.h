#pragma once

#include <vector>
#include "Texture.h"
//#include "SpriteMesh.h"
#include "PixMath.h"
//#include "MathTypes3D.h"

namespace pix
{
	struct Vertex3D
	{
		Vertex3D()  = default;

		Vertex3D(const Vector3f& position, const SDL_Color& color, const Vector2f& texCoordinates, const Vector3f& normal) :
			Position(position),
			Color(color),
			TexCoords(texCoordinates),
			Normal(normal)
		{
		}

		Vector3f   Position;
		SDL_Color  Color;
		Vector2f   TexCoords;
		Vector3f   Normal;
	};


	/// <summary>
	/// The mesh for MeshSprite
	/// </summary>
	struct TriangleMesh3D
	{
		TriangleMesh3D() = default;

		std::vector<Vertex3D> Vertices; //defines the "sprite-model" in model space

		const Texture* MeshTexture;
	};

}
