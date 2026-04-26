#pragma once

#include "TargetTexture.h"
#include "SpriteMesh.h"
#include "TriangleMesh2D.h"
#include "TriangleMesh3D.h"
#include "Sprite3DEx.h"
#include "Sprite3DExNode.h"

namespace pix
{
	// TriangleMeshRenderer3D batches and renders 3D objects based on TriangleMesh2D and TriangleMesh3D to a render target.
	// Rendered meshes must contain a valid triangle list (vertex count divisible by 3).
	// 
	// Coordinate spaces:
	// - World space: X right, Y up, -Z forward.
	// - With identity camera rotation, the camera looks along -Z.
	// - Logical render-target coordinates: top-left = (0,0), bottom-right = (logicalResolutionWidth, logicalResolutionHeight).
	//
	// Camera:
	// - The camera is provided as a MovableObject3D in world space.
	// - Its position defines the view origin.
	// - Its rotation affects the rendered orientation.
	// - Its scale property is ignored.
	//
	// Usage:
	// 1) Call BeginBatch() once per frame (or whenever configuration changes).
	// 2) Call Render() to submit meshes/sprites to the batch.
	// 3) Call RenderBatch() to draw all submitted geometry. It may be called multiple times in a row with different textures or render targets.
	//
	// Philosophy:
    // TriangleMeshRenderer3D focuses on performant rendering of the supported 3D objects.
    // Geometry is collected into a batch and submitted to the GPU in a single RenderBatch() call,
    // which greatly reduces communication overhead between the CPU and GPU.
    // This trades some convenience of immediate per-object rendering for significant performance gains.
	class TriangleMeshRenderer3D
	{

	public:

		TriangleMeshRenderer3D(int initialVertexBatchSize = 100000);
		~TriangleMeshRenderer3D()  = default;

		// Renders a TriangleMesh3D using the specified world transform.
		// This is the most performant rendering path for a TriangleMesh3D when the final transform
		// is already available (e.g., for static meshes or when the transform is computed externally).
		// Backface culling is applied. Front faces appear in clockwise vertex order on the render target.
		void Render(const TriangleMesh3D& mesh, const Transform3D& transform);
		
		// Renders a TriangleMesh2D using the specified world transform.
		// This is the most performant rendering path for a TriangleMesh2D when the final transform
		// is already available (e.g., static meshes or externally computed transform).
		void Render(const TriangleMesh2D& mesh, const Transform3D& transform);

		// Renders a Sprite3DEx using interpolated transform state.
		// The sprite's previous and current transforms are interpolated using the interpolation factor specified in BeginBatch().
		// This is the typical rendering path for moving sprites without hierarchical parent transforms.
		void Render(const Sprite3DEx& sprite);

		// Transforms node to world space and renders it using interpolated transform state.
		// This is the most general rendering path but also the least performant.
		// It evaluates the Transform3D hierarchy per vertex without using precomputed affine transformation matrices.
		void Render(const Sprite3DExNode& node);

		// Optimized variant of Render(const Sprite3DExNode&).
		// Assumes that no rotated non-uniform scaling exists in the ancestor chain
		// (rotated uniform scaling is supported). Under this constraint, hierarchical
		// evaluation can be reduced to a single composed transform, improving performance.
		void RenderFast(const Sprite3DExNode& node); 

		// Clears the current batch and updates the rendering configuration.
		// After calling BeginBatch(), subsequent Render() calls append geometry to the batch, transformed according to this configuration.
		// 
		// Parameters:
		// - camera: World-space camera used for view position and rotation.
		// - interpolationAlpha: Interpolation factor in [0.0f, 1.0f] used for previous -> current transform blending.
		// - renderTargetOffset: Point on the render target that corresponds to camera-space origin
        //   (top-left = (0,0), bottom-right = (logicalResolutionWidth, logicalResolutionHeight)).   
		// - verticalFOV: Vertical field of view in degrees for perspective projection.
		void BeginBatch(const MovableObject3D& camera, float interpolationAlpha, Vec2f renderTargetOffset, float verticalFOV = 60.0f);

		// Renders the current batch to the specified render target using the given texture.
		// If renderTarget is nullptr, rendering is performed to the default back buffer.
		// RenderBatch() may be called multiple times in a row with different textures or render targets; it does not modify the batch or the configuration. 
		// The batch is cleared only by calling BeginBatch().
		// 
		// Note: 
		// Render target is renderer-global state. This function sets the render target
		// and does not restore the previous one (sticky render state contract).
		void RenderBatch(const Texture& texture, TargetTexture* renderTarget);

	private:

		struct Configuration
		{
			Vec3 InterpolatedCameraPosition = Vec3(0.0 , 0.0, 0.0);
			Rotation3D InterpolatedCameraRotation;
			Vec3f InterpolatedCameraZAxis = Vec3f(0.0f, 0.0f, 1.0f);
			float InterpolationAlpha = 1.0f;
			Vec2f RenderTargetOffset = Vec2f(0.0f, 0.0f);

			float CameraDistanceToScreen = (1080.0f * 0.5f) / std::tan(60.0f * 0.5f * (float)RADIANS_PER_DEGREE); // = 935.3f for 1080p and verticalFOV = 60.0f
		};

		Configuration configuration_;

		std::vector<Vertex2D> vertexBatch_;
		std::vector<Vec3> worldPositionBuffer_; // Stores current world-space vertex positions
		std::vector<Vec3> prevWorldPositionBuffer_; // Stores previous world-space vertex positions

		static constexpr float MIN_DISTANCE_TO_CAMERA = 0.5f;
	};

}

