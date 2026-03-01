#include "SpriteMeshRenderer2D.h"
#include "Renderer.h"
#include "PixMath.h"

namespace pix
{

	SpriteMeshRenderer2D::SpriteMeshRenderer2D(int initialVertexBatchSize)
	{
		vertexBatch_.reserve(initialVertexBatchSize);
		vertexIndices_.reserve((initialVertexBatchSize * 3) / 2);
	}


	void SpriteMeshRenderer2D::Render(const SpriteMesh& mesh, const Transform2D& transform) 
	{
		const Vertex2D* const vertices = mesh.Vertices;

		// Precompute total (interpolated) scale vor vertices:
		const Vec2f scale = transform.Scale * configuration_.InterpolatedCameraZoom;

		// Precompute total (interpolated) rotation for vertices:
		Vec2f xAxis = transform.Rotation.GetXAxis();
		configuration_.InterpolatedCameraRotation.InverseRotatePoint(xAxis);

		// Compute distance-to-camera (in camera space float should provide sufficient precision):
		Vec2f destinationCenter = Vec2f(transform.Position - configuration_.InterpolatedCameraPosition);

		// Apply camera rotation to distance-to-camera:
		configuration_.InterpolatedCameraRotation.InverseRotatePoint(destinationCenter);

		// Apply camera zoom to distance-to-camera:
		destinationCenter *= configuration_.InterpolatedCameraZoom;

		//destinationCenter.X = std::roundf(destinationCenter.X); // Snapping pixel art games
		//destinationCenter.Y = std::roundf(destinationCenter.Y);

		for (size_t i = 0; i < SpriteMesh::VERTEX_COUNT; i++)
		{
			// Scale the current mesh vertex:
			Vec2f destination = vertices[i].Position * scale;

			// Rotate the current mesh vertex:
			RotatePointUnchecked(xAxis, destination);

			// Translate to (scaled) camera space:
			destination += destinationCenter;

			// Position on render target
			destination.X = configuration_.RenderTargetCenter.X + destination.X;
			destination.Y = configuration_.RenderTargetCenter.Y - destination.Y;

			// Add to vertices batch:
			vertexBatch_.emplace_back(destination, vertices[i].Color, vertices[i].UV);
		}
	}

	void SpriteMeshRenderer2D::Render(const Sprite2D& sprite)
	{
		if (!sprite.Mesh) return;

		const Vertex2D* const vertices = sprite.Mesh->Vertices;

		// Interpolate transform
		Transform2D interpolatedTransform = GetInterpolated(sprite.GetPreviousTransform(), sprite.Transform, configuration_.InterpolationAlpha);

		// Precompute total (interpolated) scale for vertices
		interpolatedTransform.Scale *= configuration_.InterpolatedCameraZoom; // Misusing as a temporary for total scale is fine here 

		// Precompute total (interpolated) rotation for vertices
		Vec2f xAxis = interpolatedTransform.Rotation.GetXAxis();
		configuration_.InterpolatedCameraRotation.InverseRotatePoint(xAxis); // Rotation of raw X axis avoids normalization step

		// World -> camera space of mesh center (in camera space float provides sufficient precision)
		Vec2f destinationCenter = Vec2f(interpolatedTransform.Position - configuration_.InterpolatedCameraPosition);

		// Apply inverse camera rotation
		configuration_.InterpolatedCameraRotation.InverseRotatePoint(destinationCenter);

		// Apply camera zoom
		destinationCenter *= configuration_.InterpolatedCameraZoom;

		// Transform to render target
		for (size_t i = 0; i < SpriteMesh::VERTEX_COUNT; i++)
		{
			// Scale vertex
			Vec2f destination = vertices[i].Position * interpolatedTransform.Scale;

			// Rotate vertex
			RotatePointUnchecked(xAxis, destination);

			// Translate vertex
			destination += destinationCenter;

			// Camera space -> render target (SDL is Y-down)
			destination.X = configuration_.RenderTargetCenter.X + destination.X;
			destination.Y = configuration_.RenderTargetCenter.Y - destination.Y;

			vertexBatch_.emplace_back(destination, vertices[i].Color, vertices[i].UV);
		}
	}


