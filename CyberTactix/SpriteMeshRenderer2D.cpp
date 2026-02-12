#include "SpriteMeshRenderer2D.h"
#include "Renderer.h"


namespace pix
{

	SpriteMeshRenderer2D::SpriteMeshRenderer2D(int initialVertexBatchSize)  :
		configuration_(),
		vertexBatch_(),
		vertexIndices_()
	{
		vertexBatch_.reserve(initialVertexBatchSize);
		vertexIndices_.reserve((initialVertexBatchSize * 3) / 2);
	}


	void SpriteMeshRenderer2D::Render(const SpriteMesh& mesh, const Transform2D& transform) 
	{
		const Vertex2D* const vertices = mesh.Vertices;

		// Compute distance-to-camera (in camera space float should provide sufficient precision):
		Vector2f destinationCenter = Vector2f(transform.Position - configuration_.InterpolatedCameraPosition);

		// Apply camera rotation to distance-to-camera:
		destinationCenter = configuration_.InterpolatedInversedCameraRotation.RotatePoint(destinationCenter);

		// Apply camera zoom to distance-to-camera:
		destinationCenter *= configuration_.InterpolatedCameraZoom;

		//destinationCenter.X = std::roundf(destinationCenter.X); // Snapping pixel art games
		//destinationCenter.Y = std::roundf(destinationCenter.Y);

		// Precompute total (interpolated) scale vor vertices:
		const Vector2f scale = transform.Scale * configuration_.InterpolatedCameraZoom;

		// Precompute total (interpolated) rotation for vertices:
		Vector2f xAxis = transform.Rotation.GetXAxis();
		xAxis = configuration_.InterpolatedInversedCameraRotation.RotatePoint(xAxis);

		for (int i = 0; i < 4; i++)
		{
			// Scale the current mesh vertex:
			Vector2f destination = vertices[i].Position * scale;

			// Rotate the current mesh vertex:
			destination = RotatePointRaw(xAxis, destination);

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
		const Vertex2D* const vertices = sprite.Mesh->Vertices;

		Vector2d pos = sprite.Transform.Position;
		const Vector2d& prevPos = sprite.GetPreviousTransform().Position;
		Vector2f scale = sprite.Transform.Scale;
		const Vector2f& prevScale = sprite.GetPreviousTransform().Scale;
		Vector2f xAxis = sprite.Transform.Rotation.GetXAxis();
		const Vector2f& prevXAxis = sprite.GetPreviousTransform().Rotation.GetXAxis();

		pos = InterpolateRaw(prevPos, pos, static_cast<double>(configuration_.InterpolationAlpha));
		scale = InterpolateRaw(prevScale, scale, configuration_.InterpolationAlpha);
		xAxis = InterpolateRaw(prevXAxis, xAxis, configuration_.InterpolationAlpha);

		// Precompute total (interpolated) scale vor vertices:
		scale *= configuration_.InterpolatedCameraZoom;

		// Precompute total (interpolated) rotation for vertices:
		xAxis = configuration_.InterpolatedInversedCameraRotation.RotatePoint(xAxis);

		// Compute distance-to-camera (in camera space float should provide sufficient precision):
		Vector2f destinationCenter = Vector2f(pos - configuration_.InterpolatedCameraPosition);

		// Apply camera rotation to distance-to-camera:
		destinationCenter = configuration_.InterpolatedInversedCameraRotation.RotatePoint(destinationCenter);

		// Apply camera zoom to distance-to-camera:
		destinationCenter *= configuration_.InterpolatedCameraZoom;

		//destinationCenter.X = std::roundf(destinationCenter.X); // Snapping pixel art games
		//destinationCenter.Y = std::roundf(destinationCenter.Y);

		for (int i = 0; i < 4; i++)
		{
			// Scale the current mesh vertex:
			Vector2f destination = vertices[i].Position * scale;

			// Rotate the current mesh vertex:
			destination = RotatePointRaw(xAxis, destination);

			// Translate to (scaled) camera space:
			destination += destinationCenter;

			// Position on render target:
			destination.X = configuration_.RenderTargetCenter.X + destination.X;
			destination.Y = configuration_.RenderTargetCenter.Y - destination.Y;

			// Achieve resolution independence: Scale from logical size to the current target resolution (as logical size is only applied to the "null-target" in SDL2).
			//destination *= configuration_.RenderScale;  

			vertexBatch_.emplace_back(destination, vertices[i].Color, vertices[i].UV);

			//verticesBatch_.push_back( Vertex2D(configuration_.RenderCenter.X + destination.X, configuration_.RenderCenter.Y - destination.Y, vertices[i].color, vertices[i].tex_coord.x, vertices[i].tex_coord.y));
		}
	}




	


	void SpriteMeshRenderer2D::Render(const Sprite2DNode& node) 
	{
		const Vertex2D* const vertices = node.Mesh->Vertices;
		const Sprite2DNode* parent = &node;
		const double interpolationAlphaD = static_cast<double>(configuration_.InterpolationAlpha); // Convert to double for later use

		Vector2d vertexPoints[4] = { static_cast<Vector2d>(vertices[0].Position),  static_cast<Vector2d>(vertices[1].Position),
									 static_cast<Vector2d>(vertices[2].Position),  static_cast<Vector2d>(vertices[3].Position) };

		Vector2d previousVertexPoints[4] = { vertexPoints[0], vertexPoints[1], vertexPoints[2], vertexPoints[3] };


		// Transform to world space:
		while (parent != nullptr)
		{
			parent->Transform.ApplyToPoints(vertexPoints, 4);
			parent->GetPreviousTransform().ApplyToPoints(previousVertexPoints, 4);
			parent = parent->GetParent();
		}

		// Transform to render target:
		for (int i = 0; i < 4; i++)
		{
			// Interpolate world position:
			vertexPoints[i] = InterpolateRaw(previousVertexPoints[i], vertexPoints[i], interpolationAlphaD);

			// Compute distance-to-camera:
			Vector2f destination = Vector2f(vertexPoints[i] - configuration_.InterpolatedCameraPosition);

			// Apply camera rotation to distance-to-camera:
			destination = configuration_.InterpolatedInversedCameraRotation.RotatePoint(destination);

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

	

	void SpriteMeshRenderer2D::Render(const SpriteMesh& mesh, const Vector2d& startPoint, const Vector2d& endPoint, float lineWidth) 
	{
		const Vertex2D* const vertices = mesh.Vertices;

		// Compute distance-to-camera (in camera space float should provide sufficient precision):
		Vector2f transformedStartPoint = Vector2f(startPoint - configuration_.InterpolatedCameraPosition);
		Vector2f transformedEndPoint = Vector2f(endPoint - configuration_.InterpolatedCameraPosition);

		// Apply camera rotation:
		transformedStartPoint = configuration_.InterpolatedInversedCameraRotation.RotatePoint(transformedStartPoint);
		transformedEndPoint = configuration_.InterpolatedInversedCameraRotation.RotatePoint(transformedEndPoint);

		// Apply camera zoom:
		transformedStartPoint *= configuration_.InterpolatedCameraZoom;
		transformedEndPoint *= configuration_.InterpolatedCameraZoom;

		// Transform to screen space:
		transformedStartPoint.X = configuration_.RenderTargetCenter.X + transformedStartPoint.X;
		transformedStartPoint.Y = configuration_.RenderTargetCenter.Y - transformedStartPoint.Y;

		transformedEndPoint.X = configuration_.RenderTargetCenter.X + transformedEndPoint.X;
		transformedEndPoint.Y = configuration_.RenderTargetCenter.Y - transformedEndPoint.Y;

		const Vector2f scaledDirectionVector = (transformedStartPoint - transformedEndPoint).Normalize() * (lineWidth * 0.5f);
		const Vector2f scaledDirectionVectorNormal = scaledDirectionVector.GetNormal();

		Vector2f transformedVertex = transformedStartPoint - scaledDirectionVectorNormal;
		vertexBatch_.emplace_back(transformedVertex, vertices[0].Color, vertices[0].UV);

		transformedVertex = transformedEndPoint - scaledDirectionVectorNormal;
		vertexBatch_.emplace_back(transformedVertex, vertices[1].Color, vertices[1].UV);

		transformedVertex = transformedEndPoint + scaledDirectionVectorNormal;
		vertexBatch_.emplace_back(transformedVertex, vertices[2].Color, vertices[2].UV);

		transformedVertex = transformedStartPoint + scaledDirectionVectorNormal;
		vertexBatch_.emplace_back(transformedVertex, vertices[3].Color, vertices[3].UV);
	}





	void SpriteMeshRenderer2D::RenderPixel(const SpriteMesh& mesh, const Vector2d& point, float pointWidth) 
	{
		const Vertex2D* const vertices = mesh.Vertices;

		// Compute distance-to-camera (in camera space float should provide sufficient precision):
		Vector2f transformedPoint = Vector2f(point - configuration_.InterpolatedCameraPosition);

		// Apply camera rotation:
		transformedPoint = configuration_.InterpolatedInversedCameraRotation.RotatePoint(transformedPoint);

		// Apply camera zoom:
		transformedPoint *= configuration_.InterpolatedCameraZoom;

		// Transform to screen space:
		transformedPoint.X = configuration_.RenderTargetCenter.X + transformedPoint.X;
		transformedPoint.Y = configuration_.RenderTargetCenter.Y - transformedPoint.Y;

		pointWidth *= 0.5f; // pointWidth is now the approximate radius around the actual point

		Vector2f transformedVertex = Vector2f(transformedPoint.X - pointWidth, transformedPoint.Y - pointWidth);
		vertexBatch_.emplace_back(transformedVertex, vertices[0].Color, vertices[0].UV);

		transformedVertex = Vector2f(transformedPoint.X + pointWidth, transformedPoint.Y - pointWidth);
		vertexBatch_.emplace_back(transformedVertex, vertices[1].Color, vertices[1].UV);

		transformedVertex = Vector2f(transformedPoint.X + pointWidth, transformedPoint.Y + pointWidth);
		vertexBatch_.emplace_back(transformedVertex, vertices[2].Color, vertices[2].UV);

		transformedVertex = Vector2f(transformedPoint.X - pointWidth, transformedPoint.Y + pointWidth);
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








	void SpriteMeshRenderer2D::BeginBatch(const MovableObject2D* camera, float interpolationAlpha, const Vector2f& renderTargetCenter) 
	{
		vertexBatch_.clear();

		if (interpolationAlpha > 1.0f)      interpolationAlpha = 1.0f;
		else if (interpolationAlpha < 0.0f) interpolationAlpha = 0.0f;

		configuration_.InterpolatedCameraPosition = { 0.0, 0.0 };
		configuration_.InterpolatedCameraZoom = { 1.0f, 1.0f };
		configuration_.InterpolatedInversedCameraRotation.SetToIdentity();
		configuration_.InterpolationAlpha = interpolationAlpha;
		configuration_.RenderTargetCenter.X = renderTargetCenter.X;
		configuration_.RenderTargetCenter.Y = Renderer::Get().GetLogicalResolutionHeight() - renderTargetCenter.Y;

		if (camera != nullptr)
		{
			const Vector2d& position = camera->Transform.Position;
			const Vector2d& previousPosition = camera->GetPreviousTransform().Position;
			const Vector2f& zoom = camera->Transform.Scale;
			const Vector2f& previousZoom = camera->GetPreviousTransform().Scale;
			const Rotation2D& rotation = camera->Transform.Rotation;
			const Rotation2D& previousRotation = camera->GetPreviousTransform().Rotation;

			configuration_.InterpolatedCameraPosition = InterpolateRaw(previousPosition, position, static_cast<double>(interpolationAlpha));
			configuration_.InterpolatedCameraZoom = InterpolateRaw(previousZoom, zoom, interpolationAlpha);
			configuration_.InterpolatedInversedCameraRotation = Interpolate(previousRotation, rotation, interpolationAlpha).Inverse();
		}
	}

	void SpriteMeshRenderer2D::RenderBatch(const Texture& boundTexture, TargetTexture* renderTarget) 
	{
		UpdateVertexIndices();

		//SDL_RenderGeometry(SDLRenderer::Get(), configuration_.BoundTexture->Get(), verticesBatch_.data(), verticesBatch_.size(), vertexIndexArray_.data(), (verticesBatch_.size() * 3) / 2);

		const Vertex2D* const vertexArray = vertexBatch_.data();

		Renderer& systemRenderer = Renderer::Get();

		systemRenderer.SetRenderTarget(renderTarget);

		int width, height;
		renderTarget->GetSize(width, height);

		if (renderTarget != nullptr)
			systemRenderer.SetRenderScale(float(width) / systemRenderer.GetLogicalResolutionWidth(),
				float(height) / systemRenderer.GetLogicalResolutionHeight());

		//SDL_RenderGeometryRaw(SystemRenderer::Get().GetSDLRenderer(),configuration_.BoundTexture->Get(), &(vertexArray->Position.X), 20, &(vertexArray->Color), 20, &(vertexArray->TexCoordinates.X), 20, verticesBatch_.size(),
		//	vertexIndexArray_.data(), (verticesBatch_.size() * 3) / 2, 4);

		systemRenderer.RenderGeometryRaw(boundTexture, &(vertexArray->Position.X), 20, &(vertexArray->Color), 20, &(vertexArray->UV.X), 20, vertexBatch_.size(),
			vertexIndices_.data(), (vertexBatch_.size() * 3) / 2, 4);
	}




	/// <summary>
	/// First triangle: 0-3-1
	/// Second triangle: 1-3-2
	/// Vertex layout:
	/// 0--1
	/// |  |
	/// 3--2
	/// </summary>
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