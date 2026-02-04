#pragma once

#include "MoveableObject3D.h"
#include "SpriteMesh.h"

//Note, don't draw sprites directly to the display's rendertarget (NULL) but to a set one since sdl-scale can mess up zooming

//Sprite is the basis class for a sprite-based object that can move in space
namespace pix
{
	class Sprite3DNode : public MoveableObject3D
	{
	public:

		Sprite3DNode();

		Sprite3DNode(const SpriteMesh* mesh, const Transform3D& transform);

		Sprite3DNode(const SpriteMesh* mesh, const Transform3D& transform, const Transform3D& prevTransform);

		Sprite3DNode(const SpriteMesh* mesh, const Vector3d& position, const Vector3f& scale = { 1.0f, 1.0f, 0.0f }, const Rotation3D& rotation = Rotation3D());

		virtual ~Sprite3DNode();


		void SetParent(Sprite3DNode* newParent);

		Sprite3DNode* GetParent() const;

		const std::vector<Sprite3DNode*>& GetChildren() const;


		Transform3D GetGlobalTransform() const;

		Transform3D GetGlobalPrevTransform() const;


		const SpriteMesh* Mesh; //defines the "sprite-model" in model space

	private:

		Sprite3DNode* parent_;
		std::vector<Sprite3DNode*> children_;  //TODO: has to be managed by methods

	};

}

