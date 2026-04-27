#include "TriangleMesh2DRenderer2D.h"
#include "Renderer.h"

namespace pix
{

	TriangleMesh2DRenderer2D::TriangleMesh2DRenderer2D(int initialVertexBatchCapacity)
	{
		if (initialVertexBatchCapacity > 0)
			vertexBatch_.reserve(initialVertexBatchCapacity);

		// A reasonable default, used only for Sprite2DExNode meshes
		worldPositionBuffer_.reserve(100);
		prevWorldPositionBuffer_.reserve(100);
	}

	// Optimized test
	/* 
	void TriangleMesh2DRenderer2D::Render(const TriangleMesh2D& mesh, const Transform2D& transform)
	{
		const std::vector<Vertex2DEx>& vertices = mesh.Vertices;
		const int vertexCount = vertices.size();

		// Precompute combined object/camera scale
		const Vec2f scale = transform.Scale * configuration_.InterpolatedCameraZoom;

		// Precompute the object rotation in camera space
		Vec2f xAxis = transform.Rotation.GetXAxis();
		configuration_.InterpolatedCameraRotation.InverseRotatePoint(xAxis); // Rotating the local X axis directly avoids an extra normalization step

		// Precompute combined scaling and rotation
		const Vec2f scaledXAxis = xAxis * scale.X;
		const Vec2f scaledYAxis = xAxis.GetNormal() * scale.Y;

		// Start with the world-space vector from camera to object origin (float precision is sufficient in camera-relative space)
		Vec2f cameraToObjectOrigin = Vec2f(transform.Position - configuration_.InterpolatedCameraPosition);

		// Transform the origin offset into camera-scaled space
		configuration_.InterpolatedCameraRotation.InverseRotatePoint(cameraToObjectOrigin);
		cameraToObjectOrigin *= configuration_.InterpolatedCameraZoom;

		// Precompute the full affine transform into render-target coordinates
		const Vec2f basisX = Vec2f(scaledXAxis.X, -scaledXAxis.Y);
		const Vec2f basisY = Vec2f(scaledYAxis.X, -scaledYAxis.Y);
		const Vec2f translation = Vec2f(configuration_.RenderTargetOffset.X + cameraToObjectOrigin.X,
			configuration_.RenderTargetOffset.Y - cameraToObjectOrigin.Y);

		for (int i = 0; i < vertexCount; i++)
		{
			// Transform directly into render-target coordinates
			const Vec2f vertexPosition = (basisX * vertices[i].Position.X) +
				(basisY * vertices[i].Position.Y) +
				translation;

			vertexBatch_.emplace_back(vertexPosition, vertices[i].Color, vertices[i].UV);
		}
	}
	*/



	void TriangleMesh2DRenderer2D::Render(const TriangleMesh2D& mesh, const Transform2D& transform) 
	{
		const std::vector<Vertex2DEx>& vertices = mesh.Vertices; 
		const int vertexCount = vertices.size();

		// Precompute combined object/camera scale for vertices
		const Vec2f scale = transform.Scale * configuration_.InterpolatedCameraZoom;

		// Precompute the object rotation in camera space
		Vec2f xAxis = transform.Rotation.GetXAxis();
		configuration_.InterpolatedCameraRotation.InverseRotatePoint(xAxis); // Rotating the local X axis directly avoids an extra normalization step

		// Precompute for combined scaling and rotation per vertex
		const Vec2f scaledXAxis = xAxis * scale.X;
		const Vec2f scaledYAxis = xAxis.GetNormal() * scale.Y;

		// Start with the world-space vector from camera to object origin (float precision is sufficient in camera-relative space)
		Vec2f cameraToObjectOrigin = Vec2f(transform.Position - configuration_.InterpolatedCameraPosition);

		// Transform the origin offset into camera-scaled space
		configuration_.InterpolatedCameraRotation.InverseRotatePoint(cameraToObjectOrigin);
		cameraToObjectOrigin *= configuration_.InterpolatedCameraZoom;

		for (int i = 0; i < vertexCount; i++)
		{
			// Scale and rotate the vertex position
			Vec2f vertexPosition = (scaledXAxis * vertices[i].Position.X) + (scaledYAxis * vertices[i].Position.Y);

			// Translate the vertex position by the camera-to-object-origin vector
			vertexPosition += cameraToObjectOrigin;

			// Logical screen space -> render target (Y increases downward)
			vertexPosition.X = configuration_.RenderTargetOffset.X + vertexPosition.X;
			vertexPosition.Y = configuration_.RenderTargetOffset.Y - vertexPosition.Y; 

			// Add the transformed vertex to the batch
			vertexBatch_.emplace_back(vertexPosition, vertices[i].Color, vertices[i].UV);
		}
	}

	

