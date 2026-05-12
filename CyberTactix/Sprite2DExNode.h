#pragma once

#include <vector>
#include "MovableObject2D.h"
#include "TriangleMesh2D.h"

namespace pix
{
	// A Sprite2DExNode instance is functionally a Sprite2DEx object that can participate in a transform hierarchy.
	// - Mesh, parent, and children are non-owning references managed externally.
	// - On destruction, the node detaches from its parent and re-roots its children in world space.
	// 
	// NOTE: Changing a child's parent or deleting a child while iterating over GetChildren() shrinks the child list. 
    // Iterate from back to front, use while (!GetChildren().empty()), or snapshot the child pointer list first.
	// 
	// Philosophy:
	// Sprite2DExNode is the most complete foundational hierarchical renderable 2D entity that can move through space. 
	// It does not own the parent or children, leaving ownership decisions to the caller to preserve flexibility.
	class Sprite2DExNode : public MovableObject2D
	{
	public:

		Sprite2DExNode() = default;
		Sprite2DExNode(const TriangleMesh2D* mesh, const Transform2D& transform);
		Sprite2DExNode(const TriangleMesh2D* mesh, const Transform2D& transform, const Transform2D& prevTransform);
		explicit Sprite2DExNode(const TriangleMesh2D* mesh, Vec2 position = Vec2(0.0, 0.0), Vec2f scale = Vec2f(1.0f, 1.0f), Rotation2D rotation = Rotation2D());

		// Prevent copying 
		Sprite2DExNode(const Sprite2DExNode&) = delete;
		Sprite2DExNode& operator= (const Sprite2DExNode&) = delete;

		// On destruction, parent and children become detached. The children become roots in world space,
		// and this node is removed from its parent’s child list. 
		~Sprite2DExNode() override;

		// Sets a new parent with proper attach/detach management.
		// newParent may be nullptr to make this node a root with Transform in world space.
		// If newParent is the current parent, this node, or a descendant of it, the parent won't change.
		// Returns true if parent is already set or has changed to newParent, false otherwise.
		bool SetParent(Sprite2DExNode* newParent);

		Sprite2DExNode* GetParent() const;

		const std::vector<Sprite2DExNode*>& GetChildren() const;

		// Returns the effective decomposed transform in world space. 
		// Correct result is only guaranteed with no rotated non-uniform scaling in the ancestor chain (rotated uniform nonzero scale is safe). 
		Transform2D GetGlobalTransform() const;

		// Returns the previous effective decomposed transform in world space.
        // Same limitations as GetGlobalTransform().
		Transform2D GetPrevGlobalTransform() const;



		const TriangleMesh2D* Mesh = nullptr; 

	private:

		Sprite2DExNode* parent_ = nullptr;
		std::vector<Sprite2DExNode*> children_;  

	};

}

