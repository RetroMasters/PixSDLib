#pragma once

#include <vector>
#include "PixMath.h"
#include "TargetTexture.h"
#include "SpriteMesh.h"
#include "Sprite2DEx.h"
#include "Sprite2DExNode.h"

namespace pix
{
	// TriangleMesh2DRenderer2D batches and renders TriangleMesh2D-based objects to a render target.
	//
	// Coordinate spaces:
	// - World space: X right, Y up.
	// - Logical screen space: origin (0,0) at the bottom-left of the render target,
	//   (logicalWidth, logicalHeight) at the top-right.
	// - World space maps directly to logical screen space when the camera position is (0,0)
	//   and zoom is (1,1).
	//
	// Camera:
	// - The camera is provided as a MovableObject2D in world space.
	// - Its position defines the view origin.
	// - Its scale represents the zoom vector.
	// - Its rotation affects the rendered orientation.
	//
	// Usage:
	// 1) Call BeginBatch() once per frame (or whenever configuration changes).
	// 2) Call Render(...) to submit meshes/sprites to the batch.
	// 3) Call RenderBatch() to draw all submitted geometry.
	class TriangleMesh2DRenderer2D
	{

	public:

		TriangleMesh2DRenderer2D(int initialVertexBatchSize = 100000);
		~TriangleMesh2DRenderer2D()  = default;

		// Renders a TriangleMesh2D using the specified world transform.
		// This is the most performant rendering path when the final transform
		// is already available (e.g., static meshes or externally computed transform).
		void Render(const TriangleMesh2D& mesh, const Transform2D& transform);

		// Renders a Sprite2DEx using interpolated transform state.
		// The sprite's previous and current transforms are interpolated using the interpolation factor specified in BeginBatch().
		// This is the typical rendering path for moving sprites without hierarchical parent transforms.
		void Render(const Sprite2DEx& sprite);

		// Transforms the node hierarchy to world space and renders it using interpolated transform state.
		// This is the most general rendering path but also the least performant.
		// It evaluates the Transform2D hierarchy per vertex without using
		// precomputed affine transformation matrices.
		void Render(const Sprite2DExNode& node);
 
		// Optimized variant of Render(const Sprite2DExNode&).
		// Assumes that no rotated non-uniform scaling exists in the ancestor chain
		// (rotated uniform scaling is supported). Under this constraint, hierarchical
		// evaluation can be reduced to a single composed transform, improving performance.
		void RenderFast(const Sprite2DExNode& node);

		// Clears the current batch and updates the rendering configuration.
		// After calling BeginBatch(), subsequent Render() calls append geometry to the batch, transformed according to this configuration.
		// 
		// Parameters:
		// - camera: Optional world-space camera (position/rotation/scale = zoom).
		// - interpolationAlpha: Interpolation factor in [0.0f, 1.0f] used for previous -> current transform blending.
		// - renderTargetCenter: Logical render-target coordinate that corresponds to camera-space origin.
		void BeginBatch(const MovableObject2D* camera = nullptr, float interpolationAlpha = 1.0f, Vec2f renderTargetCenter = { 0.0f, 0.0f });

		// Renders the current batch to the specified render target using the given texture.
		// If renderTarget is nullptr, rendering is performed to the default back buffer.
		// RenderBatch() may be called independently; it does not modify the batch or the configuration. The batch is cleared only by calling BeginBatch().
		// 
		// Note: 
		// Render target is renderer-global state. This function sets the render target
		// and does not restore the previous one (sticky render state contract).
		void RenderBatch(const Texture& texture, TargetTexture* renderTarget);

	private:

		struct Configuration
		{
			float InterpolationAlpha = 1.0f;
			Vec2  InterpolatedCameraPosition = {0.0, 0.0};
			Vec2f InterpolatedCameraZoom = {1.0f, 1.0f};
			Rotation2D InterpolatedCameraRotation;
			Vec2f RenderTargetCenter = {0.0f, 0.0f};
		};


		Configuration configuration_;

		std::vector<Vertex2D> vertexBatch_;

		std::vector<Vec2> pointBuffer1_;
		std::vector<Vec2> pointBuffer2_;
	};

}


