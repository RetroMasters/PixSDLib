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

		const Vector3f meshCenterDistanceFromCamera = Vector3f(transform.Position - configuration_.InterpolatedCameraPosition);

		//Transform to camera space:
		for (int i = 0; i < 4; i++)
		{
			Vector3f vertexPoint = { vertices[i].Position.X, vertices[i].Position.Y, 0.0f };

			// Scale the mesh point:
			vertexPoint.X *= transform.Scale.X;
			vertexPoint.Y *= transform.Scale.Y; //ignore Z

			// Rotate the mesh point:
			vertexPoint = (transform.Rotation.GetXAxis() * vertexPoint.X) + (transform.Rotation.GetYAxis() * vertexPoint.Y); //ignore Z

			// Compute distance to camera:
			vertexPoint += meshCenterDistanceFromCamera;

			// Project to camera space:
			const float z = configuration_.InterpolatedCameraAxisZ.DotProduct(vertexPoint);
			if (z < minDistanceToCamera_)
			{
				for (int j = i; j > 0; j--) vertexBatch_.pop_back();
				return;
			}
			const float x = configuration_.InterpolatedInversedCameraRotation.GetXAxis().DotProduct(vertexPoint);
			const float y = configuration_.InterpolatedInversedCameraRotation.GetYAxis().DotProduct(vertexPoint);


			// Project to screen space:
			const Vector2f screenCoords = { configuration_.RenderTargetCenter.X + (x * configuration_.CameraDistanceToScreen) / z,
											configuration_.RenderTargetCenter.Y - (y * configuration_.CameraDistanceToScreen) / z };

			// Add final vertices to batch:
			vertexBatch_.emplace_back(screenCoords, vertices[i].Color, vertices[i].UV);
		}
	}



	void SpriteMeshRenderer3D::Render(const Sprite3D& sprite) 
	{
		const Vertex2D* const vertices = sprite.Mesh->Vertices;

		Vector3d pos = sprite.Transform.Position;
		const Vector3d& prevPos = sprite.GetPreviousTransform().Position;
		Vector3f scale = sprite.Transform.Scale;
		const Vector3f& prevScale = sprite.GetPreviousTransform().Scale;
		Vector3f rotX = sprite.Transform.Rotation.GetXAxis();
		const Vector3f& prevRotX = sprite.GetPreviousTransform().Rotation.GetXAxis();
		Vector3f rotY = sprite.Transform.Rotation.GetYAxis();
		const Vector3f& prevRotY = sprite.GetPreviousTransform().Rotation.GetYAxis();

		pos = InterpolateRaw(prevPos, pos, static_cast<double>(configuration_.InterpolationAlpha));
		scale = InterpolateRaw(prevScale, scale, configuration_.InterpolationAlpha);
		rotX = InterpolateRaw(prevRotX, rotX, configuration_.InterpolationAlpha);
		rotY = InterpolateRaw(prevRotY, rotY, configuration_.InterpolationAlpha);

		const Vector3f meshCenterDistanceFromCamera = Vector3f(pos - configuration_.InterpolatedCameraPosition);

		//Transform to camera space:
		for (int i = 0; i < 4; i++)
		{
			Vector3f vertexPoint = { vertices[i].Position.X, vertices[i].Position.Y, 0.0f };

			// Scale the mesh point:
			vertexPoint.X *= scale.X;
			vertexPoint.Y *= scale.Y; //ignore Z

			// Rotate the mesh point:
			vertexPoint = (rotX * vertexPoint.X) + (rotY * vertexPoint.Y); //ignore Z because it is zero

			// Compute distance to camera:
			vertexPoint += meshCenterDistanceFromCamera;

			// Project to camera space:
			const float z = configuration_.InterpolatedCameraAxisZ.DotProduct(vertexPoint);
			if (z < minDistanceToCamera_)
			{
				for (int j = i; j > 0; j--) vertexBatch_.pop_back();
				return;
			}
			const float x = configuration_.InterpolatedInversedCameraRotation.GetXAxis().DotProduct(vertexPoint);
			const float y = configuration_.InterpolatedInversedCameraRotation.GetYAxis().DotProduct(vertexPoint);


			// Project to screen space:
			const Vector2f screenCoords = { configuration_.RenderTargetCenter.X + (x * configuration_.CameraDistanceToScreen) / z,
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

		Vector3d vertexPoints[4] = { {vertices[0].Position.X, vertices[0].Position.Y, 0.0},
									 {vertices[1].Position.X, vertices[1].Position.Y, 0.0},
									 {vertices[2].Position.X, vertices[2].Position.Y, 0.0},
									 {vertices[3].Position.X, vertices[3].Position.Y, 0.0} };

		Vector3d prevVertexPoints[4] = { vertexPoints[0], vertexPoints[1], vertexPoints[2], vertexPoints[3] };


		// Transform to world space:
		while (parent != nullptr)
		{
			parent->Transform.ApplyToPoints(vertexPoints, 4);
			parent->GetPreviousTransform().ApplyToPoints(prevVertexPoints, 4);
			parent = parent->GetParent();
		}

		//Transform to camera and screen space:
		for (int i = 0; i < 4; i++)
		{
			// Interpolate world position:
			vertexPoints[i] = InterpolateRaw(prevVertexPoints[i], vertexPoints[i], interpolationAlphaD);

			// Compute distance-to-camera:
			const Vector3f distanceToCamera = Vector3f(vertexPoints[i] - configuration_.InterpolatedCameraPosition);

			// Project to camera space:
			const float z = configuration_.InterpolatedCameraAxisZ.DotProduct(distanceToCamera);
			if (z < minDistanceToCamera_)
			{
				for (int j = i; j > 0; j--)
					vertexBatch_.pop_back();
				return;
			}
			const float x = configuration_.InterpolatedInversedCameraRotation.GetXAxis().DotProduct(distanceToCamera);
			const float y = configuration_.InterpolatedInversedCameraRotation.GetYAxis().DotProduct(distanceToCamera);

			// Project to screen space:
			const Vector2f screenCoords = { configuration_.RenderTargetCenter.X + (x * configuration_.CameraDistanceToScreen) / z, configuration_.RenderTargetCenter.Y - (y * configuration_.CameraDistanceToScreen) / z };

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

	void SpriteMeshRenderer3D::Render(const SpriteMesh& mesh, const Vector3d& startPoint, const Vector3d& endPoint, float lineWidth) 
	{
		const Vertex2D* const vertices = mesh.Vertices;

		// Compute distance-to-camera (in camera space float should provide sufficient precision):
		Vector3f startPointInCameraSpace = Vector3f(startPoint - configuration_.InterpolatedCameraPosition);
		Vector3f endPointInCameraSpace = Vector3f(endPoint - configuration_.InterpolatedCameraPosition);


		// Project to camera space:
		float x1 = configuration_.InterpolatedInversedCameraRotation.GetXAxis().DotProduct(startPointInCameraSpace);
		float y1 = configuration_.InterpolatedInversedCameraRotation.GetYAxis().DotProduct(startPointInCameraSpace);
		float z1 = configuration_.InterpolatedCameraAxisZ.DotProduct(startPointInCameraSpace);

		float x2 = configuration_.InterpolatedInversedCameraRotation.GetXAxis().DotProduct(endPointInCameraSpace);
		float y2 = configuration_.InterpolatedInversedCameraRotation.GetYAxis().DotProduct(endPointInCameraSpace);
		float z2 = configuration_.InterpolatedCameraAxisZ.DotProduct(endPointInCameraSpace);

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
		const Vector2f startScreenCoords = { configuration_.RenderTargetCenter.X + (x1 * configuration_.CameraDistanceToScreen) / z1,
											 configuration_.RenderTargetCenter.Y - (y1 * configuration_.CameraDistanceToScreen) / z1 };

		// Project to screen space:
		const Vector2f endScreenCoords = { configuration_.RenderTargetCenter.X + (x2 * configuration_.CameraDistanceToScreen) / z2,
										   configuration_.RenderTargetCenter.Y - (y2 * configuration_.CameraDistanceToScreen) / z2 };

		const Vector2f scaledDirectionVector = (startScreenCoords - endScreenCoords).Normalize() * (lineWidth * 0.5f);
		const Vector2f scaledDirectionVectorNormal = scaledDirectionVector.GetNormal();


		Vector2f transformedVertex = startScreenCoords - scaledDirectionVectorNormal;
		vertexBatch_.emplace_back(transformedVertex, vertices[0].Color, vertices[0].UV);

		transformedVertex = endScreenCoords - scaledDirectionVectorNormal;
		vertexBatch_.emplace_back(transformedVertex, vertices[1].Color, vertices[1].UV);

		transformedVertex = endScreenCoords + scaledDirectionVectorNormal;
		vertexBatch_.emplace_back(transformedVertex, vertices[2].Color, vertices[2].UV);

		transformedVertex = startScreenCoords + scaledDirectionVectorNormal;
		vertexBatch_.emplace_back(transformedVertex, vertices[3].Color, vertices[3].UV);
	}





	void SpriteMeshRenderer3D::BeginBatch(const MoveableObject3D* camera, float interpolationAlpha, const Vector2f& renderTargetCenter, float verticalFOV) 
	{
		vertexBatch_.clear();

		if (interpolationAlpha > 1.0f)      interpolationAlpha = 1.0f;
		else if (interpolationAlpha < 0.0f) interpolationAlpha = 0.0f;

		configuration_.InterpolationAlpha = interpolationAlpha;
		configuration_.InterpolatedCameraPosition = { 0.0,  0.0,  0.0 };
		configuration_.InterpolatedInversedCameraRotation.SetToIdentity();
		configuration_.InterpolatedCameraAxisZ = { 0.0f, 0.0f, 1.0f };
		configuration_.RenderTargetCenter = renderTargetCenter;

		configuration_.CameraDistanceToScreen = (Renderer::Get().GetLogicalResolutionHeight() * 0.5f) / std::tan(verticalFOV * 0.5f * RADIANS_PER_DEGREE_F);

		if (camera != nullptr)
		{
			configuration_.InterpolatedCameraPosition = InterpolateRaw(camera->GetPreviousTransform().Position, camera->Transform.Position, static_cast<double>(interpolationAlpha));
			configuration_.InterpolatedInversedCameraRotation = Interpolate(camera->GetPreviousTransform().Rotation, camera->Transform.Rotation, interpolationAlpha);
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
