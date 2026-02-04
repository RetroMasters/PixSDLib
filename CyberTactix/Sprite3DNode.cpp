#include "Sprite3DNode.h"

#include"PixMath.h"

namespace pix
{

	Sprite3DNode::Sprite3DNode() : MoveableObject3D(),
		Mesh(nullptr),
		parent_(nullptr),
		children_()
	{
	}

	Sprite3DNode::Sprite3DNode(const SpriteMesh* mesh, const Transform3D& transform) : MoveableObject3D(transform),
		Mesh(mesh),
		parent_(nullptr),
		children_()
	{
	}

	Sprite3DNode::Sprite3DNode(const SpriteMesh* mesh, const Transform3D& transform, const Transform3D& prevTransform) : MoveableObject3D(transform, prevTransform),
		Mesh(mesh),
		parent_(nullptr),
		children_()
	{
	}

	Sprite3DNode::Sprite3DNode(const SpriteMesh* mesh, const Vector3d& position, const Vector3f& scale, const Rotation3D& rotation) : MoveableObject3D(position, scale, rotation),
		Mesh(mesh),
		parent_(nullptr),
		children_()
	{
	}

	Sprite3DNode::~Sprite3DNode() // Detach self from parent and children to ensure they remain in a valid state
	{
		const int childrenCount = children_.size();

		SetParent(nullptr);

		for (int i = 0; i < childrenCount; i++)
		{
			children_[i]->Transform = children_[i]->GetGlobalTransform();
			children_[i]->prevTransform_ = children_[i]->GetGlobalPrevTransform();

			children_[i]->parent_ = nullptr;
		}
	}

	void Sprite3DNode::SetParent(Sprite3DNode* newParent)
	{
		if (newParent == parent_) return;

		if (parent_ != nullptr)
		{
			Transform = GetGlobalTransform();
			prevTransform_ = GetGlobalPrevTransform();

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
			Transform3D newParentTransform = newParent->GetGlobalTransform();
			Transform3D newParentPrevTransform = newParent->GetGlobalPrevTransform();

			Vector3d position = Transform.Position - newParentTransform.Position;
			Vector3f scale = DivideSafe(Transform.Scale, newParentTransform.Scale);
			Transform = Transform3D(position, scale, newParentTransform.Rotation.ToLocalRotation(Transform.Rotation));

			position = prevTransform_.Position - newParentPrevTransform.Position;
			scale = DivideSafe(prevTransform_.Scale, newParentPrevTransform.Scale);
			prevTransform_ = Transform3D(position, scale, newParentPrevTransform.Rotation.ToLocalRotation(prevTransform_.Rotation));

			newParent->children_.push_back(this);
		}

		// Make the newParent known to this node:
		parent_ = newParent;
	}


	Sprite3DNode* Sprite3DNode::GetParent() const
	{
		return parent_;
	}

	const std::vector<Sprite3DNode*>& Sprite3DNode::GetChildren() const
	{
		return children_;
	}




	Transform3D Sprite3DNode::GetGlobalTransform() const
	{
		const Sprite3DNode* parent = parent_;

		Vector3d   position = Transform.Position;
		Vector3f   scale = Transform.Scale;
		Rotation3D rotation = Transform.Rotation;

		// Transform to world space:
		while (parent != nullptr)
		{
			const Transform3D& parentTransform = parent->Transform;

			scale *= parentTransform.Scale;
			rotation.AddGlobalRotation(parentTransform.Rotation);
			parentTransform.ApplyToPoint(position);

			parent = parent->parent_;
		}

		return Transform3D(position, scale, rotation);
	}

	Transform3D Sprite3DNode::GetGlobalPrevTransform() const
	{
		const Sprite3DNode* parent = parent_;

		Vector3d   position = prevTransform_.Position;
		Vector3f   scale = prevTransform_.Scale;
		Rotation3D rotation = prevTransform_.Rotation;


		// Transform to world space:
		while (parent != nullptr)
		{
			const Transform3D& prevParentTransform = parent->prevTransform_;

			scale *= prevParentTransform.Scale;
			rotation.AddGlobalRotation(prevParentTransform.Rotation);
			prevParentTransform.ApplyToPoint(position);

			parent = parent->parent_;
		}

		return Transform3D(position, scale, rotation);
	}

}
