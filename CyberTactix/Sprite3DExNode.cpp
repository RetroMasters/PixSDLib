#include "Sprite3DExNode.h"

namespace pix
{

	Sprite3DExNode::Sprite3DExNode(const TriangleMesh2D* mesh, const Transform3D& transform) : MovableObject3D(transform),
		Mesh(mesh)
	{
	}

	Sprite3DExNode::Sprite3DExNode(const TriangleMesh2D* mesh, const Transform3D& transform, const Transform3D& prevTransform) : MovableObject3D(transform, prevTransform),
		Mesh(mesh)
	{
	}

	Sprite3DExNode::Sprite3DExNode(const TriangleMesh2D* mesh, const Vec3& position, Vec3f scale, const Rotation3D& rotation) : MovableObject3D(position, scale, rotation),
		Mesh(mesh)
	{
	}

	Sprite3DExNode::~Sprite3DExNode() 
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

	bool Sprite3DExNode::SetParent(Sprite3DExNode* newParent)
	{
		if (newParent == parent_) return true;

		Sprite3DExNode* currentParent = newParent;

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

		// Add to newParent
		if (newParent)
		{
			Transform3D newParentTransform = newParent->GetGlobalTransform();
			Transform3D newParentPrevTransform = newParent->GetPrevGlobalTransform();

			newParentTransform.InverseTransformPoint(Transform.Position);
			Transform.Rotation = newParentTransform.Rotation.GetLocalRotationOf(Transform.Rotation);
			Transform.Scale = GetSafeDivision(Transform.Scale, newParentTransform.Scale);
			
			newParentPrevTransform.InverseTransformPoint(prevTransform_.Position);
			prevTransform_.Rotation = newParentPrevTransform.Rotation.GetLocalRotationOf(prevTransform_.Rotation);
			prevTransform_.Scale = GetSafeDivision(prevTransform_.Scale, newParentPrevTransform.Scale);

			newParent->children_.push_back(this);
		}

		// Make the newParent known to this node
		parent_ = newParent;

		return true;
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

		Vec3 position = Transform.Position;
		Vec3f scale = Transform.Scale;
		Rotation3D rotation = Transform.Rotation;

		// Transform to world space
		while (parent)
		{
			const Transform3D& parentTransform = parent->Transform;

			scale *= parentTransform.Scale;
			rotation.AddGlobalRotation(parentTransform.Rotation);
			parentTransform.TransformPoint(position);

			parent = parent->parent_;
		}

		return Transform3D(position, scale, rotation);
	}

	Transform3D Sprite3DExNode::GetPrevGlobalTransform() const
	{
		const Sprite3DExNode* parent = parent_;

		Vec3 position = prevTransform_.Position;
		Vec3f scale = prevTransform_.Scale;
		Rotation3D rotation = prevTransform_.Rotation;


		// Transform to world space
		while (parent)
		{
			const Transform3D& prevParentTransform = parent->prevTransform_;

			scale *= prevParentTransform.Scale;
			rotation.AddGlobalRotation(prevParentTransform.Rotation);
			prevParentTransform.TransformPoint(position);

			parent = parent->parent_;
		}

		return Transform3D(position, scale, rotation);
	}

}