#include "SpriteMeshUtils.h"

namespace pix
{

	SpriteMesh GetQuadMesh2D(Vector2f topLeftPosition, Vector2f bottomRightPosition, const UVRect& rectUV) 
	{
		constexpr SDL_Color white = { 255, 255, 255, 255 };

		Vertex2D topLeft(Vector2f(topLeftPosition), white, rectUV.TopLeft);
		Vertex2D topRight(Vector2f(bottomRightPosition.X, topLeftPosition.Y), white, Vector2f(rectUV.BottomRight.X, rectUV.TopLeft.Y));
		Vertex2D bottomRight(Vector2f(bottomRightPosition), white, rectUV.BottomRight);
		Vertex2D bottomLeft(Vector2f(topLeftPosition.X, bottomRightPosition.Y), white, Vector2f(rectUV.TopLeft.X, rectUV.BottomRight.Y));

		return SpriteMesh(topLeft, topRight, bottomRight, bottomLeft);
	}

	Vector2f GetBoundsSize(const SpriteMesh& mesh) 
	{
		Vector2f min(mesh.BottomLeft().Position);
		Vector2f max(mesh.TopRight().Position);

		for (int i = 0; i < 4; i++)
		{
			if (mesh.Vertices[i].Position.X < min.X) min.X = mesh.Vertices[i].Position.X;
			if (mesh.Vertices[i].Position.Y < min.Y) min.Y = mesh.Vertices[i].Position.Y;
			if (mesh.Vertices[i].Position.X > max.X) max.X = mesh.Vertices[i].Position.X;
			if (mesh.Vertices[i].Position.Y > max.Y) max.Y = mesh.Vertices[i].Position.Y;
		}

		return max - min;
	}

	void SetUV(SpriteMesh& mesh, const UVQuad& quadUV) 
	{
		mesh.TopLeft().UV = quadUV.TopLeft;
		mesh.TopRight().UV = quadUV.TopRight;
		mesh.BottomRight().UV = quadUV.BottomRight;
		mesh.BottomLeft().UV = quadUV.BottomLeft;
	}

	void SetUV(SpriteMesh& mesh, const UVRect& rectUV) 
	{
		mesh.TopLeft().UV = rectUV.TopLeft;
		mesh.TopRight().UV = Vector2f(rectUV.BottomRight.X, rectUV.TopLeft.Y);
		mesh.BottomRight().UV = rectUV.BottomRight;
		mesh.BottomLeft().UV = Vector2f(rectUV.TopLeft.X, rectUV.BottomRight.Y);
	}

}