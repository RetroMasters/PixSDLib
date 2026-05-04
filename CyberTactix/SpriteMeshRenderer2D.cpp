#include "SpriteMeshRenderer2D.h"
#include "Renderer.h"

namespace pix
{

	SpriteMeshRenderer2D::SpriteMeshRenderer2D(int initialVertexBatchCapacity)
	{
		if (initialVertexBatchCapacity > 0)
		{
			vertexBatch_.reserve(initialVertexBatchCapacity);
			vertexIndices_.reserve((initialVertexBatchCapacity * 3) / 2);
		}
	}



	void SpriteMeshRenderer2D::Render(const SpriteMesh& mesh, const Transform2D& transform) 
	{
		const Vertex2D* const vertices = mesh.Vertices;

		// Precompute the object scale in logical screen space
		const Vec2f scale = transform.Scale * configuration_.InterpolatedCameraZoom;

		// Precompute the object rotation in logical screen space
		Vec2f xAxis = transform.Rotation.GetXAxis();
		configuration_.InterpolatedCameraRotation.InverseRotatePoint(xAxis);

		// Precompute the combined scale and rotation in logical screen space for per-vertex use
		const Vec2f scaledXAxis = xAxis * scale.X;
		const Vec2f scaledYAxis = xAxis.GetNormal() * scale.Y;

		// Start with the world-space vector from camera to object origin (float precision is sufficient in camera-relative space)
		Vec2f originOffset(transform.Position - configuration_.InterpolatedCameraPosition);

		// Transform the origin offset to logical screen space
		configuration_.InterpolatedCameraRotation.InverseRotatePoint(originOffset);
		originOffset *= configuration_.InterpolatedCameraZoom;

		for (int i = 0; i < 4; i++)
		{
			// Scale and rotate the vertex position
			Vec2f vertexPosition = (scaledXAxis * vertices[i].Position.X) + (scaledYAxis * vertices[i].Position.Y);

			// After translating by origin offset, the vertex position is now in logical screen space
			vertexPosition += originOffset;

			// Logical screen space -> render target (Y increases downward)
			vertexPosition.X = configuration_.RenderTargetOffset.X + vertexPosition.X;
			vertexPosition.Y = configuration_.RenderTargetOffset.Y - vertexPosition.Y;

			// Add the transformed vertex to the batch
			vertexBatch_.emplace_back(vertexPosition, vertices[i].Color, vertices[i].UV);
		}
	}

	void SpriteMeshRenderer2D::Render(const Sprite2D& sprite)
	{
		if (!sprite.Mesh) return;

		const Vertex2D* const vertices = sprite.Mesh->Vertices;

		// Interpolate the sprite transform
		Transform2D interpolatedTransform = GetInterpolated(sprite.GetPreviousTransform(), sprite.Transform, configuration_.InterpolationAlpha);

		// Precompute the object rotation in logical screen space
		Vec2f xAxis = interpolatedTransform.Rotation.GetXAxis();
		configuration_.InterpolatedCameraRotation.InverseRotatePoint(xAxis);

		// Precompute the combined scale and rotation in logical screen space for per-vertex use
		const Vec2f scaledXAxis = xAxis * (interpolatedTransform.Scale.X * configuration_.InterpolatedCameraZoom.X);
		const Vec2f scaledYAxis = xAxis.GetNormal() * (interpolatedTransform.Scale.Y * configuration_.InterpolatedCameraZoom.Y);

		// Start with the world-space vector from camera to object origin (float precision is sufficient in camera-relative space)
		Vec2f originOffset(interpolatedTransform.Position - configuration_.InterpolatedCameraPosition);

		// Transform the origin offset to logical screen space
		configuration_.InterpolatedCameraRotation.InverseRotatePoint(originOffset);
		originOffset *= configuration_.InterpolatedCameraZoom;

		for (int i = 0; i < 4; i++)
		{
			// Scale and rotate the vertex position
			Vec2f vertexPosition = (scaledXAxis * vertices[i].Position.X) + (scaledYAxis * vertices[i].Position.Y);

			// After translating by origin offset, the vertex position is now in logical screen space
			vertexPosition += originOffset;

			// Logical screen space -> render target (Y increases downward)
			vertexPosition.X = configuration_.RenderTargetOffset.X + vertexPosition.X;
			vertexPosition.Y = configuration_.RenderTargetOffset.Y - vertexPosition.Y;

			// Add the transformed vertex to the batch
			vertexBatch_.emplace_back(vertexPosition, vertices[i].Color, vertices[i].UV);
		}
	}

