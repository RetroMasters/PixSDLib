#include "SpriteMeshRenderer3D.h"
#include "Renderer.h"

namespace pix
{

	SpriteMeshRenderer3D::SpriteMeshRenderer3D(int initialVertexBatchCapacity)
	{
		if (initialVertexBatchCapacity > 0)
		{
			vertexBatch_.reserve(initialVertexBatchCapacity);
			vertexIndices_.reserve((initialVertexBatchCapacity * 3) / 2);
		}
	}



	void SpriteMeshRenderer3D::Render(const SpriteMesh& mesh, const Transform3D& transform) 
	{
		const Vertex2D* const vertices = mesh.Vertices;

		// Precompute the transform's combined scale and rotation for per-vertex use (flat mesh: Z can be ignored)
		const Vec3f scaledXAxis = transform.Rotation.GetXAxis() * transform.Scale.X;
		const Vec3f scaledYAxis = transform.Rotation.GetYAxis() * transform.Scale.Y;

		// World-space vector from camera to object origin (float precision is sufficient in camera-relative space)
		const Vec3f originOffset(transform.Position - configuration_.InterpolatedCameraPosition);

		for (int i = 0; i < 4; i++)
		{
			Vec3f vertexPosition(vertices[i].Position.X, vertices[i].Position.Y, 0.0f);

			// Scale and rotate the vertex position (flat mesh: Z can be ignored)
			vertexPosition = (scaledXAxis * vertexPosition.X) + (scaledYAxis * vertexPosition.Y); 

			// Translate the vertex position by origin offset
			vertexPosition += originOffset;

			// Transform the vertex position to camera space or discard this mesh
			const float z = configuration_.InterpolatedCameraZAxis.GetDotProduct(vertexPosition);
			if (z > -NEAR_CLIP_DISTANCE)
			{
				// Remove already added vertices of the current quad from the batch
				for (int j = 0; j < i; j++) 
				  vertexBatch_.pop_back();

				return;
			}
			const float x = configuration_.InterpolatedCameraRotation.GetXAxis().GetDotProduct(vertexPosition);
			const float y = configuration_.InterpolatedCameraRotation.GetYAxis().GetDotProduct(vertexPosition);

			// Project the vertex position to logical render-target coordinates (Y increases downward)
			Vec2f renderTargetCoords(x * configuration_.CameraDistanceToScreen / (-z), y * configuration_.CameraDistanceToScreen / z);
			renderTargetCoords += configuration_.RenderTargetOffset;

			// Add the transformed vertex to the batch
			vertexBatch_.emplace_back(renderTargetCoords, vertices[i].Color, vertices[i].UV);
		}
	}



	void SpriteMeshRenderer3D::Render(const Sprite3D& sprite) 
	{
		if (!sprite.Mesh) return;

		const Vertex2D* const vertices = sprite.Mesh->Vertices;

		// Interpolate the sprite transform
		Transform3D interpolatedTransform = GetInterpolated(sprite.GetPreviousTransform(), sprite.Transform, configuration_.InterpolationAlpha);

		// Precompute the interpolated transform's combined scale and rotation for per-vertex use (flat mesh: Z can be ignored)
		const Vec3f scaledXAxis = interpolatedTransform.Rotation.GetXAxis() * interpolatedTransform.Scale.X;
		const Vec3f scaledYAxis = interpolatedTransform.Rotation.GetYAxis() * interpolatedTransform.Scale.Y;

		// World-space vector from camera to object origin (float precision is sufficient in camera-relative space)
		const Vec3f originOffset(interpolatedTransform.Position - configuration_.InterpolatedCameraPosition);

		for (int i = 0; i < 4; i++)
		{
			Vec3f vertexPosition(vertices[i].Position.X, vertices[i].Position.Y, 0.0f);

			// Scale and rotate the vertex position (flat mesh: Z can be ignored)
			vertexPosition = (scaledXAxis * vertexPosition.X) + (scaledYAxis * vertexPosition.Y); 

			// Translate the vertex position by origin offset
			vertexPosition += originOffset;

			// Transform the vertex position to camera space or discard this sprite
			const float z = configuration_.InterpolatedCameraZAxis.GetDotProduct(vertexPosition);
			if (z > -NEAR_CLIP_DISTANCE)
			{
				// Remove already added vertices of the current quad from the batch
				for (int j = 0; j < i; j++) 
				   vertexBatch_.pop_back();

				return;
			}
			const float x = configuration_.InterpolatedCameraRotation.GetXAxis().GetDotProduct(vertexPosition);
			const float y = configuration_.InterpolatedCameraRotation.GetYAxis().GetDotProduct(vertexPosition);

			// Project the vertex position to logical render-target coordinates (Y increases downward)
			Vec2f renderTargetCoords(x * configuration_.CameraDistanceToScreen / (-z), y * configuration_.CameraDistanceToScreen / z);
			renderTargetCoords += configuration_.RenderTargetOffset;

			// Add the transformed vertex to the batch
			vertexBatch_.emplace_back(renderTargetCoords, vertices[i].Color, vertices[i].UV);
		}
	}



