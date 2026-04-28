#include "TriangleMeshRenderer3D.h"
#include "Renderer.h"

namespace pix
{
	TriangleMeshRenderer3D::TriangleMeshRenderer3D(int initialVertexBatchCapacity)
	{
		if (initialVertexBatchCapacity > 0)
			vertexBatch_.reserve(initialVertexBatchCapacity);

		    // A reasonable default, used only in Render(const Sprite3DExNode& node) 
			worldPositionBuffer_.reserve(100); 
			prevWorldPositionBuffer_.reserve(100);
	}



	void TriangleMeshRenderer3D::Render(const TriangleMesh3D& mesh, const Transform3D& transform)
	{
		const std::vector<Vertex3D>& vertices = mesh.Vertices;
		const int vertexCount = vertices.size();

		// Precompute for combined scaling and rotation per vertex
		const Vec3f scaledXAxis = transform.Rotation.GetXAxis() * transform.Scale.X;
		const Vec3f scaledYAxis = transform.Rotation.GetYAxis() * transform.Scale.Y;
		const Vec3f scaledZAxis = transform.Rotation.GetZAxis() * transform.Scale.Z;

		// World-space vector from camera to object origin (float precision is sufficient in camera-relative space)
		const Vec3f cameraToObjectOrigin(transform.Position - configuration_.InterpolatedCameraPosition);

		for (int i = 0; i < vertexCount; i++)
		{
			const int triangleVertexIndex = i % 3;

			Vec3f vertexPosition = vertices[i].Position;

			// Scale and rotate the vertex position
			vertexPosition = (scaledXAxis * vertexPosition.X) + (scaledYAxis * vertexPosition.Y) + (scaledZAxis * vertexPosition.Z);

			// Translate the vertex position by the camera-to-object-origin vector
			vertexPosition += cameraToObjectOrigin;

			// Transform the vertex position to camera space or discard the triangle
			const float z = configuration_.InterpolatedCameraZAxis.GetDotProduct(vertexPosition);
			if (z > -NEAR_CLIP_DISTANCE)
			{
				// Remove already added vertices of the current triangle from the batch
				for (int j = triangleVertexIndex; j > 0; j--) 
					vertexBatch_.pop_back();

				i += (2 - triangleVertexIndex); // Jump to the last vertex of the discarded triangle

				continue;
			}
			const float x = configuration_.InterpolatedCameraRotation.GetXAxis().GetDotProduct(vertexPosition);
			const float y = configuration_.InterpolatedCameraRotation.GetYAxis().GetDotProduct(vertexPosition);

			// Project the vertex position to logical render-target coordinates (Y increases downward)
			Vec2f renderTargetCoords(x * configuration_.CameraDistanceToScreen / (-z), y * configuration_.CameraDistanceToScreen / z);
			renderTargetCoords += configuration_.RenderTargetOffset;

			// Cull backface or add the transformed vertex to the batch
			if (triangleVertexIndex == 2)
			{
				const int end = vertexBatch_.size() - 1;

				const Vec2f edgeNormal(vertexBatch_[end - 1].Position.Y - vertexBatch_[end].Position.Y, vertexBatch_[end].Position.X - vertexBatch_[end - 1].Position.X);
				const Vec2f closingEdge(renderTargetCoords.X - vertexBatch_[end].Position.X, renderTargetCoords.Y - vertexBatch_[end].Position.Y);

				if (closingEdge.GetDotProduct(edgeNormal) > 0.0f) // Front faces appear in clockwise vertex order on the screen
				{
					vertexBatch_.emplace_back(renderTargetCoords, vertices[i].Color, vertices[i].UV);
				}
				else
				{
					vertexBatch_.pop_back();
					vertexBatch_.pop_back();
				}
			}
			else
			{
				vertexBatch_.emplace_back(renderTargetCoords, vertices[i].Color, vertices[i].UV);
			}
		}
	}

