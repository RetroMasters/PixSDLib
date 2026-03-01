#include "TriangleMesh2DRenderer2D.h"
#include "Renderer.h"

namespace pix
{

	TriangleMesh2DRenderer2D::TriangleMesh2DRenderer2D(int initialVertexBatchSize)
	{
		if (initialVertexBatchSize > 0)
			vertexBatch_.reserve(initialVertexBatchSize);

		pointBuffer1_.reserve(6); // Typical sprite mesh (quad) = 6 vertices
		pointBuffer2_.reserve(6);
	}

	void TriangleMesh2DRenderer2D::Render(const TriangleMesh2D& mesh, const Transform2D& transform) 
	{
		const std::vector<Vertex2DEx>& vertices = mesh.Vertices; //reference to vertices for convenience
		const size_t vertexCount = vertices.size();

		// World -> camera space of mesh center (in camera space float provides sufficient precision)
		Vec2f destinationCenter = Vec2f(transform.Position - configuration_.InterpolatedCameraPosition);

		// Apply inverse camera rotation
		configuration_.InterpolatedCameraRotation.InverseRotatePoint(destinationCenter);

		// Apply camera zoom
		destinationCenter *= configuration_.InterpolatedCameraZoom;

		// Precompute total scale for vertices: 
		const Vec2f scale = transform.Scale * configuration_.InterpolatedCameraZoom;

		// Precompute total (interpolated) rotation for vertices:
		Vec2f xAxis = transform.Rotation.GetXAxis(); 
		configuration_.InterpolatedCameraRotation.InverseRotatePoint(xAxis); // Rotation of raw X axis avoids normalization step

		for (size_t i = 0; i < vertexCount; i++)
		{
			// Scale vertex
			Vec2f destination = vertices[i].Position * scale;

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

	

	void TriangleMesh2DRenderer2D::Render(const Sprite2DEx& sprite)
	{
		if (!sprite.Mesh) return;

		const std::vector<Vertex2DEx>& vertices = sprite.Mesh->Vertices;
		const size_t vertexCount = vertices.size();

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
		for (size_t i = 0; i < vertexCount; i++)
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

	void TriangleMesh2DRenderer2D::Render(const Sprite2DExNode& node)
	{
		if (!node.Mesh) return;

		const Sprite2DExNode* parent = &node;
		const std::vector<Vertex2DEx>& vertices = (node.Mesh)->Vertices;
		const size_t vertexCount = vertices.size();
		const double interpolationAlpha = (double)configuration_.InterpolationAlpha; // Convert to double for use

		pointBuffer1_.clear(); // Stores the vertex positions
		pointBuffer2_.clear(); // Stores the previous vertex positions

		if (pointBuffer1_.capacity() < vertexCount) 
		{
			pointBuffer1_.reserve(vertexCount);
			pointBuffer2_.reserve(vertexCount);
		}

		for (size_t i = 0; i < vertexCount; i++)
			pointBuffer1_.emplace_back(vertices[i].Position.X, vertices[i].Position.Y);

		pointBuffer2_ = pointBuffer1_;

		// Transform to world space
		while (parent != nullptr)
		{
			parent->Transform.TransformPoints(pointBuffer1_.data(), pointBuffer1_.size());
			parent->GetPreviousTransform().TransformPoints(pointBuffer2_.data(), pointBuffer2_.size());
			parent = parent->GetParent();
		}

		// Transform to render target
		for (size_t i = 0; i < vertexCount; i++)
		{
			// Interpolate world position
			pointBuffer1_[i] = GetInterpolatedUnchecked(pointBuffer2_[i], pointBuffer1_[i], interpolationAlpha);

			// World -> camera space (in camera space float provides sufficient precision)
			Vec2f destination = Vec2f(pointBuffer1_[i] - configuration_.InterpolatedCameraPosition);

			// Apply inverse camera rotation
			configuration_.InterpolatedCameraRotation.InverseRotatePoint(destination);

			// Apply camera zoom
			destination *= configuration_.InterpolatedCameraZoom;

			// Camera space -> render target (SDL is Y-down)
			destination.X = configuration_.RenderTargetCenter.X + destination.X;
			destination.Y = configuration_.RenderTargetCenter.Y - destination.Y; 

			vertexBatch_.emplace_back(destination, vertices[i].Color, vertices[i].UV);
		}
	}

	void TriangleMesh2DRenderer2D::RenderFast(const Sprite2DExNode& node)
	{
		if (!node.Mesh) return;

		const std::vector<Vertex2DEx>& vertices = (node.Mesh)->Vertices;
		const size_t vertexCount = vertices.size();

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
		for (size_t i = 0; i < vertexCount; i++)
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

	void TriangleMesh2DRenderer2D::BeginBatch(const MovableObject2D* camera, float interpolationAlpha, Vec2f renderTargetCenter) 
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

	void TriangleMesh2DRenderer2D::RenderBatch(const Texture& texture, TargetTexture* renderTarget) 
	{
		if (vertexBatch_.empty()) return;

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

		renderer.RenderGeometryRaw(texture, &(vertexArray->Position.X), stride, &(vertexArray->Color), stride, &(vertexArray->UV.X), stride, vertexBatch_.size(), nullptr, 0, 4);

		renderer.SetRenderScale(cachedRenderScale.X, cachedRenderScale.Y); // Restore cached render scale
	}

}