	void SpriteMeshRenderer3D::Render(const Sprite3DNode& node) 
	{
		if (!node.Mesh) return;

		const Vertex2D* const vertices = node.Mesh->Vertices;
		const Sprite3DNode* parent = &node;
		const double interpolationAlpha = configuration_.InterpolationAlpha; // Convert to double for repeated use

		// Cache initial vertex positions
		Vec3 worldPositionBuffer[4] = 
		{ 
		  Vec3(vertices[0].Position.X, vertices[0].Position.Y, 0.0),
		  Vec3(vertices[1].Position.X, vertices[1].Position.Y, 0.0),
		  Vec3(vertices[2].Position.X, vertices[2].Position.Y, 0.0),
		  Vec3(vertices[3].Position.X, vertices[3].Position.Y, 0.0) 
		};
		
		Vec3 prevWorldPositionBuffer[4] = { worldPositionBuffer[0], worldPositionBuffer[1], worldPositionBuffer[2], worldPositionBuffer[3] };
		
		// Transform current and previous vertex positions to world space
		while (parent)
		{
			parent->Transform.TransformPoints(worldPositionBuffer, 4);
			parent->GetPreviousTransform().TransformPoints(prevWorldPositionBuffer, 4);
			parent = parent->GetParent();
		}

		for (int i = 0; i < 4; i++)
		{
			// Interpolate the world-space vertex position
			worldPositionBuffer[i] = GetInterpolatedUnchecked(prevWorldPositionBuffer[i], worldPositionBuffer[i], interpolationAlpha);

			// World-space vector from camera to vertex (float precision is sufficient in camera-relative space)
			const Vec3f cameraToVertex(worldPositionBuffer[i] - configuration_.InterpolatedCameraPosition);

			// Transform the vertex position to camera space or discard this sprite node
			const float z = configuration_.InterpolatedCameraZAxis.GetDotProduct(cameraToVertex);
			if (z > -NEAR_CLIP_DISTANCE)
			{
				// Remove already added vertices of the current quad from the batch
				for (int j = 0; j < i; j++)
					vertexBatch_.pop_back();

				return;
			}
			const float x = configuration_.InterpolatedCameraRotation.GetXAxis().GetDotProduct(cameraToVertex);
			const float y = configuration_.InterpolatedCameraRotation.GetYAxis().GetDotProduct(cameraToVertex);

			// Project the vertex position to logical render-target coordinates (Y increases downward)
			Vec2f renderTargetCoords(x * configuration_.CameraDistanceToScreen / (-z), y * configuration_.CameraDistanceToScreen / z);
			renderTargetCoords += configuration_.RenderTargetOffset;

			// Add the transformed vertex to the batch
			vertexBatch_.emplace_back(renderTargetCoords, vertices[i].Color, vertices[i].UV);
		}
	}



