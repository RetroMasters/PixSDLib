#include "TriangleMesh2DRenderer2D.h"
#include "Renderer.h"

#include "PixMath.h"

namespace pix
{

	TriangleMesh2DRenderer2D::TriangleMesh2DRenderer2D(int initialVertexBatchSize)  :
		configuration_(),
		vertexBatch_(),
		pointBuffer1_(),
		pointBuffer2_()
	{
		if (initialVertexBatchSize > 0)
			vertexBatch_.reserve(initialVertexBatchSize);
	}

	void TriangleMesh2DRenderer2D::Render(const TriangleMesh2D& mesh, const Transform2D& transform) 
	{
		const std::vector<Vertex2DEx>& vertices = mesh.Vertices;
		const int vertexCount = vertices.size();

		// Compute distance-to-camera (in camera space float should provide sufficient precision):
		Vec2f destinationCenter = Vec2f(transform.Position - configuration_.InterpolatedCameraPosition);

		// Apply camera rotation to distance-to-camera:
		configuration_.InterpolatedInversedCameraRotation.RotatePoint(destinationCenter);

		// Scale/Zoom the rotated distance-to-camera:
		destinationCenter *= configuration_.InterpolatedCameraZoom;

		// Precompute total scale for vertices: 
		const Vec2f scale = transform.Scale * configuration_.InterpolatedCameraZoom;

		// Precompute total (interpolated) rotation for vertices:
		Vec2f xAxis = transform.Rotation.GetXAxis();
		configuration_.InterpolatedInversedCameraRotation.RotatePoint(xAxis);

		for (int i = 0; i < vertexCount; i++)
		{
			// Scale the current mesh vertex:
			Vec2f destination = vertices[i].Position * scale;

			// Rotate the current mesh vertex:
			RotatePointUnchecked(xAxis, destination);

			// Translate to (scaled) camera space:
			destination += destinationCenter;

			// Position on render target:
			destination.X = configuration_.RenderTargetCenter.X + destination.X;
			destination.Y = configuration_.RenderTargetCenter.Y - destination.Y;

			vertexBatch_.emplace_back(destination, vertices[i].Color, vertices[i].UV);
		}
	}


	void TriangleMesh2DRenderer2D::Render(const Sprite2DEx& sprite) 
	{
		const std::vector<Vertex2DEx>& vertices = sprite.Mesh->Vertices; //reference to vertices for convenience
		const int vertexCount = vertices.size();

		Vec2 position = sprite.Transform.Position;
		const Vec2& previousPosition = sprite.GetPreviousTransform().Position;
		Vec2f scale = sprite.Transform.Scale;
		const Vec2f& previousScale = sprite.GetPreviousTransform().Scale;
		Vec2f xAxis = sprite.Transform.Rotation.GetXAxis();
		const Vec2f& previousXAxis = sprite.GetPreviousTransform().Rotation.GetXAxis();

		position = GetInterpolatedUnchecked(previousPosition, position, static_cast<double>(configuration_.InterpolationAlpha));

		// Precompute total (interpolated) scale vor vertices:
		scale = GetInterpolatedUnchecked(previousScale, scale, configuration_.InterpolationAlpha);
		scale *= configuration_.InterpolatedCameraZoom;

		// Precompute total (interpolated) rotation for vertices:
		xAxis = GetInterpolatedUnchecked(previousXAxis, xAxis, configuration_.InterpolationAlpha);
		configuration_.InterpolatedInversedCameraRotation.RotatePoint(xAxis);

		// Compute distance-to-camera (in camera space float should provide sufficient precision):
		Vec2f destinationCenter = Vec2f(position - configuration_.InterpolatedCameraPosition);

		// Apply camera rotation to distance-to-camera:
		configuration_.InterpolatedInversedCameraRotation.RotatePoint(destinationCenter);

		// Apply camera zoom to distance-to-camera:
		destinationCenter *= configuration_.InterpolatedCameraZoom;


		for (int i = 0; i < vertexCount; i++)
		{
			// Scale the current mesh vertex:
			Vec2f destination = vertices[i].Position * scale.X;

			// Rotate the current mesh vertex:
			RotatePointUnchecked(xAxis, destination);

			// Translate to (scaled) camera space:
			destination += destinationCenter;

			// Position on render target:
			destination.X = configuration_.RenderTargetCenter.X + destination.X;
			destination.Y = configuration_.RenderTargetCenter.Y - destination.Y;

			vertexBatch_.emplace_back(destination, vertices[i].Color, vertices[i].UV);
		}
	}



