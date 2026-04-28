#pragma once

#include <vector>
#include <cmath>
#include "PixMath.h"
#include "MovableObject3D.h"
#include "Texture.h"
#include "TargetTexture.h"
#include "SpriteMesh.h"
#include "Sprite3D.h"
#include "Sprite3DNode.h"

namespace pix
{
	// SpriteMeshRenderer3D batches and renders 3D objects based on SpriteMesh to a render target.
	// 
	// Coordinate spaces:
	// - World space: X right, Y up, -Z forward.
	// - With identity camera rotation, the camera looks along -Z.
	// - Logical render-target coordinates: origin = (0, 0) at the top-left of the render target, and (logicalResolutionWidth, logicalResolutionHeight) at the bottom-right.
	//
	// Camera:
	// - The camera is provided as a MovableObject3D in world space.
	// - Its position defines the view origin.
	// - Its rotation affects the rendered orientation.
	// - Its scale property is ignored.
	//
	// Usage:
	// 1) Call BeginBatch() once per frame (or whenever configuration changes).
	// 2) Call the render methods to submit geometry to the batch.
	// 3) Call RenderBatch() to draw all submitted geometry. It may be called multiple times in a row with different textures or render targets.
	//
	// Philosophy:
	// SpriteMeshRenderer3D focuses on performant rendering of the supported 3D objects.
	// Geometry is collected into a batch and submitted to the GPU through RenderBatch(),
	// which greatly reduces communication overhead between the CPU and GPU.
	// This trades some convenience of immediate per-object rendering for significant performance gains.
	class SpriteMeshRenderer3D
	{

	public:

		SpriteMeshRenderer3D(int initialVertexBatchCapacity = 100000);
	    ~SpriteMeshRenderer3D() = default;

		// Renders a SpriteMesh using the specified world transform.
		// This is the most performant rendering path for a SpriteMesh when the final transform
		// is already available (e.g., for static meshes or externally computed transform).
		void Render(const SpriteMesh& mesh, const Transform3D& transform);

		// Renders a Sprite3D using interpolated transform state.
		// The sprite's previous and current transforms are interpolated using the interpolation factor specified in BeginBatch().
		// This is the typical rendering path for moving sprites without hierarchical parent transforms.
		void Render(const Sprite3D& sprite);

		// Transforms a Sprite3DNode to world space and renders it using interpolated transform state.
		// This is the most general rendering path but also the least performant.
		// It evaluates the Transform3D hierarchy per vertex without using precomputed affine transformation matrices.
		void Render(const Sprite3DNode& node);

		// Optimized variant of Render(const Sprite3DNode&).
		// Assumes that no rotated non-uniform scaling exists in the ancestor chain
		// (rotated uniform scaling is supported). Under this constraint, hierarchical
		// evaluation can be reduced to a single composed transform, improving performance.
		void RenderFast(const Sprite3DNode& node);

		// Renders a line between the world-space positions startPoint and endPoint by stretching the mesh along the segment.
		// lineWidth is specified in logical render-target units. Fractional values are supported and influence rasterization/rounding.
		// If lineWidth is negative (not intended), the generated corner ordering is flipped.
		// The line segment runs through the center of the generated quad.
		// A line segment with camera-space depth greater than -NEAR_CLIP_DISTANCE is clipped.
		void RenderLine(const SpriteMesh& mesh, const Vec3& startPoint, const Vec3& endPoint, float lineWidth);

		// Renders a point at the specified world-space position using a quad centered on that position.
        // pointSize is specified in logical render-target units and defines the size of the generated quad.
		// Fractional values are supported and influence rasterization/rounding.
		// If pointSize is negative (not intended), the generated corner ordering is flipped.
		void RenderPoint(const SpriteMesh& mesh, const Vec3& point, float pointSize);

		// Clears the current batch and updates the rendering configuration.
		// After calling BeginBatch(), subsequent render calls append geometry to the batch, transformed according to this configuration.
		// 
		// Parameters:
		// - camera: World-space camera used for view position and rotation.
		// - renderTargetOffset: Logical render-target coordinate that corresponds to camera-space origin.
		//   (top-left = (0,0), bottom-right = (logicalResolutionWidth, logicalResolutionHeight)).  
		// - interpolationAlpha: Interpolation factor used for previous -> current transform blending (internally clamped to [0.0f, 1.0f]).
		// - verticalFOV: Vertical field of view in degrees for perspective projection.
		void BeginBatch(const MovableObject3D& camera, Vec2f renderTargetOffset, float interpolationAlpha= 1.0f, float verticalFOV = 60.0f);

		// Renders the current batch to the specified render target using the given texture.
		// If renderTarget is nullptr, rendering is performed to the default back buffer.
		// RenderBatch() may be called multiple times in a row with different textures or render targets; it does not modify the batch or the configuration. 
		// The batch is cleared only by calling BeginBatch().
		// 
		// Note: 
		// Render target is renderer-global state. This function sets the render target
		// and does not restore the previous one.
		void RenderBatch(const Texture& texture, TargetTexture* renderTarget);


	private:

		// The near clip plane is at z = -NEAR_CLIP_DISTANCE. 
		// The constant must be greater than zero to avoid projection at z = 0.
		// Geometry with camera-space depth greater than -NEAR_CLIP_DISTANCE is clipped (lines) or discarded (triangles).
		static constexpr float NEAR_CLIP_DISTANCE = 0.5f;

		struct Configuration
		{
			Vec3 InterpolatedCameraPosition = Vec3(0.0, 0.0, 0.0);
			Rotation3D InterpolatedCameraRotation;
			Vec3f InterpolatedCameraZAxis = Vec3f(0.0f, 0.0f, 1.0f);
			float InterpolationAlpha = 1.0f;
			Vec2f RenderTargetOffset = Vec2f(0.0f, 0.0f);

			float CameraDistanceToScreen = (1080.0f * 0.5f) / std::tan(60.0f * 0.5f * (float)RADIANS_PER_DEGREE); // = 935.3f for 1080p and verticalFOV = 60.0f
		};


		void UpdateVertexIndices();

		Configuration configuration_;

		std::vector<Vertex2D> vertexBatch_;
		std::vector<int> vertexIndices_;
	};

}
