#include "Sprite2DNode.h"

namespace pix
{
	Sprite2DNode::Sprite2DNode(const SpriteMesh* mesh, const Transform2D& transform) : MovableObject2D(transform),
		Mesh(mesh)
	{
	}

	Sprite2DNode::Sprite2DNode(const SpriteMesh* mesh, const Transform2D& transform, const Transform2D& prevTransform) : MovableObject2D(transform, prevTransform),
		Mesh(mesh)
	{
	}

	Sprite2DNode::Sprite2DNode(const SpriteMesh* mesh, Vec2 position, Vec2f scale, Rotation2D rotation) : MovableObject2D(position, scale, rotation),
		Mesh(mesh)
	{
	}

	Sprite2DNode::~Sprite2DNode()  
	{
		// Detach self from parent
		SetParent(nullptr);

		const int childCount = children_.size();
		// Re-root children directly because this node is being destroyed
		for (int i = 0; i < childCount; i++) 
		{
			children_[i]->Transform = children_[i]->GetGlobalTransform();
			children_[i]->prevTransform_ = children_[i]->GetPrevGlobalTransform();
			children_[i]->parent_ = nullptr;
		}
	}

	bool Sprite2DNode::SetParent(Sprite2DNode* newParent) 
	{
		if (newParent == parent_) return true;

		Sprite2DNode* currentParent = newParent; 
		// The new parent must not be this node or a descendant of it
		while (currentParent)
		{
			if (currentParent == this) return false;

			currentParent = currentParent->parent_;
		}

		// Remove from current parent
		if (parent_)
		{
			Transform = GetGlobalTransform();
			prevTransform_ = GetPrevGlobalTransform();

			const int parentChildCount = parent_->children_.size();

			for (int i = 0; i < parentChildCount; i++)
			{
				if (parent_->children_[i] == this)
				{
					parent_->children_.erase(parent_->children_.begin() + i);
					break;
				}
			}
		}

		// Add to new parent
		if (newParent)
		{
			Transform2D newParentTransform = newParent->GetGlobalTransform();
			Transform2D newParentPrevTransform = newParent->GetPrevGlobalTransform();

			newParentTransform.InverseTransformPoint(Transform.Position);
			Transform.Rotation.AddRotation(newParentTransform.Rotation.GetInverse());
			Transform.Scale = GetSafeDivision(Transform.Scale, newParentTransform.Scale);

			newParentPrevTransform.InverseTransformPoint(prevTransform_.Position);
			prevTransform_.Rotation.AddRotation(newParentPrevTransform.Rotation.GetInverse());
			prevTransform_.Scale = GetSafeDivision(prevTransform_.Scale, newParentPrevTransform.Scale);

			newParent->children_.push_back(this);
		}

		// Make newParent known to this node
		parent_ = newParent;

		return true;
	}

	Sprite2DNode* Sprite2DNode::GetParent() const 
	{
		return parent_;
	}

	const std::vector<Sprite2DNode*>& Sprite2DNode::GetChildren() const 
	{
		return children_;
	}

	Transform2D Sprite2DNode::GetGlobalTransform() const 
	{
		const Sprite2DNode* parent = parent_;

		Vec2   position = Transform.Position;
		Vec2f   scale = Transform.Scale;
		Rotation2D rotation = Transform.Rotation;

		// Transform to world space
		while (parent)
		{
			scale *= parent->Transform.Scale;
			rotation.AddRotation(parent->Transform.Rotation);
			parent->Transform.TransformPoint(position);

			parent = parent->parent_;
		}

		return Transform2D(position, scale, rotation);
	}

	Transform2D Sprite2DNode::GetPrevGlobalTransform() const 
	{
		const Sprite2DNode* parent = parent_;

		Vec2   position = prevTransform_.Position;
		Vec2f   scale = prevTransform_.Scale;
		Rotation2D rotation = prevTransform_.Rotation;

		// Transform to world space
		while (parent)
		{
			scale *= parent->prevTransform_.Scale;
			rotation.AddRotation(parent->prevTransform_.Rotation);
			parent->prevTransform_.TransformPoint(position);

			parent = parent->parent_;
		}

		return Transform2D(position, scale, rotation);
	}

}