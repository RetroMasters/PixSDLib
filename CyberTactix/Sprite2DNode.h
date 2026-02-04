#pragma once

#include "MoveableObject2D.h"
#include "SpriteMesh.h"

//Sprite is the basis class for a sprite-based object that can move in space
namespace pix
{
	class Sprite2DNode : public MoveableObject2D
	{
	public:

		Sprite2DNode() ;

		Sprite2DNode(const SpriteMesh* mesh, const Transform2D& transform) ;

		Sprite2DNode(const SpriteMesh* mesh, const Transform2D& transform, const Transform2D& prevTransform) ;

		Sprite2DNode(const SpriteMesh* mesh, const Vector2d& position, const Vector2f& scale = { 1.0f, 1.0f }, const Rotation2D& rotation = Rotation2D()) ;

		virtual ~Sprite2DNode() ;


		void SetParent(Sprite2DNode* newParent) ;

		Sprite2DNode* GetParent() const ;

		const std::vector<Sprite2DNode*>& GetChildren() const ;


		Transform2D GetGlobalTransform() const ;

		Transform2D GetGlobalPreviousTransform() const ;



		const SpriteMesh* Mesh; //defines the "sprite-model" in model space

	private:

		Sprite2DNode* parent_;
		std::vector<Sprite2DNode*> children_;  //TODO: has to be managed by methods

	};

}
