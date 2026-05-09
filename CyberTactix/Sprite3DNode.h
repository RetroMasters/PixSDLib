#pragma once

#include <vector>
#include "MovableObject3D.h"
#include "SpriteMesh.h"

namespace pix
{
	// A Sprite3DNode instance is functionally a Sprite3D object that can participate in a transform hierarchy.
    // - Mesh, parent, and children are non-owning references managed externally.
    // - On destruction, the node detaches from its parent and re-roots its children in world space.
	// 
	// NOTE: Changing a child's parent or deleting a child while iterating over GetChildren() shrinks the child list. 
	// Iterate from back to front, use while (!GetChildren().empty()), or snapshot the child pointer list first.
	// 
	// Philosophy:
	// Sprite3DNode is the minimal hierarchical renderable 3D entity that can move through space. 
	// It does not own the parent or children, leaving ownership decisions to the caller to preserve flexibility.
	class Sprite3DNode : public MovableObject3D
	{
	public:

		Sprite3DNode() = default;
		Sprite3DNode(const SpriteMesh* mesh, const Transform3D& transform);
		Sprite3DNode(const SpriteMesh* mesh, const Transform3D& transform, const Transform3D& prevTransform);
		Sprite3DNode(const SpriteMesh* mesh, const Vec3& position, Vec3f scale = Vec3f(1.0f, 1.0f, 1.0f), const Rotation3D& rotation = Rotation3D());

		// Prevent copying
		Sprite3DNode(const Sprite3DNode&) = delete;
		Sprite3DNode& operator= (const Sprite3DNode&) = delete;

		// On destruction, parent and children become detached. The children become roots in world space,
		// and this node is removed from its parent’s child list. 
		~Sprite3DNode() override;

		// Sets a new parent with proper attach/detach management.
		// newParent may be nullptr to make this node a root with Transform in world space.
		// If newParent is the current parent, this node, or a descendant of it, the parent won't change.
		// Returns true if parent is already set or has changed to newParent, false otherwise.
		bool SetParent(Sprite3DNode* newParent);

		Sprite3DNode* GetParent() const;

		const std::vector<Sprite3DNode*>& GetChildren() const;

		// Returns the effective decomposed transform in world space. 
	    // Correct result is only guaranteed with no rotated non-uniform scaling in the ancestor chain (rotated uniform nonzero scale is safe). 
		Transform3D GetGlobalTransform() const;

		// Returns the previous effective decomposed transform in world space.
		// Same limitations as GetGlobalTransform().
		Transform3D GetPrevGlobalTransform() const;



		const SpriteMesh* Mesh = nullptr;

	private:

		Sprite3DNode* parent_ = nullptr;
		std::vector<Sprite3DNode*> children_; 

	};

}

