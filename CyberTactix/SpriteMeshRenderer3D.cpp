#include "SpriteMeshRenderer3D.h"
#include "TriangleMesh2DRenderer2D.h" //SDL_GameControllerButton

namespace pix
{

	SpriteMeshRenderer3D::SpriteMeshRenderer3D(int initialVertexBatchSize)  :
		configuration_()
	{
		if (initialVertexBatchSize > 0)
			vertexBatch_.reserve(initialVertexBatchSize);
	}

	void SpriteMeshRenderer3D::Render(const SpriteMesh& mesh, const Transform3D& transform) 
	{
		const Vertex2D* const vertices = mesh.Vertices;

		const Vec3f meshCenterDistanceFromCamera = Vec3f(transform.Position - configuration_.InterpolatedCameraPosition);

		//Transform to camera space:
		for (int i = 0; i < 4; i++)
		{
			Vec3f vertexPoint = { vertices[i].Position.X, vertices[i].Position.Y, 0.0f };

			// Scale the mesh point:
			vertexPoint.X *= transform.Scale.X;
			vertexPoint.Y *= transform.Scale.Y; //ignore Z

			// Rotate the mesh point:
			vertexPoint = (transform.Rotation.GetXAxis() * vertexPoint.X) + (transform.Rotation.GetYAxis() * vertexPoint.Y); //ignore Z

			// Compute distance to camera:
			vertexPoint += meshCenterDistanceFromCamera;

			// Project to camera space:
			const float z = configuration_.InterpolatedCameraAxisZ.GetDotProduct(vertexPoint);
			if (z < minDistanceToCamera_)
			{
				for (int j = i; j > 0; j--) vertexBatch_.pop_back();
				return;
			}
			const float x = configuration_.InterpolatedInversedCameraRotation.GetXAxis().GetDotProduct(vertexPoint);
			const float y = configuration_.InterpolatedInversedCameraRotation.GetYAxis().GetDotProduct(vertexPoint);


			// Project to screen space:
			const Vec2f screenCoords = { configuration_.RenderTargetCenter.X + (x * configuration_.CameraDistanceToScreen) / z,
											configuration_.RenderTargetCenter.Y - (y * configuration_.CameraDistanceToScreen) / z };

			// Add final vertices to batch:
			vertexBatch_.emplace_back(screenCoords, vertices[i].Color, vertices[i].UV);
		}
	}



	void SpriteMeshRenderer3D::Render(const Sprite3D& sprite) 
	{
		const Vertex2D* const vertices = sprite.Mesh->Vertices;

		Vec3 pos = sprite.Transform.Position;
		const Vec3& prevPos = sprite.GetPreviousTransform().Position;
		Vec3f scale = sprite.Transform.Scale;
		const Vec3f& prevScale = sprite.GetPreviousTransform().Scale;
		Vec3f rotX = sprite.Transform.Rotation.GetXAxis();
		const Vec3f& prevRotX = sprite.GetPreviousTransform().Rotation.GetXAxis();
		Vec3f rotY = sprite.Transform.Rotation.GetYAxis();
		const Vec3f& prevRotY = sprite.GetPreviousTransform().Rotation.GetYAxis();

		pos = GetInterpolatedUnchecked(prevPos, pos, static_cast<double>(configuration_.InterpolationAlpha));
		scale = GetInterpolatedUnchecked(prevScale, scale, configuration_.InterpolationAlpha);
		rotX = GetInterpolatedUnchecked(prevRotX, rotX, configuration_.InterpolationAlpha);
		rotY = GetInterpolatedUnchecked(prevRotY, rotY, configuration_.InterpolationAlpha);

		const Vec3f meshCenterDistanceFromCamera = Vec3f(pos - configuration_.InterpolatedCameraPosition);

		//Transform to camera space:
		for (int i = 0; i < 4; i++)
		{
			Vec3f vertexPoint = { vertices[i].Position.X, vertices[i].Position.Y, 0.0f };

			// Scale the mesh point:
			vertexPoint.X *= scale.X;
			vertexPoint.Y *= scale.Y; //ignore Z

			// Rotate the mesh point:
			vertexPoint = (rotX * vertexPoint.X) + (rotY * vertexPoint.Y); //ignore Z because it is zero

			// Compute distance to camera:
			vertexPoint += meshCenterDistanceFromCamera;

			// Project to camera space:
			const float z = configuration_.InterpolatedCameraAxisZ.GetDotProduct(vertexPoint);
			if (z < minDistanceToCamera_)
			{
				for (int j = i; j > 0; j--) vertexBatch_.pop_back();
				return;
			}
			const float x = configuration_.InterpolatedInversedCameraRotation.GetXAxis().GetDotProduct(vertexPoint);
			const float y = configuration_.InterpolatedInversedCameraRotation.GetYAxis().GetDotProduct(vertexPoint);


			// Project to screen space:
			const Vec2f screenCoords = { configuration_.RenderTargetCenter.X + (x * configuration_.CameraDistanceToScreen) / z,
											configuration_.RenderTargetCenter.Y - (y * configuration_.CameraDistanceToScreen) / z };

			// Add final vertices to batch:
			vertexBatch_.emplace_back(screenCoords, vertices[i].Color, vertices[i].UV);
		}
	}

