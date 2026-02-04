#pragma once

#include "MoveableObject2D.h"
#include "TriangleMesh2D.h"


//Sprite is the basis class for a sprite-based object that can move in space
namespace pix
{
	class Sprite2DExNode : public MoveableObject2D
	{
	public:

		Sprite2DExNode() ;

		Sprite2DExNode(const TriangleMesh2D* mesh, const Transform2D& transform) ;

		Sprite2DExNode(const TriangleMesh2D* mesh, const Transform2D& transform, const Transform2D& prevTransform) ;

		Sprite2DExNode(const TriangleMesh2D* mesh, const Vector2d& position, const Vector2f& scale = { 1.0f, 1.0f }, const Rotation2D& rotation = Rotation2D()) ;

		virtual ~Sprite2DExNode() ;


		void SetParent(Sprite2DExNode* newParent) ;


		Sprite2DExNode* GetParent() const ;

		const std::vector<Sprite2DExNode*>& GetChildren() const ;



		Transform2D GetGlobalTransform() const ;

		Transform2D GetGlobalPreviousTransform() const ;



		const TriangleMesh2D* Mesh; //defines the "sprite-model" in model space

	private:

		Sprite2DExNode* parent_;
		std::vector<Sprite2DExNode*> children_;  //TODO: has to be managed by methods

	};

}

