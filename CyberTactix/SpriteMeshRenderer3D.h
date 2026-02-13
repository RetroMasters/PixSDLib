
#pragma once

#include <SDL.h>
#include "PixMath.h"
#include "TargetTexture.h"
#include "SpriteMesh.h"
#include "Sprite3D.h"
#include "Sprite3DNode.h"
#include "Renderer.h"

namespace pix
{
	/// <summary>
	/// Renders sprites and textures with a camera that has a world-position, zoom, rotation.
	/// </summary>
	class SpriteMeshRenderer3D
	{

	public:

		SpriteMeshRenderer3D(int initialVertexBatchSize = 100000) ;
		virtual ~SpriteMeshRenderer3D()  = default;

		void Render(const SpriteMesh& mesh, const Transform3D& transform) ;

		void Render(const Sprite3D& sprite) ;

		void Render(const Sprite3DNode& node) ;

		void Render(const SpriteMesh& mesh, const Vector3d& startPoint, const Vector3d& endPoint, float lineWidth) ;

		//void BeginBatch(const Texture& boundTexture, TargetTexture* renderTarget = nullptr, float interpolation = 1.0f, const MoveableObject3D* camera = nullptr, const Vector2f& renderCenter = { 0.0f,0.0f }, float verticalFOV = 60.0f);

		void BeginBatch(const MovableObject3D* camera, float interpolationAlpha, const Vector2f& renderTargetCenter, float verticalFOV = 60.0f) ;

		void RenderBatch(const Texture& boundTexture, TargetTexture* renderTarget) ;


	private:

		struct Configuration
		{
			float InterpolationAlpha = 1.0f;
			Vector2f RenderTargetCenter = { 0.0f , 0.0f };

			float      CameraDistanceToScreen = (1080 * 0.5f) / std::tan(60.0f * 0.5f * RADIANS_PER_DEGREE_F); //935.3f; 
			Vector3d   InterpolatedCameraPosition = { 0.0 , 0.0, 0.0 };
			Rotation3D InterpolatedInversedCameraRotation;
			Vector3f   InterpolatedCameraAxisZ = { 0.0f, 0.0f, 1.0f };
		};


		void UpdateVertexIndices() ;

		Configuration configuration_;

		std::vector<Vertex2D> vertexBatch_;
		std::vector<int>      vertexIndices_;

		static constexpr float minDistanceToCamera_ = 0.5f;
	};

}