	void SpriteMeshRenderer2D::Render(const Sprite2DNode& node) 
	{
		if (!node.Mesh) return;

		const Vertex2D* const vertices = node.Mesh->Vertices;
		const Sprite2DNode* parent = &node;
		const double interpolationAlpha = configuration_.InterpolationAlpha; // Convert to double for repeated use

		// Cache initial vertex positions
		Vec2 worldPositionBuffer[4] = 
		{ 
			Vec2(vertices[0].Position), 
			Vec2(vertices[1].Position), 
			Vec2(vertices[2].Position), 
			Vec2(vertices[3].Position) 
		};

		Vec2 prevWorldPositionBuffer[4] = { worldPositionBuffer[0], worldPositionBuffer[1], worldPositionBuffer[2], worldPositionBuffer[3] };

		// Transform current and previous vertex positions to world space
		while (parent)
		{
			parent->Transform.TransformPoints(worldPositionBuffer, 4);
			parent->GetPreviousTransform().TransformPoints(prevWorldPositionBuffer, 4);
			parent = parent->GetParent();
		}

		// Precompute the camera's combined scale and inverse rotation for per-vertex use
		Rotation2D inverseCameraRotation = configuration_.InterpolatedCameraRotation;
		inverseCameraRotation.Inverse();
		Vec2f scaledCameraXAxis = inverseCameraRotation.GetXAxis() * configuration_.InterpolatedCameraZoom.X;
		Vec2f scaledCameraYAxis = inverseCameraRotation.GetYAxis() * configuration_.InterpolatedCameraZoom.Y;

		for (int i = 0; i < 4; i++)
		{
			// Interpolate the world-space vertex position
			worldPositionBuffer[i] = GetInterpolatedUnchecked(prevWorldPositionBuffer[i], worldPositionBuffer[i], interpolationAlpha);

			// Start with the world-space vector from camera to vertex position (float precision is sufficient in camera-relative space)
			Vec2f vertexPosition(worldPositionBuffer[i] - configuration_.InterpolatedCameraPosition);

			// After zooming and rotating, the vertex position is now in logical screen space
			vertexPosition = (scaledCameraXAxis * vertexPosition.X) + (scaledCameraYAxis * vertexPosition.Y);

			// Logical screen space -> render target (Y increases downward)
			vertexPosition.X = configuration_.RenderTargetOffset.X + vertexPosition.X;
			vertexPosition.Y = configuration_.RenderTargetOffset.Y - vertexPosition.Y;

			// Add the transformed vertex to the batch
			vertexBatch_.emplace_back(vertexPosition, vertices[i].Color, vertices[i].UV);
		}
	}



	void SpriteMeshRenderer2D::RenderFast(const Sprite2DNode& node)
	{
		if (!node.Mesh) return;

		const Vertex2D* const vertices = node.Mesh->Vertices;

		Transform2D prevTransform = node.GetPrevGlobalTransform();

		// Interpolate the global node transform
		Transform2D interpolatedTransform = node.GetGlobalTransform();
		interpolatedTransform = GetInterpolated(prevTransform, interpolatedTransform, configuration_.InterpolationAlpha);

		// Precompute the object rotation in logical screen space
		Vec2f xAxis = interpolatedTransform.Rotation.GetXAxis();
		configuration_.InterpolatedCameraRotation.InverseRotatePoint(xAxis);

		// Precompute the combined scale and rotation in logical screen space for per-vertex use
		const Vec2f scaledXAxis = xAxis * (interpolatedTransform.Scale.X * configuration_.InterpolatedCameraZoom.X);
		const Vec2f scaledYAxis = xAxis.GetNormal() * (interpolatedTransform.Scale.Y * configuration_.InterpolatedCameraZoom.Y);

		// Start with the world-space vector from camera to object origin (float precision is sufficient in camera-relative space)
		Vec2f originOffset(interpolatedTransform.Position - configuration_.InterpolatedCameraPosition);

		// Transform the origin offset to logical screen space
		configuration_.InterpolatedCameraRotation.InverseRotatePoint(originOffset);
		originOffset *= configuration_.InterpolatedCameraZoom;

		for (int i = 0; i < 4; i++)
		{
			// Scale and rotate the vertex position
			Vec2f vertexPosition = (scaledXAxis * vertices[i].Position.X) + (scaledYAxis * vertices[i].Position.Y);

			// After translating by origin offset, the vertex position is now in logical screen space
			vertexPosition += originOffset;

			// Logical screen space -> render target (Y increases downward)
			vertexPosition.X = configuration_.RenderTargetOffset.X + vertexPosition.X;
			vertexPosition.Y = configuration_.RenderTargetOffset.Y - vertexPosition.Y;

			// Add the transformed vertex to the batch
			vertexBatch_.emplace_back(vertexPosition, vertices[i].Color, vertices[i].UV);
		}
	}