	void SpriteMeshRenderer3D::Render(const Sprite3DNode& node) 
	{
		const Vertex2D* const vertices = node.Mesh->Vertices;
		const Sprite3DNode* parent = &node;
		const double interpolationAlphaD = static_cast<double>(configuration_.InterpolationAlpha); // Convert to double for later use

		Vec3 vertexPoints[4] = { {vertices[0].Position.X, vertices[0].Position.Y, 0.0},
									 {vertices[1].Position.X, vertices[1].Position.Y, 0.0},
									 {vertices[2].Position.X, vertices[2].Position.Y, 0.0},
									 {vertices[3].Position.X, vertices[3].Position.Y, 0.0} };

		Vec3 prevVertexPoints[4] = { vertexPoints[0], vertexPoints[1], vertexPoints[2], vertexPoints[3] };


		// Transform to world space:
		while (parent != nullptr)
		{
			parent->Transform.TransformPoints(vertexPoints, 4);
			parent->GetPreviousTransform().TransformPoints(prevVertexPoints, 4);
			parent = parent->GetParent();
		}

		//Transform to camera and screen space:
		for (int i = 0; i < 4; i++)
		{
			// Interpolate world position:
			vertexPoints[i] = GetInterpolatedUnchecked(prevVertexPoints[i], vertexPoints[i], interpolationAlphaD);

			// Compute distance-to-camera:
			const Vec3f distanceToCamera = Vec3f(vertexPoints[i] - configuration_.InterpolatedCameraPosition);

			// Project to camera space:
			const float z = configuration_.InterpolatedCameraAxisZ.GetDotProduct(distanceToCamera);
			if (z < minDistanceToCamera_)
			{
				for (int j = i; j > 0; j--)
					vertexBatch_.pop_back();
				return;
			}
			const float x = configuration_.InterpolatedInversedCameraRotation.GetXAxis().GetDotProduct(distanceToCamera);
			const float y = configuration_.InterpolatedInversedCameraRotation.GetYAxis().GetDotProduct(distanceToCamera);

			// Project to screen space:
			const Vec2f screenCoords = { configuration_.RenderTargetCenter.X + (x * configuration_.CameraDistanceToScreen) / z, configuration_.RenderTargetCenter.Y - (y * configuration_.CameraDistanceToScreen) / z };

			// Add final vertices to batch:
			vertexBatch_.emplace_back(screenCoords, vertices[i].Color, vertices[i].UV);
		}
	}