	void SpriteMeshRenderer3D::RenderFast(const Sprite3DNode& node)
	{
		if (!node.Mesh) return;

		const Vertex2D* const vertices = node.Mesh->Vertices;

		Transform3D prevTransform = node.GetPrevGlobalTransform();

		// Interpolate the global node transform
		Transform3D interpolatedTransform = node.GetGlobalTransform();
		interpolatedTransform = GetInterpolated(prevTransform, interpolatedTransform, configuration_.InterpolationAlpha);

		// Precompute the interpolated transform's combined scale and rotation for per-vertex use (flat mesh: Z can be ignored)
		const Vec3f scaledXAxis = interpolatedTransform.Rotation.GetXAxis() * interpolatedTransform.Scale.X;
		const Vec3f scaledYAxis = interpolatedTransform.Rotation.GetYAxis() * interpolatedTransform.Scale.Y;

		// World-space vector from camera to object origin (float precision is sufficient in camera-relative space)
		const Vec3f originOffset(interpolatedTransform.Position - configuration_.InterpolatedCameraPosition);

		for (int i = 0; i < 4; i++)
		{
			Vec3f vertexPosition(vertices[i].Position.X, vertices[i].Position.Y, 0.0f);

			// Scale and rotate the vertex position (flat mesh: Z can be ignored)
			vertexPosition = (scaledXAxis * vertexPosition.X) + (scaledYAxis * vertexPosition.Y);

			// Translate the vertex position by origin offset
			vertexPosition += originOffset;

			// Transform the vertex position to camera space or discard this sprite node
			const float z = configuration_.InterpolatedCameraZAxis.GetDotProduct(vertexPosition);
			if (z > -NEAR_CLIP_DISTANCE)
			{
				// Remove already added vertices of the current quad from the batch
				for (int j = 0; j < i; j++)
					vertexBatch_.pop_back();

				return;
			}
			const float x = configuration_.InterpolatedCameraRotation.GetXAxis().GetDotProduct(vertexPosition);
			const float y = configuration_.InterpolatedCameraRotation.GetYAxis().GetDotProduct(vertexPosition);

			// Project the vertex position to logical render-target coordinates (Y increases downward)
			Vec2f renderTargetCoords(x * configuration_.CameraDistanceToScreen / (-z), y * configuration_.CameraDistanceToScreen / z);
			renderTargetCoords += configuration_.RenderTargetOffset;

			// Add the transformed vertex to the batch
			vertexBatch_.emplace_back(renderTargetCoords, vertices[i].Color, vertices[i].UV);
		}
	}