	void SpriteMeshRenderer2D::Render(const Sprite2DNode& node) 
	{
		const Vertex2D* const vertices = node.Mesh->Vertices;
		const Sprite2DNode* parent = &node;
		const double interpolationAlpha = static_cast<double>(configuration_.InterpolationAlpha); // Convert to double for use

		Vec2 vertexPoints[SpriteMesh::VERTEX_COUNT] = { Vec2(vertices[0].Position), Vec2(vertices[1].Position),
							     Vec2(vertices[2].Position), Vec2(vertices[3].Position) };

		Vec2 prevVertexPoints[SpriteMesh::VERTEX_COUNT] = { vertexPoints[0], vertexPoints[1], vertexPoints[2], vertexPoints[3] };

		// Transform to world space:
		while (parent != nullptr)
		{
			parent->Transform.TransformPoints(vertexPoints, SpriteMesh::VERTEX_COUNT);
			parent->GetPreviousTransform().TransformPoints(prevVertexPoints, SpriteMesh::VERTEX_COUNT);
			parent = parent->GetParent();
		}

		// Transform to render target:
		for (size_t i = 0; i < SpriteMesh::VERTEX_COUNT; i++)
		{
			// Interpolate world position:
			vertexPoints[i] = GetInterpolatedUnchecked(prevVertexPoints[i], vertexPoints[i], interpolationAlpha);

			// Compute distance-to-camera:
			Vec2f destination = Vec2f(vertexPoints[i] - configuration_.InterpolatedCameraPosition);

			// Apply camera rotation to distance-to-camera:
			configuration_.InterpolatedCameraRotation.InverseRotatePoint(destination);

			// Apply camera zoom to distance-to-camera:
			destination *= configuration_.InterpolatedCameraZoom;

			// Position on render target:
			destination.X = configuration_.RenderTargetCenter.X + destination.X;
			destination.Y = configuration_.RenderTargetCenter.Y - destination.Y;

			//destination.X = std::roundf(destination.X); // Snapping for pixel art games
			//destination.Y = std::roundf(destination.Y);

			// Add to vertices batch:
			vertexBatch_.emplace_back(destination, vertices[i].Color, vertices[i].UV);
		}

	}

	void SpriteMeshRenderer2D::RenderFast(const Sprite2DNode& node)
	{
		if (!node.Mesh) return;

		const Vertex2D* const vertices = node.Mesh->Vertices;

		Transform2D globalTransform = node.GetGlobalTransform();
		Transform2D prevGlobalTransform = node.GetPrevGlobalTransform();

		// Interpolate global transform
		globalTransform = GetInterpolated(prevGlobalTransform, globalTransform, configuration_.InterpolationAlpha);

		// Precompute total (interpolated) scale for vertices
		globalTransform.Scale *= configuration_.InterpolatedCameraZoom; // Misusing as a temporary for total scale is fine here 

		// Precompute total (interpolated) rotation for vertices
		Vec2f xAxis = globalTransform.Rotation.GetXAxis();
		configuration_.InterpolatedCameraRotation.InverseRotatePoint(xAxis); // Rotation of raw X axis avoids normalization step

		// World -> camera space of mesh center (in camera space float provides sufficient precision)
		Vec2f destinationCenter = Vec2f(globalTransform.Position - configuration_.InterpolatedCameraPosition);

		// Apply inverse camera rotation
		configuration_.InterpolatedCameraRotation.InverseRotatePoint(destinationCenter);

		// Apply camera zoom
		destinationCenter *= configuration_.InterpolatedCameraZoom;

		// Transform to render target
		for (size_t i = 0; i < SpriteMesh::VERTEX_COUNT; i++)
		{
			// Scale vertex
			Vec2f destination = vertices[i].Position * globalTransform.Scale;

			// Rotate vertex
			RotatePointUnchecked(xAxis, destination);

			// Translate vertex
			destination += destinationCenter;

			// Camera space -> render target (SDL is Y-down)
			destination.X = configuration_.RenderTargetCenter.X + destination.X;
			destination.Y = configuration_.RenderTargetCenter.Y - destination.Y;

			vertexBatch_.emplace_back(destination, vertices[i].Color, vertices[i].UV);
		}
	}