	/*
	void QuadMesh2DRenderer3D::Render(const QuadMesh2D& mesh, const Vector3d& startPoint, const Vector3d& endPoint, float lineWidth) 
	{
		const Vertex2D* const vertices = mesh.Vertices;

		// Compute distance-to-camera (in camera space float should provide sufficient precision):
		Vector3f startPointInCameraSpace = Vector3f(startPoint - configuration_.InterpolatedCameraPosition);
		Vector3f endPointInCameraSpace = Vector3f(endPoint - configuration_.InterpolatedCameraPosition);

		// Project to camera space:
		float z = configuration_.InterpolatedCameraAxisZ.DotProduct(startPointInCameraSpace);
		float x = configuration_.InterpolatedInversedCameraRotation.GetXAxis().DotProduct(startPointInCameraSpace);
		float y = configuration_.InterpolatedInversedCameraRotation.GetYAxis().DotProduct(startPointInCameraSpace);

		// Project to screen space:
		const Vector2f startScreenCoords = { configuration_.RenderTargetCenter.X + (x * configuration_.CameraDistanceToScreen) / z,
											 configuration_.RenderTargetCenter.Y - (y * configuration_.CameraDistanceToScreen) / z };

		z = configuration_.InterpolatedCameraAxisZ.DotProduct(endPointInCameraSpace);
		x = configuration_.InterpolatedInversedCameraRotation.GetXAxis().DotProduct(endPointInCameraSpace);
		y = configuration_.InterpolatedInversedCameraRotation.GetYAxis().DotProduct(endPointInCameraSpace);

		// Project to screen space:
		const Vector2f endScreenCoords = { configuration_.RenderTargetCenter.X + (x * configuration_.CameraDistanceToScreen) / z,
										   configuration_.RenderTargetCenter.Y - (y * configuration_.CameraDistanceToScreen) / z };

		const Vector2f scaledDirectionVector = (endScreenCoords - startScreenCoords).Normalize() * (lineWidth);
		const Vector2f scaledDirectionVectorNormal = scaledDirectionVector.GetNormal();


		Vector2f transformedVertex = startScreenCoords + scaledDirectionVectorNormal;
		vertexBatch_.emplace_back(transformedVertex, vertices[0].Color, vertices[0].TexCoordinates);

		transformedVertex = endScreenCoords + scaledDirectionVectorNormal;
		vertexBatch_.emplace_back(transformedVertex, vertices[1].Color, vertices[1].TexCoordinates);

		transformedVertex = endScreenCoords;
		vertexBatch_.emplace_back(transformedVertex, vertices[2].Color, vertices[2].TexCoordinates);

		transformedVertex = startScreenCoords;
		vertexBatch_.emplace_back(transformedVertex, vertices[3].Color, vertices[3].TexCoordinates);
	}
	*/

	void SpriteMeshRenderer3D::Render(const SpriteMesh& mesh, const Vec3& startPoint, const Vec3& endPoint, float lineWidth) 
	{
		const Vertex2D* const vertices = mesh.Vertices;

		// Compute distance-to-camera (in camera space float should provide sufficient precision):
		Vec3f startPointInCameraSpace = Vec3f(startPoint - configuration_.InterpolatedCameraPosition);
		Vec3f endPointInCameraSpace = Vec3f(endPoint - configuration_.InterpolatedCameraPosition);


		// Project to camera space:
		float x1 = configuration_.InterpolatedInversedCameraRotation.GetXAxis().GetDotProduct(startPointInCameraSpace);
		float y1 = configuration_.InterpolatedInversedCameraRotation.GetYAxis().GetDotProduct(startPointInCameraSpace);
		float z1 = configuration_.InterpolatedCameraAxisZ.GetDotProduct(startPointInCameraSpace);

		float x2 = configuration_.InterpolatedInversedCameraRotation.GetXAxis().GetDotProduct(endPointInCameraSpace);
		float y2 = configuration_.InterpolatedInversedCameraRotation.GetYAxis().GetDotProduct(endPointInCameraSpace);
		float z2 = configuration_.InterpolatedCameraAxisZ.GetDotProduct(endPointInCameraSpace);

		if (z1 < minDistanceToCamera_ && z2 < minDistanceToCamera_)
			return;


		// Project the point behind the camera onto the camera plane
		if (z1 < minDistanceToCamera_ && z2 >= minDistanceToCamera_)
		{
			const float t = (minDistanceToCamera_ - z1) / (z2 - z1);

			x1 = x1 + t * (x2 - x1);
			y1 = y1 + t * (y2 - y1);
			z1 = minDistanceToCamera_;
		}
		else if (z2 < minDistanceToCamera_ && z1 >= minDistanceToCamera_)
		{
			const float t = (minDistanceToCamera_ - z2) / (z1 - z2);

			x2 = x2 + t * (x1 - x2);
			y2 = y2 + t * (y1 - y2);
			z2 = minDistanceToCamera_;
		}


		// Project to screen space:
		const Vec2f startScreenCoords = { configuration_.RenderTargetCenter.X + (x1 * configuration_.CameraDistanceToScreen) / z1,
											 configuration_.RenderTargetCenter.Y - (y1 * configuration_.CameraDistanceToScreen) / z1 };

		// Project to screen space:
		const Vec2f endScreenCoords = { configuration_.RenderTargetCenter.X + (x2 * configuration_.CameraDistanceToScreen) / z2,
										   configuration_.RenderTargetCenter.Y - (y2 * configuration_.CameraDistanceToScreen) / z2 };

		const Vec2f scaledDirectionVector = (startScreenCoords - endScreenCoords).Normalize() * (lineWidth * 0.5f);
		const Vec2f scaledDirectionVectorNormal = scaledDirectionVector.GetNormal();


		Vec2f transformedVertex = startScreenCoords - scaledDirectionVectorNormal;
		vertexBatch_.emplace_back(transformedVertex, vertices[0].Color, vertices[0].UV);

		transformedVertex = endScreenCoords - scaledDirectionVectorNormal;
		vertexBatch_.emplace_back(transformedVertex, vertices[1].Color, vertices[1].UV);

		transformedVertex = endScreenCoords + scaledDirectionVectorNormal;
		vertexBatch_.emplace_back(transformedVertex, vertices[2].Color, vertices[2].UV);

		transformedVertex = startScreenCoords + scaledDirectionVectorNormal;
		vertexBatch_.emplace_back(transformedVertex, vertices[3].Color, vertices[3].UV);
	}





