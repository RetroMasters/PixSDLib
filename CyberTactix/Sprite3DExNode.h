#pragma once

#include "MoveableObject3D.h"
#include "TriangleMesh2D.h"

namespace pix
{
	// A Sprite3DExNode instance is functionally a Sprite3DEx object that can participate in a transform hierarchy.
	// - Non-owning: lifetime of parent/children is managed externally.
	// - On destruction, the node detaches from its parent and re-roots its children in world space.
	// - Hierarchy math assumes no rotated non-uniform scaling in the ancestor chain (uniform scale is safe).
	// 
	// NOTE: If you delete (also by changing their parent) children in a loop, use while(!children_.empty()) delete back(); or snapshot first, 
	// because deleting a child shrinks the children vector.
	// 
	// Philosophy:
	// Sprite3DExNode is the most complete foundational hierarchical renderable 3D entity that can MOVE in space. 
	// It does not own the parent or children. Thus, decisions about ownership are left to the caller to grant the respective flexibility.
	class Sprite3DExNode : public MovableObject3D
	{
	public:

		Sprite3DExNode();

		Sprite3DExNode(const TriangleMesh2D* mesh, const Transform3D& transform);

		Sprite3DExNode(const TriangleMesh2D* mesh, const Transform3D& transform, const Transform3D& prevTransform);

		Sprite3DExNode(const TriangleMesh2D* mesh, const Vec3& position, const Vec3f& scale = { 1.0f, 1.0f, 0.0f }, const Rotation3D& rotation = Rotation3D());

		// Prevent copying
		Sprite3DExNode(const Sprite3DExNode&) = delete;
		Sprite3DExNode& operator= (const Sprite3DExNode&) = delete;

		// On destruction, parent and children become detached. The children become roots in world space,
		// and this node is removed from its parent’s child list. 
		virtual ~Sprite3DExNode();

		// Sets a new parent with proper attach/detach management. newParent may be nullptr to set the root node with Transform in world space.
		// If newParent is the current parent, this node, or a descendent of it, the parent won't change.
		void SetParent(Sprite3DExNode* newParent);

		Sprite3DExNode* GetParent() const;

		const std::vector<Sprite3DExNode*>& GetChildren() const;

		// Returns the effective single transform in world space. 
		// Correct result is only guaranteed with no rotated non-uniform scaling in the ancestor chain (uniform scale is safe). 
		Transform3D GetGlobalTransform() const;

		Transform3D GetGlobalPrevTransform() const;


		const TriangleMesh2D* Mesh; 

	private:

		Sprite3DExNode* parent_;
		std::vector<Sprite3DExNode*> children_;  

	};

}


