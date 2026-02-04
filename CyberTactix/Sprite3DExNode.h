#pragma once

#include "MoveableObject3D.h"
#include "TriangleMesh2D.h"

//Sprite is the basis class for a sprite-based object that can move in space
namespace pix
{
	class Sprite3DExNode : public MoveableObject3D
	{
	public:

		Sprite3DExNode();

		Sprite3DExNode(const TriangleMesh2D* mesh, const Transform3D& transform);

		Sprite3DExNode(const TriangleMesh2D* mesh, const Transform3D& transform, const Transform3D& prevTransform);

		Sprite3DExNode(const TriangleMesh2D* mesh, const Vector3d& position, const Vector3f& scale = { 1.0f, 1.0f, 0.0f }, const Rotation3D& rotation = Rotation3D());

		virtual ~Sprite3DExNode();


		void SetParent(Sprite3DExNode* newParent);

		Sprite3DExNode* GetParent() const;

		const std::vector<Sprite3DExNode*>& GetChildren() const;


		Transform3D GetGlobalTransform() const;

		Transform3D GetGlobalPrevTransform() const;


		const TriangleMesh2D* Mesh; //defines the "sprite-model" in model space

	private:

		Sprite3DExNode* parent_;
		std::vector<Sprite3DExNode*> children_;  

	};

}