	void TriangleMesh2DRenderer2D::Render(const Sprite2DEx& sprite)
	{
		if (!sprite.Mesh) return;

		const std::vector<Vertex2DEx>& vertices = sprite.Mesh->Vertices;
		const int vertexCount = vertices.size();

		// Interpolate the sprite transform
		Transform2D interpolatedTransform = GetInterpolated(sprite.GetPreviousTransform(), sprite.Transform, configuration_.InterpolationAlpha);

		// Precompute the object scale in camera space
		interpolatedTransform.Scale *= configuration_.InterpolatedCameraZoom;

		// Precompute the object rotation in camera space
		Vec2f xAxis = interpolatedTransform.Rotation.GetXAxis();
		configuration_.InterpolatedCameraRotation.InverseRotatePoint(xAxis); // Rotating the local X axis directly avoids an extra normalization step

		// Precompute for combined scaling and rotation per vertex
		const Vec2f scaledXAxis = xAxis * interpolatedTransform.Scale.X;
		const Vec2f scaledYAxis = xAxis.GetNormal() * interpolatedTransform.Scale.Y;

		// Start with the world-space vector from camera to object origin (float precision is sufficient in camera-relative space)
		Vec2f cameraToObjectOrigin = Vec2f(interpolatedTransform.Position - configuration_.InterpolatedCameraPosition);

		// Transform the origin offset into camera-scaled space
		configuration_.InterpolatedCameraRotation.InverseRotatePoint(cameraToObjectOrigin);
		cameraToObjectOrigin *= configuration_.InterpolatedCameraZoom;

		for (int i = 0; i < vertexCount; i++)
		{
			// Scale and rotate the vertex position
			Vec2f vertexPosition = (scaledXAxis * vertices[i].Position.X) + (scaledYAxis * vertices[i].Position.Y);

			// Translate the vertex position by the camera-to-object-origin vector
			vertexPosition += cameraToObjectOrigin;

			// Logical screen space -> render target (Y increases downward)
			vertexPosition.X = configuration_.RenderTargetOffset.X + vertexPosition.X;
			vertexPosition.Y = configuration_.RenderTargetOffset.Y - vertexPosition.Y;

			// Add the transformed vertex to the batch
			vertexBatch_.emplace_back(vertexPosition, vertices[i].Color, vertices[i].UV);
		}
	}

	void TriangleMesh2DRenderer2D::Render(const Sprite2DExNode& node)
	{
		if (!node.Mesh) return;

		const Sprite2DExNode* parent = &node;
		const std::vector<Vertex2DEx>& vertices = node.Mesh->Vertices;
		const int vertexCount = vertices.size();
		const double interpolationAlpha = (double)configuration_.InterpolationAlpha; // Convert to double for repeated use

		worldPositionBuffer_.clear(); 
		prevWorldPositionBuffer_.clear(); 

		for (int i = 0; i < vertexCount; i++)
			worldPositionBuffer_.emplace_back(vertices[i].Position.X, vertices[i].Position.Y);

		prevWorldPositionBuffer_ = worldPositionBuffer_;

		// Transform current and previous vertex positions to world space
		while (parent)
		{
			parent->Transform.TransformPoints(worldPositionBuffer_.data(), worldPositionBuffer_.size());
			parent->GetPreviousTransform().TransformPoints(prevWorldPositionBuffer_.data(), prevWorldPositionBuffer_.size());
			parent = parent->GetParent();
		}

		// Precompute for combined scaling and rotation per vertex
		Rotation2D inverseCameraRotation = configuration_.InterpolatedCameraRotation;
		inverseCameraRotation.Inverse();
		Vec2f scaledCameraXAxis = inverseCameraRotation.GetXAxis() * configuration_.InterpolatedCameraZoom.X;
		Vec2f scaledCameraYAxis = inverseCameraRotation.GetYAxis() * configuration_.InterpolatedCameraZoom.Y;

		for (int i = 0; i < vertexCount; i++)
		{
			// Interpolate the world-space vertex position
			worldPositionBuffer_[i] = GetInterpolatedUnchecked(prevWorldPositionBuffer_[i], worldPositionBuffer_[i], interpolationAlpha);

			// Start with the world-space vector from camera to vertex (float precision is sufficient in camera-relative space)
			Vec2f vertexPosition = Vec2f(worldPositionBuffer_[i] - configuration_.InterpolatedCameraPosition);

			// Zoom and rotate the vertex position
			vertexPosition = (scaledCameraXAxis * vertexPosition.X) + (scaledCameraYAxis * vertexPosition.Y);

			// Logical screen space -> render target (Y increases downward)
			vertexPosition.X = configuration_.RenderTargetOffset.X + vertexPosition.X;
			vertexPosition.Y = configuration_.RenderTargetOffset.Y - vertexPosition.Y; 

			vertexBatch_.emplace_back(vertexPosition, vertices[i].Color, vertices[i].UV);
		}
	}