	void SpriteMeshRenderer2D::RenderLine(const SpriteMesh& mesh, Vec2 startPoint, Vec2 endPoint, float lineWidth) 
	{
		const Vertex2D* const vertices = mesh.Vertices;

		// Start with the world-space vectors from camera to the line points (float precision is sufficient in camera-relative space)
		Vec2f transformedStartPoint(startPoint - configuration_.InterpolatedCameraPosition);
		Vec2f transformedEndPoint(endPoint - configuration_.InterpolatedCameraPosition);

		// Apply inverse camera rotation
		configuration_.InterpolatedCameraRotation.InverseRotatePoint(transformedStartPoint);
		configuration_.InterpolatedCameraRotation.InverseRotatePoint(transformedEndPoint);

		// Apply camera zoom, now the points are in logical screen space
		transformedStartPoint *= configuration_.InterpolatedCameraZoom;
		transformedEndPoint *= configuration_.InterpolatedCameraZoom;

		// Logical screen space -> render target (Y increases downward)
		transformedStartPoint.X = configuration_.RenderTargetOffset.X + transformedStartPoint.X;
		transformedStartPoint.Y = configuration_.RenderTargetOffset.Y - transformedStartPoint.Y;
		transformedEndPoint.X = configuration_.RenderTargetOffset.X + transformedEndPoint.X;
		transformedEndPoint.Y = configuration_.RenderTargetOffset.Y - transformedEndPoint.Y;

		const Vec2f halfWidthNormal = ((transformedStartPoint - transformedEndPoint).Normalize() * (lineWidth * 0.5f)).GetNormal();

		// Add quad points in clockwise order on the screen around the centered line segment
		Vec2f quadPoint = transformedStartPoint + halfWidthNormal;
		vertexBatch_.emplace_back(quadPoint, vertices[0].Color, vertices[0].UV);

		quadPoint = transformedEndPoint + halfWidthNormal;
		vertexBatch_.emplace_back(quadPoint, vertices[1].Color, vertices[1].UV);

		quadPoint = transformedEndPoint - halfWidthNormal;
		vertexBatch_.emplace_back(quadPoint, vertices[2].Color, vertices[2].UV);

		quadPoint = transformedStartPoint - halfWidthNormal;
		vertexBatch_.emplace_back(quadPoint, vertices[3].Color, vertices[3].UV);
	}



	void SpriteMeshRenderer2D::RenderPoint(const SpriteMesh& mesh, Vec2 point, float pointSize) 
	{
		const Vertex2D* const vertices = mesh.Vertices;

		// Start with the world-space vector from camera to the point (float precision is sufficient in camera-relative space)
		Vec2f transformedPoint(point - configuration_.InterpolatedCameraPosition);

		// Apply inverse camera rotation
		configuration_.InterpolatedCameraRotation.InverseRotatePoint(transformedPoint);

		// Apply camera zoom, now the point is in logical screen space
		transformedPoint *= configuration_.InterpolatedCameraZoom;

		// Logical screen space -> render target (Y increases downward)
		transformedPoint.X = configuration_.RenderTargetOffset.X + transformedPoint.X;
		transformedPoint.Y = configuration_.RenderTargetOffset.Y - transformedPoint.Y;

		const float halfSize = pointSize * 0.5f; // Half side length of the generated quad

		// Top-left corner
		Vec2f quadPoint(transformedPoint.X - halfSize, transformedPoint.Y - halfSize);
		vertexBatch_.emplace_back(quadPoint, vertices[0].Color, vertices[0].UV);

		// Top-right corner
		quadPoint = Vec2f(transformedPoint.X + halfSize, transformedPoint.Y - halfSize);
		vertexBatch_.emplace_back(quadPoint, vertices[1].Color, vertices[1].UV);

		// Bottom-right corner
		quadPoint = Vec2f(transformedPoint.X + halfSize, transformedPoint.Y + halfSize);
		vertexBatch_.emplace_back(quadPoint, vertices[2].Color, vertices[2].UV);

		// Bottom-left corner
		quadPoint = Vec2f(transformedPoint.X - halfSize, transformedPoint.Y + halfSize);
		vertexBatch_.emplace_back(quadPoint, vertices[3].Color, vertices[3].UV);
	}

	

	void SpriteMeshRenderer2D::BeginBatch(const MovableObject2D& camera, Vec2f renderTargetOffset, float interpolationAlpha)
	{
		vertexBatch_.clear();

		interpolationAlpha = GetClamped(interpolationAlpha, 0.0f, 1.0f);

		configuration_.InterpolatedCameraPosition = GetInterpolatedUnchecked(camera.GetPreviousTransform().Position, camera.Transform.Position, (double)interpolationAlpha);
		configuration_.InterpolatedCameraZoom = GetInterpolatedUnchecked(camera.GetPreviousTransform().Scale, camera.Transform.Scale, interpolationAlpha);
		configuration_.InterpolatedCameraRotation = GetInterpolated(camera.GetPreviousTransform().Rotation, camera.Transform.Rotation, interpolationAlpha);
		configuration_.InterpolationAlpha = interpolationAlpha;
		configuration_.RenderTargetOffset = renderTargetOffset;
	}



	void SpriteMeshRenderer2D::RenderBatch(const Texture& texture, TargetTexture* renderTarget) 
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

			renderer.SetRenderScale((float)width / renderer.GetLogicalResolutionWidth(), (float)height / renderer.GetLogicalResolutionHeight());
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
	void SpriteMeshRenderer2D::UpdateVertexIndices()
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