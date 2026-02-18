#include "TriangleMeshRenderer3D.h"

#include <iostream>


namespace pix
{
	TriangleMeshRenderer3D::TriangleMeshRenderer3D(int initialVertexBatchSize)  :
		configuration_()
	{
		if (initialVertexBatchSize > 0)
			vertexBatch_.reserve(initialVertexBatchSize);
	};


	void TriangleMeshRenderer3D::Render(const TriangleMesh3D& mesh, const Transform3D& transform) 
	{
		const std::vector<Vertex3D>& vertices = mesh.Vertices;
		const int vertexCount = vertices.size();
		const Vector3f meshCenterDistanceFromCamera = Vector3f(transform.Position - configuration_.InterpolatedCameraPosition);
		const Vector3f axisZ = transform.Rotation.GetZAxis(); // Precompute for later use

		//Transform to camera space:
		for (int i = 0; i < vertexCount; i++)
		{
			Vector3f vertexPoint = vertices[i].Position;

			// Scale the mesh point:
			vertexPoint *= transform.Scale;

			// Rotate the mesh point:
			vertexPoint = (transform.Rotation.GetXAxis() * vertexPoint.X) + (transform.Rotation.GetYAxis() * vertexPoint.Y) + (axisZ * vertexPoint.Z);

			// Compute distance to camera:
			vertexPoint += meshCenterDistanceFromCamera;

			// Project to camera space:
			const float z = configuration_.InterpolatedCameraAxisZ.DotProduct(vertexPoint);
			if (z < minDistanceToCamera_)
			{
				int removeCount = i % 3;

				for (int j = removeCount; j > 0; j--)
					vertexBatch_.pop_back();

				i += (2 - removeCount);

				continue;
			}
			const float x = configuration_.InterpolatedInversedCameraRotation.GetXAxis().DotProduct(vertexPoint);
			const float y = configuration_.InterpolatedInversedCameraRotation.GetYAxis().DotProduct(vertexPoint);

			// Project to screen space:
			Vector2f screenCoords = { (x * configuration_.CameraDistanceToScreen) / z, -(y * configuration_.CameraDistanceToScreen) / z };
			screenCoords += configuration_.RenderTargetCenter;

			// Set final vertices:
			if (i % 3 == 2)
			{
				const int end = vertexBatch_.size() - 1;

				const Vector2f firstDirectionNormal = { vertexBatch_[end - 1].Position.Y - vertexBatch_[end].Position.Y,
												  vertexBatch_[end].Position.X - vertexBatch_[end - 1].Position.X };
				const Vector2f secondDirection = { screenCoords.X - vertexBatch_[end].Position.X,
											 screenCoords.Y - vertexBatch_[end].Position.Y };

				if (secondDirection.DotProduct(firstDirectionNormal) > 0.0f) // is front face
					vertexBatch_.emplace_back(screenCoords, vertices[i].Color, vertices[i].TexCoords);
				else
				{
					vertexBatch_.pop_back();
					vertexBatch_.pop_back();
				}
			}
			else
				vertexBatch_.emplace_back(screenCoords, vertices[i].Color, vertices[i].TexCoords);
		}

	}