	void SpriteMeshRenderer2D::RenderLine(const SpriteMesh& mesh, const Vec2& startPoint, const Vec2& endPoint, float lineWidth) 
	{
		const Vertex2D* const vertices = mesh.Vertices;

		// Compute distance-to-camera (in camera space float should provide sufficient precision):
		Vec2f transformedStartPoint = Vec2f(startPoint - configuration_.InterpolatedCameraPosition);
		Vec2f transformedEndPoint = Vec2f(endPoint - configuration_.InterpolatedCameraPosition);

		// Apply camera rotation:
		configuration_.InterpolatedCameraRotation.InverseRotatePoint(transformedStartPoint);
		configuration_.InterpolatedCameraRotation.InverseRotatePoint(transformedEndPoint);

		// Apply camera zoom:
		transformedStartPoint *= configuration_.InterpolatedCameraZoom;
		transformedEndPoint *= configuration_.InterpolatedCameraZoom;

		// Transform to screen space:
		transformedStartPoint.X = configuration_.RenderTargetCenter.X + transformedStartPoint.X;
		transformedStartPoint.Y = configuration_.RenderTargetCenter.Y - transformedStartPoint.Y;

		transformedEndPoint.X = configuration_.RenderTargetCenter.X + transformedEndPoint.X;
		transformedEndPoint.Y = configuration_.RenderTargetCenter.Y - transformedEndPoint.Y;

		const Vec2f scaledDirectionVector = (transformedStartPoint - transformedEndPoint).Normalize() * (lineWidth * 0.5f);
		const Vec2f scaledDirectionVectorNormal = scaledDirectionVector.GetNormal();

		Vec2f transformedVertex = transformedStartPoint - scaledDirectionVectorNormal;
		vertexBatch_.emplace_back(transformedVertex, vertices[0].Color, vertices[0].UV);

		transformedVertex = transformedEndPoint - scaledDirectionVectorNormal;
		vertexBatch_.emplace_back(transformedVertex, vertices[1].Color, vertices[1].UV);

		transformedVertex = transformedEndPoint + scaledDirectionVectorNormal;
		vertexBatch_.emplace_back(transformedVertex, vertices[2].Color, vertices[2].UV);

		transformedVertex = transformedStartPoint + scaledDirectionVectorNormal;
		vertexBatch_.emplace_back(transformedVertex, vertices[3].Color, vertices[3].UV);
	}





	void SpriteMeshRenderer2D::RenderPixel(const SpriteMesh& mesh, const Vec2& point, float pointWidth) 
	{
		const Vertex2D* const vertices = mesh.Vertices;

		// Compute distance-to-camera (in camera space float should provide sufficient precision):
		Vec2f transformedPoint = Vec2f(point - configuration_.InterpolatedCameraPosition);

		// Apply camera rotation:
		configuration_.InterpolatedCameraRotation.InverseRotatePoint(transformedPoint);

		// Apply camera zoom:
		transformedPoint *= configuration_.InterpolatedCameraZoom;

		// Transform to screen space:
		transformedPoint.X = configuration_.RenderTargetCenter.X + transformedPoint.X;
		transformedPoint.Y = configuration_.RenderTargetCenter.Y - transformedPoint.Y;

		pointWidth *= 0.5f; // pointWidth is now the approximate radius around the actual point

		Vec2f transformedVertex = Vec2f(transformedPoint.X - pointWidth, transformedPoint.Y - pointWidth);
		vertexBatch_.emplace_back(transformedVertex, vertices[0].Color, vertices[0].UV);

		transformedVertex = Vec2f(transformedPoint.X + pointWidth, transformedPoint.Y - pointWidth);
		vertexBatch_.emplace_back(transformedVertex, vertices[1].Color, vertices[1].UV);

		transformedVertex = Vec2f(transformedPoint.X + pointWidth, transformedPoint.Y + pointWidth);
		vertexBatch_.emplace_back(transformedVertex, vertices[2].Color, vertices[2].UV);

		transformedVertex = Vec2f(transformedPoint.X - pointWidth, transformedPoint.Y + pointWidth);
		vertexBatch_.emplace_back(transformedVertex, vertices[3].Color, vertices[3].UV);
	}



	/*
	void QuadMesh2DRenderer2D::RenderPixel(const QuadMesh2D& mesh, const Vector2d& point, float pointWidth)  //Perfekte Precision dank manuellen snapping
	{
		const Vertex2D* const vertices = mesh.Vertices;

		// Compute distance-to-camera (in camera space float should provide sufficient precision):
		Vector2f transformedPoint = Vector2f(point - configuration_.InterpolatedCameraPosition);

		// Apply camera rotation:
		transformedPoint = configuration_.InterpolatedInversedCameraRotation.RotatePoint(transformedPoint);

		// Apply camera zoom:
		transformedPoint *= configuration_.InterpolatedCameraZoom;

		// Transform to screen space:
		transformedPoint.X = std::floor(configuration_.RenderTargetCenter.X + transformedPoint.X);
		transformedPoint.Y = std::ceil(configuration_.RenderTargetCenter.Y - transformedPoint.Y);

		Vector2f transformedVertex = Vector2f(transformedPoint.X, transformedPoint.Y - pointWidth);
		vertexBatch_.emplace_back(transformedVertex, vertices[0].Color, vertices[0].TexCoordinates);

		transformedVertex = Vector2f(transformedPoint.X + pointWidth, transformedPoint.Y - pointWidth);
		vertexBatch_.emplace_back(transformedVertex, vertices[1].Color, vertices[1].TexCoordinates);

		transformedVertex = Vector2f(transformedPoint.X + pointWidth, transformedPoint.Y);
		vertexBatch_.emplace_back(transformedVertex, vertices[2].Color, vertices[2].TexCoordinates);

		vertexBatch_.emplace_back(transformedPoint, vertices[3].Color, vertices[3].TexCoordinates);
	}
	*/