	void SpriteMeshRenderer3D::RenderLine(const SpriteMesh& mesh, const Vec3& startPoint, const Vec3& endPoint, float lineWidth) 
	{
		const Vertex2D* const vertices = mesh.Vertices;

		// World-space vector from camera to startPoint/endPoint (float precision is sufficient in camera-relative space)
		Vec3f cameraToStartPoint(startPoint - configuration_.InterpolatedCameraPosition);
		Vec3f cameraToEndPoint(endPoint - configuration_.InterpolatedCameraPosition);

		// Transform the line points to camera space
		float x1 = configuration_.InterpolatedCameraRotation.GetXAxis().GetDotProduct(cameraToStartPoint);
		float y1 = configuration_.InterpolatedCameraRotation.GetYAxis().GetDotProduct(cameraToStartPoint);
		float z1 = configuration_.InterpolatedCameraZAxis.GetDotProduct(cameraToStartPoint);

		float x2 = configuration_.InterpolatedCameraRotation.GetXAxis().GetDotProduct(cameraToEndPoint);
		float y2 = configuration_.InterpolatedCameraRotation.GetYAxis().GetDotProduct(cameraToEndPoint);
		float z2 = configuration_.InterpolatedCameraZAxis.GetDotProduct(cameraToEndPoint);

		if (z1 > -NEAR_CLIP_DISTANCE && z2 > -NEAR_CLIP_DISTANCE) // Discard lines behind the near clip plane
			return;

		// Clip the line segment behind the near clip plane
		if (z1 <= -NEAR_CLIP_DISTANCE && z2 > -NEAR_CLIP_DISTANCE)
		{
			const float t = (-NEAR_CLIP_DISTANCE - z1) / (z2 - z1);

			x2 = x1 + t * (x2 - x1);
			y2 = y1 + t * (y2 - y1);
			z2 = -NEAR_CLIP_DISTANCE;
		}
		else if (z2 <= -NEAR_CLIP_DISTANCE && z1 > -NEAR_CLIP_DISTANCE)
		{
			const float t = (-NEAR_CLIP_DISTANCE - z2) / (z1 - z2);

			x1 = x2 + t * (x1 - x2);
			y1 = y2 + t * (y1 - y2);
			z1 = -NEAR_CLIP_DISTANCE;
		}

		// Project the line points to logical render-target coordinates (Y increases downward)
		Vec2f startPointRenderTargetCoords(x1 * configuration_.CameraDistanceToScreen / (-z1), y1 * configuration_.CameraDistanceToScreen / z1);
		startPointRenderTargetCoords += configuration_.RenderTargetOffset;

		Vec2f endPointRenderTargetCoords(x2 * configuration_.CameraDistanceToScreen / (-z2), y2 * configuration_.CameraDistanceToScreen / z2);
		endPointRenderTargetCoords += configuration_.RenderTargetOffset;

		const Vec2f halfWidthNormal = ((startPointRenderTargetCoords - endPointRenderTargetCoords).Normalize() * (lineWidth * 0.5f)).GetNormal();

		// Add quad points in clockwise order on the screen around the centered line segment
		Vec2f quadPoint = startPointRenderTargetCoords + halfWidthNormal;
		vertexBatch_.emplace_back(quadPoint, vertices[0].Color, vertices[0].UV);

		quadPoint = endPointRenderTargetCoords + halfWidthNormal;
		vertexBatch_.emplace_back(quadPoint, vertices[1].Color, vertices[1].UV);

		quadPoint = endPointRenderTargetCoords - halfWidthNormal;
		vertexBatch_.emplace_back(quadPoint, vertices[2].Color, vertices[2].UV);

		quadPoint = startPointRenderTargetCoords - halfWidthNormal;
		vertexBatch_.emplace_back(quadPoint, vertices[3].Color, vertices[3].UV);
	}



	void SpriteMeshRenderer3D::RenderPoint(const SpriteMesh& mesh, const Vec3& point, float pointSize)
	{
		const Vertex2D* const vertices = mesh.Vertices;

		// World-space vector from camera to the point (float precision is sufficient in camera-relative space)
		Vec3f cameraToPoint(point - configuration_.InterpolatedCameraPosition);

		// Transform the point to camera space
		float x = configuration_.InterpolatedCameraRotation.GetXAxis().GetDotProduct(cameraToPoint);
		float y = configuration_.InterpolatedCameraRotation.GetYAxis().GetDotProduct(cameraToPoint);
		float z = configuration_.InterpolatedCameraZAxis.GetDotProduct(cameraToPoint);

		if (z > -NEAR_CLIP_DISTANCE) return; // Discard points behind the near clip plane

		// Project the point to logical render-target coordinates (Y increases downward)
		Vec2f renderTargetCoords(x * configuration_.CameraDistanceToScreen / (-z), y * configuration_.CameraDistanceToScreen / z);
		renderTargetCoords += configuration_.RenderTargetOffset;

		const float halfSize = pointSize * 0.5f;

		// Top-left corner
		Vec2f quadPoint = renderTargetCoords + Vec2f(-halfSize, -halfSize);
		vertexBatch_.emplace_back(quadPoint, vertices[0].Color, vertices[0].UV);

		// Top-right corner
		quadPoint = renderTargetCoords + Vec2f(halfSize, -halfSize);
		vertexBatch_.emplace_back(quadPoint, vertices[1].Color, vertices[1].UV);

		// Bottom-right corner
		quadPoint = renderTargetCoords + Vec2f(halfSize, halfSize);
		vertexBatch_.emplace_back(quadPoint, vertices[2].Color, vertices[2].UV);

		// Bottom-left corner
		quadPoint = renderTargetCoords + Vec2f(-halfSize, halfSize);
		vertexBatch_.emplace_back(quadPoint, vertices[3].Color, vertices[3].UV);
	}



