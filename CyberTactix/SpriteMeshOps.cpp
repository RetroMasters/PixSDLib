#include "SpriteMeshOps.h"

namespace pix
{
	SpriteMesh GetSpriteMesh(Vec2f topLeftPosition, Vec2f bottomRightPosition, UVRect uvRect, SDL_Color color)
	{
		Vertex2D topLeft(topLeftPosition, color, uvRect.TopLeft);
		Vertex2D topRight(Vec2f(bottomRightPosition.X, topLeftPosition.Y), color, Vec2f(uvRect.BottomRight.X, uvRect.TopLeft.Y));
		Vertex2D bottomRight(bottomRightPosition, color, uvRect.BottomRight);
		Vertex2D bottomLeft(Vec2f(topLeftPosition.X, bottomRightPosition.Y), color, Vec2f(uvRect.TopLeft.X, uvRect.BottomRight.Y));

		return SpriteMesh(topLeft, topRight, bottomRight, bottomLeft);
	}

	SpriteMesh GetSpriteMesh(Vec2f topLeftPosition, Vec2f topRightPosition, Vec2f bottomRightPosition, Vec2f bottomLeftPosition, const UVQuad& uvQuad, SDL_Color color)
	{
		Vertex2D topLeft(topLeftPosition, color, uvQuad.TopLeft());
		Vertex2D topRight(topRightPosition, color, uvQuad.TopRight());
		Vertex2D bottomRight(bottomRightPosition, color, uvQuad.BottomRight());
		Vertex2D bottomLeft(bottomLeftPosition, color, uvQuad.BottomLeft());

		return SpriteMesh(topLeft, topRight, bottomRight, bottomLeft);
	}

	Vec2f GetBoundsSize(const SpriteMesh& mesh) // Compute the size of the axis-aligned bounding box
	{
		const Vec2f topLeft(mesh.TopLeft().Position);
		const Vec2f topRight(mesh.TopRight().Position);
		const Vec2f bottomRight(mesh.BottomRight().Position);
		const Vec2f bottomLeft(mesh.BottomLeft().Position);

		Vec2f min(bottomLeft);
		Vec2f max(topRight);

		if (topLeft.X < min.X) min.X = topLeft.X;
		if (topLeft.Y < min.Y) min.Y = topLeft.Y;
		if (topRight.X < min.X) min.X = topRight.X;
		if (topRight.Y < min.Y) min.Y = topRight.Y;
		if (bottomRight.X < min.X) min.X = bottomRight.X;
		if (bottomRight.Y < min.Y) min.Y = bottomRight.Y;

		if (bottomRight.X > max.X) max.X = bottomRight.X;
		if (bottomRight.Y > max.Y) max.Y = bottomRight.Y;
		if (bottomLeft.X > max.X) max.X = bottomLeft.X;
		if (bottomLeft.Y > max.Y) max.Y = bottomLeft.Y;
		if (topLeft.X > max.X) max.X = topLeft.X;
		if (topLeft.Y > max.Y) max.Y = topLeft.Y;

		return max - min;
	}

	void SetUV(SpriteMesh& mesh, UVRect uvRect)
	{
		mesh.TopLeft().UV = uvRect.TopLeft;
		mesh.TopRight().UV = Vec2f(uvRect.BottomRight.X, uvRect.TopLeft.Y);
		mesh.BottomRight().UV = uvRect.BottomRight;
		mesh.BottomLeft().UV = Vec2f(uvRect.TopLeft.X, uvRect.BottomRight.Y);
	}

	void SetUV(SpriteMesh& mesh, const UVQuad& uvQuad) 
	{
		mesh.TopLeft().UV = uvQuad.TopLeft();
		mesh.TopRight().UV = uvQuad.TopRight();
		mesh.BottomRight().UV = uvQuad.BottomRight();
		mesh.BottomLeft().UV = uvQuad.BottomLeft();
	}
}