#pragma once

#include "TargetTexture.h"
#include "SpriteMesh.h"
#include "TriangleMesh2D.h"
#include "TriangleMesh3D.h"
#include "Sprite3DEx.h"
#include "Sprite3DExNode.h"

namespace pix
{
	// TriangleMeshRenderer3D batches and renders TriangleMesh2D/3D-based 3D objects to a render target.
	//
	// Coordinate spaces:
	// - World space: X right, Y forward, Z up.
	// - Logical screen space: origin (0,0) at the bottom-left of the render target,
	//   (logicalWidth, logicalHeight) at the top-right.
	//
	// Camera:
	// - The camera is provided as a MovableObject3D in world space.
	// - Its position defines the view origin.
	// - Its scale represents the zoom vector.  // TODO check that
	// - Its rotation affects the rendered orientation.
	//
	// Usage:
	// 1) Call BeginBatch() once per frame (or whenever configuration changes).
	// 2) Call Render() to submit meshes/sprites to the batch.
	// 3) Call RenderBatch() to draw all submitted geometry.
	class TriangleMeshRenderer3D
	{

	public:

		TriangleMeshRenderer3D(int initialVertexBatchSize = 100000) ;
		~TriangleMeshRenderer3D()  = default;

		// Renders a TriangleMesh2D using the specified world transform.
		// This is the most performant rendering path when the final transform
		// is already available (e.g., static meshes or externally computed transform).
		void Render(const TriangleMesh2D& mesh, const Transform3D& transform);

		// Renders a TriangleMesh3D using the specified world transform.
		// This is the most performant rendering path (next to rendering TriangleMesh2D) when the final transform
		// is already available (e.g., static meshes or externally computed transform).
		void Render(const TriangleMesh3D& mesh, const Transform3D& transform);

		// Renders a Sprite3DEx using interpolated transform state.
		// The sprite's previous and current transforms are interpolated using the interpolation factor specified in BeginBatch().
		// This is the typical rendering path for moving sprites without hierarchical parent transforms.
		void Render(const Sprite3DEx& sprite);

		// Transforms the node hierarchy to world space and renders it using interpolated transform state.
		// This is the most general rendering path but also the least performant.
		// It evaluates the Transform3D hierarchy per vertex without using precomputed affine transformation matrices.
		void Render(const Sprite3DExNode& node);

		// Optimized variant of Render(const Sprite3DExNode&).
		// Assumes that no rotated non-uniform scaling exists in the ancestor chain
		// (rotated uniform scaling is supported). Under this constraint, hierarchical
		// evaluation can be reduced to a single composed transform, improving performance.
		void RenderFast(const Sprite3DExNode& node); // TODO implement

		// Clears the current batch and updates the rendering configuration.
		// After calling BeginBatch(), subsequent Render() calls append geometry to the batch, transformed according to this configuration.
		// 
		// Parameters:
		// - camera: Optional world-space camera (position/rotation/scale = zoom).
		// - interpolationAlpha: Interpolation factor in [0.0f, 1.0f] used for previous -> current transform blending.
		// - renderTargetCenter: Logical render-target coordinate that corresponds to camera-space origin.
		// - verticalFOV is the vertical field of view.
		void BeginBatch(const MovableObject3D* camera, float interpolationAlpha, Vec2f renderTargetCenter, float verticalFOV = 60.0f);

		// Renders the current batch to the specified render target using the given texture.
		// If renderTarget is nullptr, rendering is performed to the default back buffer.
		// RenderBatch() may be called independently; it does not modify the batch or the configuration. The batch is cleared only by calling BeginBatch().
		// 
		// Note: 
		// Render target is renderer-global state. This function sets the render target
		// and does not restore the previous one (sticky render state contract).
		void RenderBatch(const Texture& boundTexture, TargetTexture* renderTarget);

	private:

		struct Configuration
		{
			float InterpolationAlpha = 1.0f;
			Vec2f RenderTargetCenter = { 0.0f , 0.0f };

			float CameraDistanceToScreen = (1080 * 0.5f) / std::tan(60.0f * 0.5f * (float)RADIANS_PER_DEGREE); //935.3f; 
			Vec3 InterpolatedCameraPosition = { 0.0 , 0.0, 0.0 };
			Rotation3D InterpolatedInversedCameraRotation;
			Vec3f InterpolatedCameraAxisZ = { 0.0f, 0.0f, 1.0f };
		};

		Configuration configuration_;

		std::vector<Vertex2D> vertexBatch_;
		std::vector<Vec3> pointBuffer1_;
		std::vector<Vec3> pointBuffer2_;

		static constexpr float minDistanceToCamera_ = 0.5f;
	};

}