	// Optimization test
	/*
	void TriangleMeshRenderer3D::Render(const TriangleMesh2D& mesh, const Transform3D& transform)
	{
		const std::vector<Vertex2DEx>& vertices = mesh.Vertices;
		const int vertexCount = vertices.size();

		// Object-space basis vectors scaled into world space
		const Vec3f modelXAxis = transform.Rotation.GetXAxis() * transform.Scale.X;
		const Vec3f modelYAxis = transform.Rotation.GetYAxis() * transform.Scale.Y;

		// World-space vector from camera to object origin
		const Vec3f modelOrigin = Vec3f(transform.Position - configuration_.InterpolatedCameraPosition);

		// Cache camera axes
		const Vec3f cameraXAxis = configuration_.InterpolatedCameraRotation.GetXAxis();
		const Vec3f cameraYAxis = configuration_.InterpolatedCameraRotation.GetYAxis();
		const Vec3f cameraZAxis = configuration_.InterpolatedCameraZAxis;

		// Precompute the full local-to-camera transform for local vertices (vx, vy, 0):
		//
		// x = m00 * vx + m01 * vy + m03
		// y = m10 * vx + m11 * vy + m13
		// z = m20 * vx + m21 * vy + m23
		const float m00 = cameraXAxis.GetDotProduct(modelXAxis);
		const float m01 = cameraXAxis.GetDotProduct(modelYAxis);
		const float m03 = cameraXAxis.GetDotProduct(modelOrigin);

		const float m10 = cameraYAxis.GetDotProduct(modelXAxis);
		const float m11 = cameraYAxis.GetDotProduct(modelYAxis);
		const float m13 = cameraYAxis.GetDotProduct(modelOrigin);

		const float m20 = cameraZAxis.GetDotProduct(modelXAxis);
		const float m21 = cameraZAxis.GetDotProduct(modelYAxis);
		const float m23 = cameraZAxis.GetDotProduct(modelOrigin);

		const float cameraDistanceToScreen = configuration_.CameraDistanceToScreen;
		const Vec2f renderTargetOffset = configuration_.RenderTargetOffset;

		for (int i = 0; i < vertexCount; i++)
		{
			const float vx = vertices[i].Position.X;
			const float vy = vertices[i].Position.Y;

			// Transform directly into camera space
			const float z = (m20 * vx) + (m21 * vy) + m23;
			if (z > -NEAR_CLIP_DISTANCE)
			{
				const int triangleVertexIndex = i % 3;

				// Remove already added vertices of the current triangle from the batch
				for (int j = triangleVertexIndex; j > 0; j--)
					vertexBatch_.pop_back();

				i += (2 - triangleVertexIndex); // Jump to the last vertex of the discarded triangle
				continue;
			}

			const float x = (m00 * vx) + (m01 * vy) + m03;
			const float y = (m10 * vx) + (m11 * vy) + m13;

			// Project the vertex position to logical render-target coordinates (Y increases downward)
			const float invNegZ = cameraDistanceToScreen / (-z);
			const Vec2f renderTargetCoords(
				renderTargetOffset.X + (x * invNegZ),
				renderTargetOffset.Y - (y * invNegZ)
			);

			// Add the transformed vertex to the batch
			vertexBatch_.emplace_back(renderTargetCoords, vertices[i].Color, vertices[i].UV);
		}
	}
	*/


