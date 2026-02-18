#pragma once

#include <vector>
#include "MoveableObject2D.h"
#include "TriangleMesh2D.h"

namespace pix
{
	// A Sprite2DExNode instance is functionally a Sprite2DEx object that can participate in a transform hierarchy.
	// - Non-owning: lifetime of parent/children is managed externally.
	// - On destruction, the node detaches from its parent and re-roots its children in world space.
	// - Hierarchy math assumes no rotated non-uniform scaling in the ancestor chain (uniform scale is safe).
	// 
	// NOTE: If you delete (also by changing their parent) children in a loop, use while(!children_.empty()) delete back(); or snapshot first, 
	// because deleting a child shrinks the children vector.
	// 
	// Philosophy:
	// Sprite2DExNode is the most complete foundational hierarchical renderable 2D entity that can MOVE in space. 
	// It does not own the parent or children. Thus, decisions about ownership are left to the caller to grant the respective flexibility.
	class Sprite2DExNode : public MovableObject2D
	{
	public:

		Sprite2DExNode() ;

		Sprite2DExNode(const TriangleMesh2D* mesh, const Transform2D& transform) ;

		Sprite2DExNode(const TriangleMesh2D* mesh, const Transform2D& transform, const Transform2D& prevTransform) ;

		Sprite2DExNode(const TriangleMesh2D* mesh, const Vector2d& position, const Vector2f& scale = { 1.0f, 1.0f }, const Rotation2D& rotation = Rotation2D()) ;

		// Prevent copying 
		Sprite2DExNode(const Sprite2DExNode&) = delete;
		Sprite2DExNode& operator= (const Sprite2DExNode&) = delete;

		// On destruction, parent and children become detached. The children become roots in world space,
		// and this node is removed from its parent’s child list. 
		virtual ~Sprite2DExNode() ;

		// Sets a new parent with proper attach/detach management. newParent may be nullptr to set the root node with Transform in world space.
		// If newParent is the current parent, this node, or a descendent of it, the parent won't change.
		void SetParent(Sprite2DExNode* newParent) ;


		Sprite2DExNode* GetParent() const ;

		const std::vector<Sprite2DExNode*>& GetChildren() const ;

		// Returns the effective single transform in world space. 
		// Correct result is only guaranteed with no rotated non-uniform scaling in the ancestor chain (uniform scale is safe). 
		Transform2D GetGlobalTransform() const ;

		Transform2D GetGlobalPreviousTransform() const ;


		const TriangleMesh2D* Mesh; 

	private:

		Sprite2DExNode* parent_;
		std::vector<Sprite2DExNode*> children_;  

	};

}