	void TriangleMesh2DRenderer2D::Render(const Sprite2DExNode& node) 
	{
		const Sprite2DExNode* parent = &node;
		const std::vector<Vertex2DEx>& vertices = (node.Mesh)->Vertices;
		const int vertexCount = vertices.size();
		const double interpolationAlphaD = static_cast<double>(configuration_.InterpolationAlpha); // Convert to double for later use

		pointBuffer1_.clear(); // stores the vertex points
		pointBuffer2_.clear(); // stores the previous vertex points

		for (int i = 0; i < vertexCount; i++)
			pointBuffer1_.emplace_back(vertices[i].Position.X, vertices[i].Position.Y);

		pointBuffer2_ = pointBuffer1_;

		// Transform to world space:
		while (parent != nullptr)
		{
			parent->Transform.TransformPoints(pointBuffer1_.data(), pointBuffer1_.size());
			parent->GetPreviousTransform().TransformPoints(pointBuffer2_.data(), pointBuffer2_.size());
			parent = parent->GetParent();
		}

		// Transform to render target:
		for (int i = 0; i < vertexCount; i++)
		{
			// Interpolate world position:
			pointBuffer1_[i] = GetInterpolatedUnchecked(pointBuffer2_[i], pointBuffer1_[i], interpolationAlphaD);

			// Transform to camera space:
			Vec2f destination = Vec2f(pointBuffer1_[i] - configuration_.InterpolatedCameraPosition);

			// Apply camera rotation to distance-to-camera:
			configuration_.InterpolatedInversedCameraRotation.RotatePoint(destination);

			// Apply camera zoom:
			destination *= configuration_.InterpolatedCameraZoom;

			// Position on render target:
			destination.X = configuration_.RenderTargetCenter.X + destination.X;
			destination.Y = configuration_.RenderTargetCenter.Y - destination.Y;

			vertexBatch_.emplace_back(destination, vertices[i].Color, vertices[i].UV);
		}
	}


	/*
	void Mesh2DRenderer2D::Render(const Sprite2DExNode& node)
	{
		const Sprite2DExNode* parent = &node;
		const std::vector<Vertex2DEx>& vertices = (node.Mesh)->Vertices;
		int vertexCount = vertices.size();

		vertexBuffer1_.clear(); // stores the vertex points
		vertexBuffer2_.clear(); // stores the previous vertex points

		vertexBuffer1_ = vertices;
		vertexBuffer2_ = vertexBuffer1_;

		// Transform to world space:
		while (parent != nullptr)
		{
			ApplyTransform(parent->Transform, vertexBuffer1_.data(), vertexBuffer1_.size());
			ApplyTransform(parent->GetPrevTransform(), vertexBuffer2_.data(), vertexBuffer2_.size());
			parent = parent->GetParent();
		}

		// Transform to camera space:
		for (int i = 0; i < vertexCount; i++)
		{
			// Interpolate world position:
			vertexBuffer1_[i].Position = vertexBuffer2_[i].Position + ((vertexBuffer1_[i].Position - vertexBuffer2_[i].Position) * configuration_.Interpolation);

			// Transform to camera space:
			Vector2f destination = vertexBuffer1_[i].Position - configuration_.InterpolatedCamPosition;

			// Apply camera rotation to distance-to-camera:
			RotatePoint(configuration_.InterpolatedCamXAxis, destination);

			// Apply camera zoom:
			destination = { destination.X * configuration_.InterpolatedCamZoom.X, destination.Y * configuration_.InterpolatedCamZoom.Y };
			destination.X = configuration_.RenderCenter.X + destination.X;
			destination.Y = configuration_.RenderCenter.Y - destination.Y;

			// Achieve resolution independence: Scale from logical size to the current target resolution (as logical size is only applied to the "null-target" in SDL2).
			//destination *= configuration_.RenderScale;

			vertexBatch_.emplace_back(destination, vertices[i].Color, vertices[i].TexCoordinates);
		}
	}
	*/






	void TriangleMesh2DRenderer2D::BeginBatch(const MovableObject2D* camera, float interpolationAlpha, const Vec2f& renderTargetCenter) 
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
			const Vec2& position = camera->Transform.Position;
			const Vec2& previousPosition = camera->GetPreviousTransform().Position;
			const Vec2f& zoom = camera->Transform.Scale;
			const Vec2f& previousZoom = camera->GetPreviousTransform().Scale;
			const Rotation2D& rotation = camera->Transform.Rotation;
			const Rotation2D& previousRotation = camera->GetPreviousTransform().Rotation;

			configuration_.InterpolatedCameraPosition = GetInterpolatedUnchecked(previousPosition, position, static_cast<double>(interpolationAlpha));
			configuration_.InterpolatedCameraZoom = GetInterpolatedUnchecked(previousZoom, zoom, interpolationAlpha);
			configuration_.InterpolatedInversedCameraRotation = GetInterpolated(previousRotation, rotation, interpolationAlpha).Inverse();
		}
	}



	void TriangleMesh2DRenderer2D::RenderBatch(const Texture& boundTexture, TargetTexture* renderTarget) 
	{
		//SDL_RenderGeometry(SDLRenderer::Get(), configuration_.BoundTexture->Get(), verticesBatch_.data(), verticesBatch_.size(), nullptr, 0);

		const Vertex2D* const vertexArray = vertexBatch_.data();

		Renderer& systemRenderer = Renderer::Get();

		systemRenderer.SetRenderTarget(renderTarget);

		int width, height;
		renderTarget->GetSize(width, height);

		if (renderTarget != nullptr)
			systemRenderer.SetRenderScale(float(width) / systemRenderer.GetLogicalResolutionWidth(),
				float(height) / systemRenderer.GetLogicalResolutionHeight());

		//SDL_RenderGeometryRaw(SystemRenderer::Get().GetSDLRenderer(), configuration_.BoundTexture->Get(), &(vertexArray->Position.X), 20, &(vertexArray->Color), 20, &(vertexArray->TexCoordinates.X), 20, verticesBatch_.size(), nullptr, 0, 4);

		systemRenderer.RenderGeometryRaw(boundTexture, &(vertexArray->Position.X), 20, &(vertexArray->Color), 20, &(vertexArray->UV.X), 20, vertexBatch_.size(), nullptr, 0, 4);
	}

}


