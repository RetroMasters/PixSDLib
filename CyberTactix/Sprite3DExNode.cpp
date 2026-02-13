#include "Sprite3DExNode.h"

namespace pix
{

	Sprite3DExNode::Sprite3DExNode() : MovableObject3D(),
		Mesh(nullptr),
		parent_(nullptr),
		children_()
	{
	}

	Sprite3DExNode::Sprite3DExNode(const TriangleMesh2D* mesh, const Transform3D& transform) : MovableObject3D(transform),
		Mesh(mesh),
		parent_(nullptr),
		children_()
	{
	}

	Sprite3DExNode::Sprite3DExNode(const TriangleMesh2D* mesh, const Transform3D& transform, const Transform3D& prevTransform) : MovableObject3D(transform, prevTransform),
		Mesh(mesh),
		parent_(nullptr),
		children_()
	{
	}

	Sprite3DExNode::Sprite3DExNode(const TriangleMesh2D* mesh, const Vector3d& position, const Vector3f& scale, const Rotation3D& rotation) : MovableObject3D(position, scale, rotation),
		Mesh(mesh),
		parent_(nullptr),
		children_()
	{
	}

	Sprite3DExNode::~Sprite3DExNode() 
	{
		// Detaching self from parent
		SetParent(nullptr);

		// Detaching self from children can be optimized since this node gets destroyed anyway
		for (size_t i = 0; i < children_.size(); i++)
		{
			children_[i]->Transform = children_[i]->GetGlobalTransform();
			children_[i]->prevTransform_ = children_[i]->GetGlobalPrevTransform();

			children_[i]->parent_ = nullptr;
		}
	}

	void Sprite3DExNode::SetParent(Sprite3DExNode* newParent)
	{
		if (newParent == parent_) return;

		// The new parent must not be this node or a descendant of it
		Sprite3DExNode* currentParent = newParent;
		while (currentParent)
		{
			if (currentParent == this) return;

			currentParent = currentParent->parent_;
		}

		// Remove from current parent
		if (parent_ != nullptr)
		{
			Transform = GetGlobalTransform();
			prevTransform_ = GetGlobalPrevTransform();

			for (int i = 0; i < parent_->children_.size(); i++)
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
			Transform3D newParentTransform = newParent->GetGlobalTransform();
			Transform3D newParentPrevTransform = newParent->GetGlobalPrevTransform();

			newParentTransform.ApplyInverseToPoint(Transform.Position);
			Transform.Scale = DivideSafe(Transform.Scale, newParentTransform.Scale);
			Transform.Rotation = newParentTransform.Rotation.ToLocalRotation(Transform.Rotation);

			newParentPrevTransform.ApplyInverseToPoint(prevTransform_.Position);
			prevTransform_.Scale = DivideSafe(prevTransform_.Scale, newParentPrevTransform.Scale);
			prevTransform_.Rotation = newParentPrevTransform.Rotation.ToLocalRotation(prevTransform_.Rotation);

			newParent->children_.push_back(this);
		}

		// Make the newParent known to this node
		parent_ = newParent;
	}


	Sprite3DExNode* Sprite3DExNode::GetParent() const
	{
		return parent_;
	}

	const std::vector<Sprite3DExNode*>& Sprite3DExNode::GetChildren() const
	{
		return children_;
	}




	Transform3D Sprite3DExNode::GetGlobalTransform() const
	{
		const Sprite3DExNode* parent = parent_;

		Vector3d   position = Transform.Position;
		Vector3f   scale = Transform.Scale;
		Rotation3D rotation = Transform.Rotation;

		// Transform to world space
		while (parent != nullptr)
		{
			const Transform3D& parentTransform = parent->Transform;

			scale.X *= parentTransform.Scale.X;
			scale.Y *= parentTransform.Scale.Y;

			rotation.AddGlobalRotation(parentTransform.Rotation);

			parentTransform.ApplyToPoint(position);
			parent = parent->parent_;
		}

		return Transform3D(position, scale, rotation);
	}

	Transform3D Sprite3DExNode::GetGlobalPrevTransform() const
	{
		const Sprite3DExNode* parent = parent_;

		Vector3d   position = prevTransform_.Position;
		Vector3f   scale = prevTransform_.Scale;
		Rotation3D rotation = prevTransform_.Rotation;


		// Transform to world space
		while (parent != nullptr)
		{
			const Transform3D& prevParentTransform = parent->prevTransform_;

			scale.X *= prevParentTransform.Scale.X;
			scale.Y *= prevParentTransform.Scale.Y;

			rotation.AddGlobalRotation(prevParentTransform.Rotation);

			prevParentTransform.ApplyToPoint(position);
			parent = parent->parent_;
		}

		return Transform3D(position, scale, rotation);
	}

}