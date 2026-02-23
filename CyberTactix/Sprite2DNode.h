#pragma once

#include<vector>
#include "MoveableObject2D.h"
#include "SpriteMesh.h"


namespace pix
{
	// A Sprite2DNode instance is functionally a Sprite2D object that can participate in a transform hierarchy.
	// - Non-owning: lifetime of parent/children is managed externally.
	// - On destruction, the node detaches from its parent and re-roots its children in world space.
	// - Hierarchy math assumes no rotated non-uniform scaling in the ancestor chain (uniform scale is safe).
	// 
	// NOTE: If you delete (also by changing their parent) children in a loop, use while(!children_.empty()) delete back(); or snapshot first, 
	// because deleting a child shrinks the children vector.
	// 
	// Philosophy:
	// Sprite2DNode is the minimal hierarchical renderable 2D entity that can MOVE in space. 
	// It does not own the parent or children. Thus, decisions about ownership are left to the caller to grant the respective flexibility.
	class Sprite2DNode : public MovableObject2D
	{
	public:

		Sprite2DNode();

		Sprite2DNode(const SpriteMesh* mesh, const Transform2D& transform);

		Sprite2DNode(const SpriteMesh* mesh, const Transform2D& transform, const Transform2D& prevTransform);

		Sprite2DNode(const SpriteMesh* mesh, const Vec2& position, const Vec2f& scale = { 1.0f, 1.0f }, const Rotation2D& rotation = Rotation2D());

		// Prevent copying 
		Sprite2DNode(const Sprite2DNode&) = delete;    
		Sprite2DNode& operator= (const Sprite2DNode&) = delete;

		// On destruction, parent and children become detached. The children become roots in world space,
		// and this node is removed from its parent’s child list. 
		virtual ~Sprite2DNode();

		// Sets a new parent with proper attach/detach management. newParent may be nullptr to set the root node with Transform in world space.
		// If newParent is the current parent, this node, or a descendent of it, the parent won't change.
		void SetParent(Sprite2DNode* newParent);

		Sprite2DNode* GetParent() const;

		const std::vector<Sprite2DNode*>& GetChildren() const;

		// Returns the effective single transform in world space. 
		 // Correct result is only guaranteed with no rotated non-uniform scaling in the ancestor chain (uniform scale is safe). 
		Transform2D GetGlobalTransform() const;

		Transform2D GetGlobalPreviousTransform() const;

		const SpriteMesh* Mesh; 

	private:

		Sprite2DNode* parent_;
		std::vector<Sprite2DNode*> children_;  

	};

}
