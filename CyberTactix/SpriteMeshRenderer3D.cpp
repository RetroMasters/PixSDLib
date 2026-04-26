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
			const float z = configuration_.InterpolatedCameraZAxis.GetDotProduct(vertexPoint);
			if (z > -NEAR_CLIP_DISTANCE)
			{
				for (int j = i; j > 0; j--) vertexBatch_.pop_back();
				return;
			}
			const float x = configuration_.InterpolatedCameraRotation.GetXAxis().GetDotProduct(vertexPoint);
			const float y = configuration_.InterpolatedCameraRotation.GetYAxis().GetDotProduct(vertexPoint);


			// Project to screen space:
			const Vec2f screenCoords = { configuration_.RenderTargetOffset.X + (x * configuration_.CameraDistanceToScreen) / z,
											configuration_.RenderTargetOffset.Y - (y * configuration_.CameraDistanceToScreen) / z };

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
			const float z = configuration_.InterpolatedCameraZAxis.GetDotProduct(vertexPoint);
			if (z > -NEAR_CLIP_DISTANCE)
			{
				for (int j = i; j > 0; j--) vertexBatch_.pop_back();
				return;
			}
			const float x = configuration_.InterpolatedCameraRotation.GetXAxis().GetDotProduct(vertexPoint);
			const float y = configuration_.InterpolatedCameraRotation.GetYAxis().GetDotProduct(vertexPoint);


			// Project to screen space:
			const Vec2f screenCoords = { configuration_.RenderTargetOffset.X + (x * configuration_.CameraDistanceToScreen) / z,
											configuration_.RenderTargetOffset.Y - (y * configuration_.CameraDistanceToScreen) / z };

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
			const float z = configuration_.InterpolatedCameraZAxis.GetDotProduct(distanceToCamera);
			if (z > -NEAR_CLIP_DISTANCE)
			{
				for (int j = i; j > 0; j--)
					vertexBatch_.pop_back();
				return;
			}
			const float x = configuration_.InterpolatedCameraRotation.GetXAxis().GetDotProduct(distanceToCamera);
			const float y = configuration_.InterpolatedCameraRotation.GetYAxis().GetDotProduct(distanceToCamera);

			// Project to screen space:
			const Vec2f screenCoords = { configuration_.RenderTargetOffset.X + (x * configuration_.CameraDistanceToScreen) / z, configuration_.RenderTargetOffset.Y - (y * configuration_.CameraDistanceToScreen) / z };

			// Add final vertices to batch:
			vertexBatch_.emplace_back(screenCoords, vertices[i].Color, vertices[i].UV);
		}
	}


	void SpriteMeshRenderer3D::RenderLine(const SpriteMesh& mesh, const Vec3& startPoint, const Vec3& endPoint, float lineWidth) 
	{
		const Vertex2D* const vertices = mesh.Vertices;

		// World-space vector from camera to startPoint/endPoint (camera-space float precision is sufficient)
		Vec3f cameraToStartPoint = Vec3f(startPoint - configuration_.InterpolatedCameraPosition);
		Vec3f cameraToEndPoint = Vec3f(endPoint - configuration_.InterpolatedCameraPosition);

		// Transform the line points to camera space
		float x1 = configuration_.InterpolatedCameraRotation.GetXAxis().GetDotProduct(cameraToStartPoint);
		float y1 = configuration_.InterpolatedCameraRotation.GetYAxis().GetDotProduct(cameraToStartPoint);
		float z1 = configuration_.InterpolatedCameraZAxis.GetDotProduct(cameraToStartPoint);

		float x2 = configuration_.InterpolatedCameraRotation.GetXAxis().GetDotProduct(cameraToEndPoint);
		float y2 = configuration_.InterpolatedCameraRotation.GetYAxis().GetDotProduct(cameraToEndPoint);
		float z2 = configuration_.InterpolatedCameraZAxis.GetDotProduct(cameraToEndPoint);

		if (z1 > -NEAR_CLIP_DISTANCE && z2 > -NEAR_CLIP_DISTANCE)
			return;

		// Clip the point behind the near clip plane to the near clip plane
		if (z1 <= -NEAR_CLIP_DISTANCE && z2 > -NEAR_CLIP_DISTANCE)
		{
			const float t = (-NEAR_CLIP_DISTANCE - z1) / (z2 - z1);

			x2 = x1 + t * (x2 - x1);
			y2 = y1 + t * (y2 - y1);
			z2 = -NEAR_CLIP_DISTANCE;
		}
		else if (z2 <= -NEAR_CLIP_DISTANCE && z1 > -NEAR_CLIP_DISTANCE)
		{
			const float t = (-NEAR_CLIP_DISTANCE - z2) / (z1 - z2);

			x1 = x2 + t * (x1 - x2);
			y1 = y2 + t * (y1 - y2);
			z1 = -NEAR_CLIP_DISTANCE;
		}

		// Project the line points to logical render-target coordinates (Y increases downward)
		Vec2f startPointRenderTargetCoords = Vec2f(x1 * configuration_.CameraDistanceToScreen / (-z1), y1 * configuration_.CameraDistanceToScreen / z1);
		startPointRenderTargetCoords += configuration_.RenderTargetOffset;

		Vec2f endPointRenderTargetCoords = Vec2f(x2 * configuration_.CameraDistanceToScreen / (-z2), y2 * configuration_.CameraDistanceToScreen / z2);
		endPointRenderTargetCoords += configuration_.RenderTargetOffset;

		const Vec2f halfWidthNormal = ((startPointRenderTargetCoords - endPointRenderTargetCoords).Normalize() * (lineWidth * 0.5f)).GetNormal();

		// Add quad points in clockwise order on the render target around the centered line segment
		Vec2f quadPoint = startPointRenderTargetCoords + halfWidthNormal;
		vertexBatch_.emplace_back(quadPoint, vertices[0].Color, vertices[0].UV);

		quadPoint = endPointRenderTargetCoords + halfWidthNormal;
		vertexBatch_.emplace_back(quadPoint, vertices[1].Color, vertices[1].UV);

		quadPoint = endPointRenderTargetCoords - halfWidthNormal;
		vertexBatch_.emplace_back(quadPoint, vertices[2].Color, vertices[2].UV);

		quadPoint = startPointRenderTargetCoords - halfWidthNormal;
		vertexBatch_.emplace_back(quadPoint, vertices[3].Color, vertices[3].UV);
	}

	void SpriteMeshRenderer3D::RenderPoint(const SpriteMesh& mesh, const Vec3& point, float pointWidth)
	{
		const Vertex2D* const vertices = mesh.Vertices;

		// World-space vector from camera to point (camera-space float precision is sufficient)
		Vec3f cameraToPoint = Vec3f(point - configuration_.InterpolatedCameraPosition);

		// Transform the point to camera space
		float x = configuration_.InterpolatedCameraRotation.GetXAxis().GetDotProduct(cameraToPoint);
		float y = configuration_.InterpolatedCameraRotation.GetYAxis().GetDotProduct(cameraToPoint);
		float z = configuration_.InterpolatedCameraZAxis.GetDotProduct(cameraToPoint);

		if (z > -NEAR_CLIP_DISTANCE) return; // Discard points behind the near clip plane

		// Project the point to logical render-target coordinates (Y increases downward)
		Vec2f renderTargetCoords = Vec2f(x * configuration_.CameraDistanceToScreen / (-z), y * configuration_.CameraDistanceToScreen / z);
		renderTargetCoords += configuration_.RenderTargetOffset;

		const float halfWidth = pointWidth * 0.5f;

		// Add quad points in clockwise order on the render target around the centered point
		Vec2f quadPoint = renderTargetCoords + Vec2f(-halfWidth, -halfWidth);
		vertexBatch_.emplace_back(quadPoint, vertices[0].Color, vertices[0].UV);

		quadPoint = renderTargetCoords + Vec2f(halfWidth, -halfWidth);
		vertexBatch_.emplace_back(quadPoint, vertices[1].Color, vertices[1].UV);

		quadPoint = renderTargetCoords + Vec2f(halfWidth, halfWidth);
		vertexBatch_.emplace_back(quadPoint, vertices[2].Color, vertices[2].UV);

		quadPoint = renderTargetCoords + Vec2f(-halfWidth, halfWidth);
		vertexBatch_.emplace_back(quadPoint, vertices[3].Color, vertices[3].UV);
	}


	void SpriteMeshRenderer3D::BeginBatch(const MovableObject3D& camera, float interpolationAlpha, Vec2f renderTargetOffset, float verticalFOV)
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

	void SpriteMeshRenderer3D::RenderBatch(const Texture& texture, TargetTexture* renderTarget)
	{
		if (vertexBatch_.empty()) return;

		UpdateVertexIndices();

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

		renderer.RenderGeometryRaw(texture, &(vertexArray->Position.X), stride, &(vertexArray->Color), stride, &(vertexArray->UV.X), stride, vertexBatch_.size(),
			vertexIndices_.data(), (vertexBatch_.size() * 3) / 2, 4);

		renderer.SetRenderScale(cachedRenderScale.X, cachedRenderScale.Y); // Restore cached render scale
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