	void TriangleMeshRenderer3D::Render(const TriangleMesh2D& mesh, const Transform3D& transform) 
	{
		const std::vector<Vertex2DEx>& vertices = mesh.Vertices;
		const int vertexCount = vertices.size();

		const Vector3f meshCenterDistanceFromCamera = Vector3f(transform.Position - configuration_.InterpolatedCameraPosition);

		//Transform to camera space:
		for (int i = 0; i < vertexCount; i++)
		{
			Vector3f vertexPoint = { vertices[i].Position.X, vertices[i].Position.Y, 0.0f };

			// Scale the mesh point:
			vertexPoint.X = vertexPoint.X * transform.Scale.X;
			vertexPoint.Y = vertexPoint.Y * transform.Scale.Y; //ignore Z

			// Rotate the mesh point:
			vertexPoint = (transform.Rotation.GetXAxis() * vertexPoint.X) + (transform.Rotation.GetYAxis() * vertexPoint.Y); //ignore Z

			// Compute distance to camera:
			vertexPoint += meshCenterDistanceFromCamera;

			// Project to camera space:
			const float z = configuration_.InterpolatedCameraAxisZ.DotProduct(vertexPoint);
			if (z < minDistanceToCamera_)
			{
				//int removeCount = i - ((i / 3) * 3);
				int removeCount = i % 3;

				for (int j = removeCount; j > 0; j--)
					vertexBatch_.pop_back();

				i += (2 - removeCount);

				continue;
			}
			const float x = configuration_.InterpolatedInversedCameraRotation.GetXAxis().DotProduct(vertexPoint);
			const float y = configuration_.InterpolatedInversedCameraRotation.GetYAxis().DotProduct(vertexPoint);

			// Project to screen space:
			Vector2f screenCoords = { (x * configuration_.CameraDistanceToScreen) / z, -(y * configuration_.CameraDistanceToScreen) / z };
			screenCoords += configuration_.RenderTargetCenter;

			// Add transformed vertices to batch:
			vertexBatch_.emplace_back(screenCoords, vertices[i].Color, vertices[i].UV);
		}
	}

	void TriangleMeshRenderer3D::Render(const Sprite3DEx& sprite) 
	{
		const std::vector<Vertex2DEx>& vertices = sprite.Mesh->Vertices;
		const int vertexCount = vertices.size();

		Vector3d  pos = sprite.Transform.Position;
		const Vector3d  prevPos = sprite.GetPreviousTransform().Position;
		Vector3f scale = sprite.Transform.Scale;
		const Vector3f prevScale = sprite.GetPreviousTransform().Scale;
		Vector3f rotX = sprite.Transform.Rotation.GetXAxis();
		const Vector3f prevRotX = sprite.GetPreviousTransform().Rotation.GetXAxis();
		Vector3f rotY = sprite.Transform.Rotation.GetYAxis();
		const Vector3f prevRotY = sprite.GetPreviousTransform().Rotation.GetYAxis();

		pos = InterpolateRaw(prevPos, pos, static_cast<double>(configuration_.InterpolationAlpha));
		scale = InterpolateRaw(prevScale, scale, configuration_.InterpolationAlpha);
		rotX = InterpolateRaw(prevRotX, rotX, configuration_.InterpolationAlpha);
		rotY = InterpolateRaw(prevRotY, rotY, configuration_.InterpolationAlpha);

		const Vector3f meshCenterDistanceFromCamera = Vector3f(pos - configuration_.InterpolatedCameraPosition);

		//Transform to camera space:
		for (int i = 0; i < vertexCount; i++)
		{
			Vector3f vertexPoint = { vertices[i].Position.X, vertices[i].Position.Y, 0.0f };

			// Scale the mesh point:
			vertexPoint.X *= scale.X;
			vertexPoint.Y *= scale.Y; //ignore Z

			// Rotate the mesh point:
			vertexPoint = (rotX * vertexPoint.X) + (rotY * vertexPoint.Y); //ignore Z

			// Compute distance to camera:
			vertexPoint += meshCenterDistanceFromCamera;

			// Project to camera space:
			const float z = configuration_.InterpolatedCameraAxisZ.DotProduct(vertexPoint);
			if (z < minDistanceToCamera_)
			{
				int removeCount = i % 3;

				for (int j = removeCount; j > 0; j--)
					vertexBatch_.pop_back();

				i += (2 - removeCount);

				continue;
			}
			const float x = configuration_.InterpolatedInversedCameraRotation.GetXAxis().DotProduct(vertexPoint);
			const float y = configuration_.InterpolatedInversedCameraRotation.GetYAxis().DotProduct(vertexPoint);

			// Project to screen space:
			Vector2f screenCoords = { (x * configuration_.CameraDistanceToScreen) / z, -(y * configuration_.CameraDistanceToScreen) / z };
			screenCoords += configuration_.RenderTargetCenter;

			// Add transformed vertices to batch:
			vertexBatch_.emplace_back(screenCoords, vertices[i].Color, vertices[i].UV);
		}

	}

