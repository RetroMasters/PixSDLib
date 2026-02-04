#pragma once

#include "MoveableObject2D.h"
#include "SpriteMesh.h"


//Sprite is the basis class for a single-textured object that can move in space
namespace pix
{
	class Sprite2D : public MoveableObject2D
	{
	public:

		Sprite2D() ;
		Sprite2D(const SpriteMesh* mesh, const Transform2D& transform) ;
		Sprite2D(const SpriteMesh* mesh, const Transform2D& transform, const Transform2D& prevTransform) ;
		Sprite2D(const SpriteMesh* mesh, const Vector2d& position = { 0.0, 0.0 }, const Vector2f& scale = { 1.0f, 1.0f }, const Rotation2D& rotation = Rotation2D()) ;

		virtual ~Sprite2D()  = default;

		const SpriteMesh* Mesh; //defines the "sprite-model" in model space
	};

}