	void TriangleMeshRenderer3D::Render(const TriangleMesh2D& mesh, const Transform3D& transform)
	{
		const std::vector<Vertex2DEx>& vertices = mesh.Vertices;
		const int vertexCount = vertices.size();

		// Precompute for combined scaling and rotation per vertex
		const Vec3f scaledXAxis = transform.Rotation.GetXAxis() * transform.Scale.X;
		const Vec3f scaledYAxis = transform.Rotation.GetYAxis() * transform.Scale.Y;

		// World-space vector from camera to object origin (float precision is sufficient in camera-relative space)
		const Vec3f cameraToObjectOrigin(transform.Position - configuration_.InterpolatedCameraPosition);

		for (int i = 0; i < vertexCount; i++)
		{
			Vec3f vertexPosition(vertices[i].Position.X, vertices[i].Position.Y, 0.0f);

			// Scale and rotate the vertex position (Z can be ignored)
			vertexPosition = (scaledXAxis * vertexPosition.X) + (scaledYAxis * vertexPosition.Y);

			// Translate the vertex position by the camera-to-object-origin vector
			vertexPosition += cameraToObjectOrigin;

			// Transform the vertex position to camera space or discard the triangle
			const float z = configuration_.InterpolatedCameraZAxis.GetDotProduct(vertexPosition);
			if (z > -NEAR_CLIP_DISTANCE)
			{
				const int triangleVertexIndex = i % 3;

				// Remove already added vertices of the current triangle from the batch
				for (int j = triangleVertexIndex; j > 0; j--)
					vertexBatch_.pop_back();

				i += (2 - triangleVertexIndex); // Jump to the last vertex of the discarded triangle

				continue;
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

	

	void TriangleMeshRenderer3D::Render(const Sprite3DEx& sprite)
	{
		if (!sprite.Mesh) return;

		const std::vector<Vertex2DEx>& vertices = sprite.Mesh->Vertices;
		const int vertexCount = vertices.size();

		// Interpolate the sprite transform
		Transform3D interpolatedTransform = GetInterpolated(sprite.GetPreviousTransform(), sprite.Transform, configuration_.InterpolationAlpha);

		// Precompute for combined scaling and rotation per vertex
		const Vec3f scaledXAxis = interpolatedTransform.Rotation.GetXAxis() * interpolatedTransform.Scale.X;
		const Vec3f scaledYAxis = interpolatedTransform.Rotation.GetYAxis() * interpolatedTransform.Scale.Y;

		// World-space vector from camera to object origin (float precision is sufficient in camera-relative space)
		const Vec3f cameraToObjectOrigin(interpolatedTransform.Position - configuration_.InterpolatedCameraPosition);

		for (int i = 0; i < vertexCount; i++)
		{
			Vec3f vertexPosition(vertices[i].Position.X, vertices[i].Position.Y, 0.0f);

			// Scale and rotate the vertex position (Z can be ignored)
			vertexPosition = (scaledXAxis * vertexPosition.X) + (scaledYAxis * vertexPosition.Y);

			// Translate the vertex position by the camera-to-object-origin vector
			vertexPosition += cameraToObjectOrigin;

			// Transform the vertex position to camera space or discard the triangle
			const float z = configuration_.InterpolatedCameraZAxis.GetDotProduct(vertexPosition);
			if (z > -NEAR_CLIP_DISTANCE)
			{
				const int triangleVertexIndex = i % 3;

				// Remove already added vertices of the current triangle from the batch
				for (int j = triangleVertexIndex; j > 0; j--)
					vertexBatch_.pop_back();

				i += (2 - triangleVertexIndex);  // Jump to the last vertex of the discarded triangle

				continue;
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



	void TriangleMeshRenderer3D::Render(const Sprite3DExNode& node) 
	{
		if (!node.Mesh) return;

		const Sprite3DExNode* parent = &node;
		const std::vector<Vertex2DEx>& vertices = (node.Mesh)->Vertices;
		const int vertexCount = vertices.size();
		const double interpolationAlpha = configuration_.InterpolationAlpha; // Convert to double for repeated use

		worldPositionBuffer_.clear(); 
		prevWorldPositionBuffer_.clear(); 

		for (int i = 0; i < vertexCount; i++)
			worldPositionBuffer_.emplace_back(vertices[i].Position.X, vertices[i].Position.Y, 0.0);

		prevWorldPositionBuffer_ = worldPositionBuffer_;

		// Transform current and previous vertex positions to world space
		while (parent != nullptr)
		{
			parent->Transform.TransformPoints(worldPositionBuffer_.data(), worldPositionBuffer_.size());
			parent->GetPreviousTransform().TransformPoints(prevWorldPositionBuffer_.data(), prevWorldPositionBuffer_.size());
			parent = parent->GetParent();
		}

		for (int i = 0; i < vertexCount; i++)
		{
			// Interpolate the world-space vertex position
			worldPositionBuffer_[i] = GetInterpolatedUnchecked(prevWorldPositionBuffer_[i], worldPositionBuffer_[i], interpolationAlpha);

			// World-space vector from camera to vertex (float precision is sufficient in camera-relative space)
			const Vec3f cameraToVertex(worldPositionBuffer_[i] - configuration_.InterpolatedCameraPosition);

			// Transform the vertex position to camera space or discard the triangle
			const float z = configuration_.InterpolatedCameraZAxis.GetDotProduct(cameraToVertex);
			if (z > -NEAR_CLIP_DISTANCE)
			{
				const int triangleVertexIndex = i % 3;

				// Remove already added vertices of the current triangle from the batch
				for (int j = triangleVertexIndex; j > 0; j--)
					vertexBatch_.pop_back();

				i += (2 - triangleVertexIndex); // Jump to the last vertex of the discarded triangle

				continue;
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



	void TriangleMeshRenderer3D::RenderFast(const Sprite3DExNode& node)
	{
		if (!node.Mesh) return;

		const std::vector<Vertex2DEx>& vertices = (node.Mesh)->Vertices;
		const int vertexCount = vertices.size();

		Transform3D interpolatedTransform = node.GetGlobalTransform();
		Transform3D prevTransform = node.GetPrevGlobalTransform();

		// Interpolate the global node transform
		interpolatedTransform = GetInterpolated(prevTransform, interpolatedTransform, configuration_.InterpolationAlpha);

		// Precompute for combined scaling and rotation per vertex
		const Vec3f scaledXAxis = interpolatedTransform.Rotation.GetXAxis() * interpolatedTransform.Scale.X;
		const Vec3f scaledYAxis = interpolatedTransform.Rotation.GetYAxis() * interpolatedTransform.Scale.Y;

		// World-space vector from camera to object origin (float precision is sufficient in camera-relative space)
		const Vec3f cameraToObjectOrigin(interpolatedTransform.Position - configuration_.InterpolatedCameraPosition);

		for (int i = 0; i < vertexCount; i++)
		{
			Vec3f vertexPosition(vertices[i].Position.X, vertices[i].Position.Y, 0.0f);

			// Scale and rotate the vertex position (Z can be ignored)
			vertexPosition = (scaledXAxis * vertexPosition.X) + (scaledYAxis * vertexPosition.Y);

			// Translate the vertex position by the camera-to-object-origin vector
			vertexPosition += cameraToObjectOrigin;

			// Transform the vertex position to camera space or discard the triangle
			const float z = configuration_.InterpolatedCameraZAxis.GetDotProduct(vertexPosition);
			if (z > -NEAR_CLIP_DISTANCE)
			{
				const int triangleVertexIndex = i % 3;

				// Remove already added vertices of the current triangle from the batch
				for (int j = triangleVertexIndex; j > 0; j--)
					vertexBatch_.pop_back();

				i += (2 - triangleVertexIndex);  // Jump to the last vertex of the discarded triangle

				continue;
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



	void TriangleMeshRenderer3D::BeginBatch(const MovableObject3D& camera, Vec2f renderTargetOffset, float interpolationAlpha, float verticalFOV)
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

	
	void TriangleMeshRenderer3D::RenderBatch(const Texture& texture, TargetTexture* renderTarget)
	{
		if (vertexBatch_.size() < 3) return;

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

		renderer.RenderGeometryRaw(texture, &(vertexArray->Position.X), stride, &(vertexArray->Color), stride, &(vertexArray->UV.X), stride, vertexBatch_.size(), nullptr, 0, 4);

		renderer.SetRenderScale(cachedRenderScale.X, cachedRenderScale.Y); // Restore cached render scale
	}
}