	void SpriteMeshRenderer3D::BeginBatch(const MovableObject3D& camera, Vec2f renderTargetOffset, float interpolationAlpha, float verticalFOV)
	{
		vertexBatch_.clear();

		interpolationAlpha = GetClamped(interpolationAlpha, 0.0f, 1.0f);
		verticalFOV = GetClamped(verticalFOV, 1.0f, 89.0f);

		configuration_.InterpolatedCameraPosition = GetInterpolatedUnchecked(camera.GetPreviousTransform().Position, camera.Transform.Position, (double)interpolationAlpha);
		configuration_.InterpolatedCameraRotation = GetInterpolated(camera.GetPreviousTransform().Rotation, camera.Transform.Rotation, interpolationAlpha);
		configuration_.InterpolatedCameraZAxis = configuration_.InterpolatedCameraRotation.GetZAxis();
		configuration_.InterpolationAlpha = interpolationAlpha;
		configuration_.RenderTargetOffset = renderTargetOffset;

		configuration_.CameraDistanceToScreen = (Renderer::Get().GetLogicalResolutionHeight() * 0.5f) / std::tan(verticalFOV * 0.5f * (float)RADIANS_PER_DEGREE);
	}



	void SpriteMeshRenderer3D::RenderBatch(const Texture& texture, TargetTexture* renderTarget)
	{
		if (vertexBatch_.size() < 4) return;

		UpdateVertexIndices();

		const Vertex2D* const vertexArray = vertexBatch_.data();

		Renderer& renderer = Renderer::Get();

		renderer.SetRenderTarget(renderTarget);

		Vec2f cachedRenderScale = renderer.GetRenderScale(); // Cache current render scale

		if (renderTarget)
		{
			int width, height;
			renderTarget->GetSize(width, height);

			renderer.SetRenderScale(float(width) / renderer.GetLogicalResolutionWidth(), float(height) / renderer.GetLogicalResolutionHeight());
		}

		constexpr int stride = sizeof(Vertex2D);

		renderer.RenderGeometryRaw(texture, &(vertexArray->Position.X), stride, &(vertexArray->Color), stride, &(vertexArray->UV.X), stride, vertexBatch_.size(),
			vertexIndices_.data(), (vertexBatch_.size() * 3) / 2, 4);

		renderer.SetRenderScale(cachedRenderScale.X, cachedRenderScale.Y); // Restore cached render scale
	}



	// Triangles are emitted in clockwise order on the screen.
	// First triangle:  0-1-2
	// Second triangle: 2-3-0
	// Vertex layout for a non-rotated SpriteMesh:
	// 0--1
	// |  |
	// 3--2
	void SpriteMeshRenderer3D::UpdateVertexIndices()
	{
		const int vertexCount = vertexBatch_.size();
		const int indexCount = vertexIndices_.size();

		if ((indexCount >= (vertexCount * 3) / 2) || (vertexCount < 4))
			return;

		const int lastSpriteIndex = vertexCount - 4;

		const int startSpriteIndex = (indexCount * 2) / 3;

		for (int i = startSpriteIndex; i <= lastSpriteIndex; i += 4)
		{
			vertexIndices_.push_back(i);
			vertexIndices_.push_back(i + 1);
			vertexIndices_.push_back(i + 2);

			vertexIndices_.push_back(i + 2);
			vertexIndices_.push_back(i + 3);
			vertexIndices_.push_back(i);
		}
	}

}
