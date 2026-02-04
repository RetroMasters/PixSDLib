#include "Sprite2DNode.h"

#include"PixMath.h"

namespace pix
{

	Sprite2DNode::Sprite2DNode()  : MoveableObject2D(),
		Mesh(nullptr),
		parent_(nullptr),
		children_()
	{
	}

	Sprite2DNode::Sprite2DNode(const SpriteMesh* mesh, const Transform2D& transform)  : MoveableObject2D(transform),
		Mesh(mesh),
		parent_(nullptr),
		children_()
	{
	}

	Sprite2DNode::Sprite2DNode(const SpriteMesh* mesh, const Transform2D& transform, const Transform2D& prevTransform)  : MoveableObject2D(transform, prevTransform),
		Mesh(mesh),
		parent_(nullptr),
		children_()
	{
	}

	Sprite2DNode::Sprite2DNode(const SpriteMesh* mesh, const Vector2d& position, const Vector2f& scale, const Rotation2D& rotation)  : MoveableObject2D(position, scale, rotation),
		Mesh(mesh),
		parent_(nullptr),
		children_()
	{
	}

	Sprite2DNode::~Sprite2DNode()  // Detach self from parent and the children to ensure they remain in a valid state
	{
		const int childrenCount = children_.size();

		SetParent(nullptr);

		for (int i = 0; i < childrenCount; i++)
		{
			children_[i]->Transform = children_[i]->GetGlobalTransform();
			children_[i]->prevTransform_ = children_[i]->GetGlobalPreviousTransform();

			children_[i]->parent_ = nullptr;
		}
	}

	void Sprite2DNode::SetParent(Sprite2DNode* newParent) 
	{
		if (newParent == parent_) return;

		if (parent_ != nullptr)
		{
			Transform = GetGlobalTransform();
			prevTransform_ = GetGlobalPreviousTransform();

			//const Sprite2DNode* const thisNode = this; // To prevent possible further casts of this-pointer to Sprite2DNode

			// Remove from current Parent:
			for (int i = 0; i < parent_->children_.size(); i++)
			{
				if (parent_->children_[i] == this)
				{
					parent_->children_.erase(parent_->children_.begin() + i);
					break;
				}
			}
		}

		// Add to newParent:
		if (newParent != nullptr)
		{
			const Transform2D newParentTransform = newParent->GetGlobalTransform();
			const Transform2D newParentPrevTransform = newParent->GetGlobalPreviousTransform();

			Transform.Position -= newParentTransform.Position;
			Transform.Rotation.AddRotation(newParentTransform.Rotation.GetInverse());
			Transform.Scale = DivideSafe(Transform.Scale, newParentTransform.Scale);

			prevTransform_.Position -= newParentPrevTransform.Position;
			prevTransform_.Rotation.AddRotation(newParentPrevTransform.Rotation.GetInverse());
			prevTransform_.Scale = DivideSafe(prevTransform_.Scale, newParentPrevTransform.Scale);

			newParent->children_.push_back(this);
		}

		// Make the newParent known to this node:
		parent_ = newParent;
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

		Vector2d   position = Transform.Position;
		Vector2f   scale = Transform.Scale;
		Rotation2D rotation = Transform.Rotation;

		// Transform to world space:
		while (parent != nullptr)
		{
			scale *= parent->Transform.Scale;
			rotation.AddRotation(parent->Transform.Rotation);
			parent->Transform.ApplyToPoint(position);

			parent = parent->parent_;
		}

		return Transform2D(position, scale, rotation);
	}

	Transform2D Sprite2DNode::GetGlobalPreviousTransform() const 
	{
		const Sprite2DNode* parent = parent_;

		Vector2d   position = prevTransform_.Position;
		Vector2f   scale = prevTransform_.Scale;
		Rotation2D rotation = prevTransform_.Rotation;

		// Transform to world space:
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