	void TriangleMesh2DRenderer2D::RenderFast(const Sprite2DExNode& node)
	{
		if (!node.Mesh) return;

		const std::vector<Vertex2DEx>& vertices = node.Mesh->Vertices;
		const int vertexCount = vertices.size();

		Transform2D interpolatedTransform = node.GetGlobalTransform();
		Transform2D prevTransform = node.GetPrevGlobalTransform();

		// Interpolate the global node transform
		interpolatedTransform = GetInterpolated(prevTransform, interpolatedTransform, configuration_.InterpolationAlpha);

		// Precompute the object scale in camera space
		interpolatedTransform.Scale *= configuration_.InterpolatedCameraZoom; 

		// Precompute the object rotation in camera space
		Vec2f xAxis = interpolatedTransform.Rotation.GetXAxis();
		configuration_.InterpolatedCameraRotation.InverseRotatePoint(xAxis); // Rotating the local X axis directly avoids an extra normalization step

		// Precompute for combined scaling and rotation per vertex
		const Vec2f scaledXAxis = xAxis * interpolatedTransform.Scale.X;
		const Vec2f scaledYAxis = xAxis.GetNormal() * interpolatedTransform.Scale.Y;

		// Start with the world-space vector from camera to object origin (float precision is sufficient in camera-relative space)
		Vec2f cameraToObjectOrigin = Vec2f(interpolatedTransform.Position - configuration_.InterpolatedCameraPosition);

		// Transform the origin offset into camera-scaled space
		configuration_.InterpolatedCameraRotation.InverseRotatePoint(cameraToObjectOrigin);
		cameraToObjectOrigin *= configuration_.InterpolatedCameraZoom;

		for (int i = 0; i < vertexCount; i++)
		{
			// Scale and rotate the vertex position
			Vec2f vertexPosition = (scaledXAxis * vertices[i].Position.X) + (scaledYAxis * vertices[i].Position.Y);

			// Translate the vertex position by the camera-to-object-origin vector
			vertexPosition += cameraToObjectOrigin;

			// Logical screen space -> render target (Y increases downward)
			vertexPosition.X = configuration_.RenderTargetOffset.X + vertexPosition.X;
			vertexPosition.Y = configuration_.RenderTargetOffset.Y - vertexPosition.Y;

			// Add the transformed vertex to the batch
			vertexBatch_.emplace_back(vertexPosition, vertices[i].Color, vertices[i].UV);
		}
	}

	void TriangleMesh2DRenderer2D::BeginBatch(const MovableObject2D& camera, Vec2f renderTargetOffset, float interpolationAlpha)
	{
		vertexBatch_.clear();

		interpolationAlpha = GetClamped(interpolationAlpha, 0.0f, 1.0f);

		configuration_.InterpolatedCameraPosition = GetInterpolatedUnchecked(camera.GetPreviousTransform().Position, camera.Transform.Position, (double)interpolationAlpha);
		configuration_.InterpolatedCameraZoom = GetInterpolatedUnchecked(camera.GetPreviousTransform().Scale, camera.Transform.Scale, interpolationAlpha);
		configuration_.InterpolatedCameraRotation = GetInterpolated(camera.GetPreviousTransform().Rotation, camera.Transform.Rotation, interpolationAlpha);
		configuration_.InterpolationAlpha = interpolationAlpha;
		configuration_.RenderTargetOffset = renderTargetOffset;
	}

	void TriangleMesh2DRenderer2D::RenderBatch(const Texture& texture, TargetTexture* renderTarget) 
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