	void TriangleMeshRenderer3D::Render(const Sprite3DExNode& node) 
	{
		const Sprite3DExNode* parent = &node;
		const std::vector<Vertex2DEx>& vertices = (node.Mesh)->Vertices;
		const int vertexCount = vertices.size();
		const double interpolationAlphaD = configuration_.InterpolationAlpha; // Convert to double for later use

		pointBuffer1_.clear(); // stores the vertex points
		pointBuffer2_.clear(); // stores the previous vertex points

		//for (int i = 0; i < vertexCount; i++)
		//	pointBuffer1_.push_back({ vertices[i].Position.X, vertices[i].Position.Y, 0.0 });

		for (int i = 0; i < vertexCount; i++)
			pointBuffer1_.emplace_back(vertices[i].Position.X, vertices[i].Position.Y, 0.0);

		pointBuffer2_ = pointBuffer1_;

		// Transform to world space:
		while (parent != nullptr)
		{
			parent->Transform.ApplyToPoints(pointBuffer1_.data(), pointBuffer1_.size());
			parent->GetPreviousTransform().ApplyToPoints(pointBuffer2_.data(), pointBuffer2_.size());
			parent = parent->GetParent();
		}


		//Transform to camera and screen space:
		for (int i = 0; i < vertexCount; i++)
		{
			// Interpolate world position:
			pointBuffer1_[i] = InterpolateRaw(pointBuffer2_[i], pointBuffer1_[i], interpolationAlphaD);

			// Compute distance to camera:
			const Vector3f destination = Vector3f(pointBuffer1_[i] - configuration_.InterpolatedCameraPosition);

			// Project to camera space:
			const float z = configuration_.InterpolatedCameraAxisZ.DotProduct(destination);
			if (z < minDistanceToCamera_)
			{
				int removeCount = i % 3;

				for (int j = removeCount; j > 0; j--)
					vertexBatch_.pop_back();

				i += (2 - removeCount);

				continue;
			}
			const float x = configuration_.InterpolatedInversedCameraRotation.GetXAxis().DotProduct(destination);
			const float y = configuration_.InterpolatedInversedCameraRotation.GetYAxis().DotProduct(destination);

			// Project to screen space:
			Vector2f screenCoords = { (x * configuration_.CameraDistanceToScreen) / z, -(y * configuration_.CameraDistanceToScreen) / z };
			screenCoords += configuration_.RenderTargetCenter;

			// Add transformed vertices to batch:
			vertexBatch_.emplace_back(screenCoords, vertices[i].Color, vertices[i].UV);
		}
	}



	void TriangleMeshRenderer3D::BeginBatch(const MovableObject3D* camera, float interpolationAlpha, const Vector2f& renderTargetCenter, float verticalFOV) 
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

			//std::cout << configuration_.InterpolatedCamRotationZ.Z << std::endl;
		}
	}


	void TriangleMeshRenderer3D::RenderBatch(const Texture& boundTexture, TargetTexture* renderTarget) 
	{
		//SDL_RenderGeometry(SDLRenderer::Get(), configuration_.BoundTexture->Get(), verticesBatch_.data(), verticesBatch_.size(), nullptr, 0);

		const Vertex2D* const vertexArray = vertexBatch_.data();

		Renderer& systemRenderer = Renderer::Get();

		systemRenderer.SetRenderTarget(renderTarget);

		int width, height;
		renderTarget->GetSize(width, height);

		if (renderTarget != nullptr)
			systemRenderer.SetRenderScale(width / systemRenderer.GetLogicalResolutionWidth(), height / systemRenderer.GetLogicalResolutionHeight());

		systemRenderer.RenderGeometryRaw(boundTexture, &(vertexArray->Position.X), 20, &(vertexArray->Color), 20, &(vertexArray->UV.X), 20, vertexBatch_.size(), nullptr, 0, 4);

	}

}