	void SpriteMeshRenderer2D::BeginBatch(const MovableObject2D* camera, float interpolationAlpha, const Vec2f& renderTargetCenter) 
	{
		vertexBatch_.clear();

		interpolationAlpha = GetClamped(interpolationAlpha, 0.0f, 1.0f);

		configuration_.InterpolatedCameraPosition = { 0.0, 0.0 };
		configuration_.InterpolatedCameraZoom = { 1.0f, 1.0f };
		configuration_.InterpolatedCameraRotation.SetToIdentity();
		configuration_.InterpolationAlpha = interpolationAlpha;
		configuration_.RenderTargetCenter.X = renderTargetCenter.X;
		configuration_.RenderTargetCenter.Y = Renderer::Get().GetLogicalResolutionHeight() - renderTargetCenter.Y;

		if (camera != nullptr)
		{
			const Vec2& position = camera->Transform.Position;
			const Vec2& previousPosition = camera->GetPreviousTransform().Position;
			const Vec2f& zoom = camera->Transform.Scale;
			const Vec2f& previousZoom = camera->GetPreviousTransform().Scale;
			const Rotation2D& rotation = camera->Transform.Rotation;
			const Rotation2D& previousRotation = camera->GetPreviousTransform().Rotation;

			configuration_.InterpolatedCameraPosition = GetInterpolatedUnchecked(previousPosition, position, static_cast<double>(interpolationAlpha));
			configuration_.InterpolatedCameraZoom = GetInterpolatedUnchecked(previousZoom, zoom, interpolationAlpha);
			configuration_.InterpolatedCameraRotation = GetInterpolated(previousRotation, rotation, interpolationAlpha);
		}
	}

	void SpriteMeshRenderer2D::RenderBatch(const Texture& boundTexture, TargetTexture* renderTarget) 
	{
		UpdateVertexIndices();

		const Vertex2D* const vertexArray = vertexBatch_.data();

		Renderer& renderer = Renderer::Get();

		renderer.SetRenderTarget(renderTarget);

		Vec2f cachedRenderScale = renderer.GetRenderScale(); // Cache current render scale

		if (renderTarget != nullptr)
		{
			int width, height;
			renderTarget->GetSize(width, height);

			renderer.SetRenderScale(float(width) / renderer.GetLogicalResolutionWidth(), float(height) / renderer.GetLogicalResolutionHeight());
		}
		
		constexpr int stride = sizeof(Vertex2D);

		renderer.RenderGeometryRaw(boundTexture, &(vertexArray->Position.X), stride, &(vertexArray->Color), stride, &(vertexArray->UV.X), stride, vertexBatch_.size(),
			vertexIndices_.data(), (vertexBatch_.size() * 3) / 2, 4);

		renderer.SetRenderScale(cachedRenderScale.X, cachedRenderScale.Y); // Restore cached render scale
	}

	// First triangle: 0-3-1
	// Second triangle: 1-3-2
	// Vertex layout:
	// 0--1
	// |  |
	// 3--2
	void SpriteMeshRenderer2D::UpdateVertexIndices() 
	{
		if (vertexIndices_.size() >= (vertexBatch_.size() * 3) / 2)
			return;

		const int finalSpriteIndex = vertexBatch_.size() - 4;

		const int startIndex = (vertexIndices_.size() * 2) / 3;

		for (int i = startIndex; i <= finalSpriteIndex; i += 4)
		{
			vertexIndices_.push_back(i);
			vertexIndices_.push_back(i + 3);
			vertexIndices_.push_back(i + 1);

			vertexIndices_.push_back(i + 1);
			vertexIndices_.push_back(i + 3);
			vertexIndices_.push_back(i + 2);
		}
	}

}