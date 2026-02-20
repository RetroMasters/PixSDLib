#include "SpriteMeshOps.h"

namespace pix
{
	SpriteMesh GetSpriteMesh(Vector2f topLeftPosition, Vector2f bottomRightPosition, UVRect uvRect, SDL_Color color)
	{
		Vertex2D topLeft(topLeftPosition, color, uvRect.TopLeft);
		Vertex2D topRight(Vector2f(bottomRightPosition.X, topLeftPosition.Y), color, Vector2f(uvRect.BottomRight.X, uvRect.TopLeft.Y));
		Vertex2D bottomRight(bottomRightPosition, color, uvRect.BottomRight);
		Vertex2D bottomLeft(Vector2f(topLeftPosition.X, bottomRightPosition.Y), color, Vector2f(uvRect.TopLeft.X, uvRect.BottomRight.Y));

		return SpriteMesh(topLeft, topRight, bottomRight, bottomLeft);
	}

	SpriteMesh GetSpriteMesh(Vector2f topLeftPosition, Vector2f topRightPosition, Vector2f bottomRightPosition, Vector2f bottomLeftPosition, const UVQuad& uvQuad, SDL_Color color)
	{
		Vertex2D topLeft(topLeftPosition, color, uvQuad.TopLeft);
		Vertex2D topRight(topRightPosition, color, uvQuad.TopRight);
		Vertex2D bottomRight(bottomRightPosition, color, uvQuad.BottomRight);
		Vertex2D bottomLeft(bottomLeftPosition, color, uvQuad.BottomLeft);

		return SpriteMesh(topLeft, topRight, bottomRight, bottomLeft);
	}

	Vector2f GetBoundsSize(const SpriteMesh& mesh)
	{
		Vector2f min(mesh.BottomLeft().Position);
		Vector2f max(mesh.TopRight().Position);

		if (mesh.TopLeft().Position.X < min.X) min.X = mesh.TopLeft().Position.X;
		if (mesh.TopLeft().Position.Y < min.Y) min.Y = mesh.TopLeft().Position.Y;
		if (mesh.TopRight().Position.X < min.X) min.X = mesh.TopRight().Position.X;
		if (mesh.TopRight().Position.Y < min.Y) min.Y = mesh.TopRight().Position.Y;
		if (mesh.BottomRight().Position.X < min.X) min.X = mesh.BottomRight().Position.X;
		if (mesh.BottomRight().Position.Y < min.Y) min.Y = mesh.BottomRight().Position.Y;
		
		if (mesh.BottomRight().Position.X > max.X) max.X = mesh.BottomRight().Position.X;
		if (mesh.BottomRight().Position.Y > max.Y) max.Y = mesh.BottomRight().Position.Y;
		if (mesh.BottomLeft().Position.X > max.X) max.X = mesh.BottomLeft().Position.X;
		if (mesh.BottomLeft().Position.Y > max.Y) max.Y = mesh.BottomLeft().Position.Y;
		if (mesh.TopLeft().Position.X > max.X) max.X = mesh.TopLeft().Position.X;
		if (mesh.TopLeft().Position.Y > max.Y) max.Y = mesh.TopLeft().Position.Y;
		
		return max - min;
	}

	void SetUV(SpriteMesh& mesh, UVRect uvRect)
	{
		mesh.TopLeft().UV = uvRect.TopLeft;
		mesh.TopRight().UV = Vector2f(uvRect.BottomRight.X, uvRect.TopLeft.Y);
		mesh.BottomRight().UV = uvRect.BottomRight;
		mesh.BottomLeft().UV = Vector2f(uvRect.TopLeft.X, uvRect.BottomRight.Y);
	}

	void SetUV(SpriteMesh& mesh, const UVQuad& uvQuad) 
	{
		mesh.TopLeft().UV = uvQuad.TopLeft;
		mesh.TopRight().UV = uvQuad.TopRight;
		mesh.BottomRight().UV = uvQuad.BottomRight;
		mesh.BottomLeft().UV = uvQuad.BottomLeft;
	}
}