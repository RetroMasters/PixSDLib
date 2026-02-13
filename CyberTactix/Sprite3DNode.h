#pragma once

#include <vector>
#include "MoveableObject3D.h"
#include "SpriteMesh.h"


// A Sprite3DNode instance is functionally a Sprite3D object that can participate in a transform hierarchy.
// - Non-owning: lifetime of parent/children is managed externally.
// - On destruction, the node detaches from its parent and re-roots its children in world space.
// - Hierarchy math assumes no rotated non-uniform scaling in the ancestor chain (uniform scale is safe).
// 
// NOTE: If you delete (also by changing their parent) children in a loop, use while(!children_.empty()) delete back(); or snapshot first, 
// because deleting a child shrinks the children vector.
// 
// Philosophy:
// Sprite3DNode is the minimal hierarchical renderable 3D entity that can MOVE in space. 
// It does not own the parent or children. Thus, decisions about ownership are left to the caller to grant the respective flexibility.
namespace pix
{
	class Sprite3DNode : public MovableObject3D
	{
	public:

		Sprite3DNode();

		Sprite3DNode(const SpriteMesh* mesh, const Transform3D& transform);

		Sprite3DNode(const SpriteMesh* mesh, const Transform3D& transform, const Transform3D& prevTransform);

		Sprite3DNode(const SpriteMesh* mesh, const Vector3d& position, const Vector3f& scale = { 1.0f, 1.0f, 1.0f }, const Rotation3D& rotation = Rotation3D());

		// Prevent copying
		Sprite3DNode(const Sprite3DNode&) = delete;
		Sprite3DNode& operator= (const Sprite3DNode&) = delete;

		// On destruction, parent and children become detached. The children become roots in world space,
		// and this node is removed from its parent’s child list. 
		virtual ~Sprite3DNode();

		// Sets a new parent with proper detach management. newParent may be nullptr to set the root node with Transform in world space.
		// If newParent is the current parent, this node, or a descendent of it, the parent won't change.
		void SetParent(Sprite3DNode* newParent);

		Sprite3DNode* GetParent() const;

		const std::vector<Sprite3DNode*>& GetChildren() const;

		// Returns the effective single transform in world space. 
	    // Correct result is only guaranteed with no rotated non-uniform scaling in the ancestor chain (uniform scale is safe). 
		Transform3D GetGlobalTransform() const;

		Transform3D GetGlobalPrevTransform() const;


		const SpriteMesh* Mesh;

	private:

		Sprite3DNode* parent_;
		std::vector<Sprite3DNode*> children_; 

	};

}

