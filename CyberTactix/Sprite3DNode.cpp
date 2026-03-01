#include "Sprite3DNode.h"

namespace pix
{

	Sprite3DNode::Sprite3DNode() : MovableObject3D(),
		Mesh(nullptr),
		parent_(nullptr),
		children_()
	{
	}

	Sprite3DNode::Sprite3DNode(const SpriteMesh* mesh, const Transform3D& transform) : MovableObject3D(transform),
		Mesh(mesh),
		parent_(nullptr),
		children_()
	{
	}

	Sprite3DNode::Sprite3DNode(const SpriteMesh* mesh, const Transform3D& transform, const Transform3D& prevTransform) : MovableObject3D(transform, prevTransform),
		Mesh(mesh),
		parent_(nullptr),
		children_()
	{
	}

	Sprite3DNode::Sprite3DNode(const SpriteMesh* mesh, const Vec3& position, const Vec3f& scale, const Rotation3D& rotation) : MovableObject3D(position, scale, rotation),
		Mesh(mesh),
		parent_(nullptr),
		children_()
	{
	}

	Sprite3DNode::~Sprite3DNode() // Detach self from parent and children to ensure they remain in a valid state
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



	void Sprite3DNode::SetParent(Sprite3DNode* newParent)
	{
		if (newParent == parent_) return;

		// The new parent must not be this node or a descendant of it
		Sprite3DNode* currentParent = newParent;
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
			Transform3D newParentTransform = newParent->GetGlobalTransform();
			Transform3D newParentPrevTransform = newParent->GetGlobalPrevTransform();

			newParentTransform.InverseTransformPoint(Transform.Position);
			//Transform.Rotation.AddGlobalRotation(newParentTransform.Rotation.GetInverse());
			Transform.Rotation = newParentTransform.Rotation.GetLocalRotationOf(Transform.Rotation);
			Transform.Scale = GetSafeDivision(Transform.Scale, newParentTransform.Scale);
			
			newParentPrevTransform.InverseTransformPoint(prevTransform_.Position);
			//prevTransform_.Rotation.AddGlobalRotation(newParentPrevTransform.Rotation.GetInverse());
			prevTransform_.Rotation = newParentPrevTransform.Rotation.GetLocalRotationOf(prevTransform_.Rotation);
			prevTransform_.Scale = GetSafeDivision(prevTransform_.Scale, newParentPrevTransform.Scale);
		
			newParent->children_.push_back(this);
		}

		// Make the newParent known to this node
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

		Vec3   position = Transform.Position;
		Vec3f   scale = Transform.Scale;
		Rotation3D rotation = Transform.Rotation;

		// Transform to world space
		while (parent != nullptr)
		{
			const Transform3D& parentTransform = parent->Transform;

			scale *= parentTransform.Scale;
			rotation.AddGlobalRotation(parentTransform.Rotation);
			parentTransform.TransformPoint(position);

			parent = parent->parent_;
		}

		return Transform3D(position, scale, rotation);
	}

	Transform3D Sprite3DNode::GetGlobalPrevTransform() const
	{
		const Sprite3DNode* parent = parent_;

		Vec3   position = prevTransform_.Position;
		Vec3f   scale = prevTransform_.Scale;
		Rotation3D rotation = prevTransform_.Rotation;


		// Transform to world space
		while (parent != nullptr)
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