	void SpriteMeshRenderer3D::BeginBatch(const MovableObject3D* camera, float interpolationAlpha, const Vec2f& renderTargetCenter, float verticalFOV) 
	{
		vertexBatch_.clear();

		if (interpolationAlpha > 1.0f)      interpolationAlpha = 1.0f;
		else if (interpolationAlpha < 0.0f) interpolationAlpha = 0.0f;

		configuration_.InterpolationAlpha = interpolationAlpha;
		configuration_.InterpolatedCameraPosition = { 0.0,  0.0,  0.0 };
		configuration_.InterpolatedInversedCameraRotation.SetToIdentity();
		configuration_.InterpolatedCameraAxisZ = { 0.0f, 0.0f, 1.0f };
		configuration_.RenderTargetCenter = renderTargetCenter;

		configuration_.CameraDistanceToScreen = (Renderer::Get().GetLogicalResolutionHeight() * 0.5f) / std::tan(verticalFOV * 0.5f * (float)RADIANS_PER_DEGREE);

		if (camera != nullptr)
		{
			configuration_.InterpolatedCameraPosition = GetInterpolatedUnchecked(camera->GetPreviousTransform().Position, camera->Transform.Position, static_cast<double>(interpolationAlpha));
			configuration_.InterpolatedInversedCameraRotation = GetInterpolated(camera->GetPreviousTransform().Rotation, camera->Transform.Rotation, interpolationAlpha);
			configuration_.InterpolatedCameraAxisZ = configuration_.InterpolatedInversedCameraRotation.GetZAxis();
		}
	}

	void SpriteMeshRenderer3D::RenderBatch(const Texture& boundTexture, TargetTexture* renderTarget) 
	{
		UpdateVertexIndices();

		//SDL_RenderGeometry(SDLRenderer::Get(), configuration_.BoundTexture->Get(), verticesBatch_.data(), verticesBatch_.size(), vertexIndexArray_.data(), (verticesBatch_.size() * 3) / 2);

		const Vertex2D* const vertexArray = vertexBatch_.data();

		Renderer& systemRenderer = Renderer::Get();

		systemRenderer.SetRenderTarget(renderTarget);

		int width, height;
		renderTarget->GetSize(width, height);

		if (renderTarget != nullptr)
			systemRenderer.SetRenderScale(width / systemRenderer.GetLogicalResolutionWidth(), height / systemRenderer.GetLogicalResolutionHeight());

		systemRenderer.RenderGeometryRaw(boundTexture, &(vertexArray->Position.X), 20, &(vertexArray->Color), 20, &(vertexArray->UV.X), 20, vertexBatch_.size(),
			vertexIndices_.data(), (vertexBatch_.size() * 3) / 2, 4);
	}

	void SpriteMeshRenderer3D::UpdateVertexIndices() 
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
