#include "Sprite2DExNode.h"

#include "PixMath.h"

namespace pix
{

	Sprite2DExNode::Sprite2DExNode()  : MovableObject2D(),
		Mesh(nullptr),
		parent_(nullptr),
		children_()
	{
	}

	Sprite2DExNode::Sprite2DExNode(const TriangleMesh2D* mesh, const Transform2D& transform)  : MovableObject2D(transform),
		Mesh(mesh),
		parent_(nullptr),
		children_()
	{
	}

	Sprite2DExNode::Sprite2DExNode(const TriangleMesh2D* mesh, const Transform2D& transform, const Transform2D& prevTransform)  : MovableObject2D(transform, prevTransform),
		Mesh(mesh),
		parent_(nullptr),
		children_()
	{
	}

	Sprite2DExNode::Sprite2DExNode(const TriangleMesh2D* mesh, const Vector2d& position, const Vector2f& scale, const Rotation2D& rotation)  : MovableObject2D(position, scale, rotation),
		Mesh(mesh),
		parent_(nullptr),
		children_()
	{
	}

	Sprite2DExNode::~Sprite2DExNode() 
	{
		// Detach self from parent
		SetParent(nullptr);

		// Detaching self from children can be optimized since this node gets destroyed anyway
		for (size_t i = 0; i < children_.size(); i++)
		{
			children_[i]->Transform = children_[i]->GetGlobalTransform();
			children_[i]->prevTransform_ = children_[i]->GetGlobalPreviousTransform();
			children_[i]->parent_ = nullptr;
		}
	}

	void Sprite2DExNode::SetParent(Sprite2DExNode* newParent) 
	{
		if (newParent == parent_) return;

		// The new parent must not be this node or a descendant of it
		Sprite2DExNode* currentParent = newParent;
		while (currentParent)
		{
			if (currentParent == this) return;

			currentParent = currentParent->parent_;
		}

		// Remove from current parent
		if (parent_ != nullptr)
		{
			Transform = GetGlobalTransform();
			prevTransform_ = GetGlobalPreviousTransform();

			// Remove from current Parent
			for (size_t i = 0; i < parent_->children_.size(); i++)
			{
				if (parent_->children_[i] == this)
				{
					parent_->children_.erase(parent_->children_.begin() + i);
					break;
				}
			}
		}

		// Add to newParent
		if (newParent != nullptr)
		{
			Transform2D newParentTransform = newParent->GetGlobalTransform();
			Transform2D newParentPrevTransform = newParent->GetGlobalPreviousTransform();

			newParentTransform.ApplyInverseToPoint(Transform.Position);          
			Transform.Rotation.AddRotation(newParentTransform.Rotation.GetInverse());
			Transform.Scale = DivideSafe(Transform.Scale, newParentTransform.Scale);

			newParentPrevTransform.ApplyInverseToPoint(prevTransform_.Position);
			prevTransform_.Rotation.AddRotation(newParentPrevTransform.Rotation.GetInverse());
			prevTransform_.Scale = DivideSafe(prevTransform_.Scale, newParentPrevTransform.Scale);

			newParent->children_.push_back(this);
		}

		// Make newParent known to this node
		parent_ = newParent;
	}


	Sprite2DExNode* Sprite2DExNode::GetParent() const 
	{
		return parent_;
	}

	const std::vector<Sprite2DExNode*>& Sprite2DExNode::GetChildren() const 
	{
		return children_;
	}


	Transform2D Sprite2DExNode::GetGlobalTransform() const 
	{
		const Sprite2DExNode* parent = parent_;

		Vector2d   position = Transform.Position;
		Vector2f   scale = Transform.Scale;
		Rotation2D rotation = Transform.Rotation;

		// Transform to world space
		while (parent != nullptr)
		{
			scale *= parent->Transform.Scale;
			rotation.AddRotation(parent->Transform.Rotation);
			parent->Transform.ApplyToPoint(position);

			parent = parent->parent_;
		}

		return Transform2D(position, scale, rotation);
	}

	Transform2D Sprite2DExNode::GetGlobalPreviousTransform() const 
	{
		const Sprite2DExNode* parent = parent_;

		Vector2d   position = prevTransform_.Position;
		Vector2f   scale = prevTransform_.Scale;
		Rotation2D rotation = prevTransform_.Rotation;

		// Transform to world space
		while (parent != nullptr)
		{
			scale *= parent->prevTransform_.Scale;
			rotation.AddRotation(parent->prevTransform_.Rotation);
			parent->prevTransform_.ApplyToPoint(position);

			parent = parent->parent_;
		}

		return Transform2D(position, scale, rotation);
	}

}