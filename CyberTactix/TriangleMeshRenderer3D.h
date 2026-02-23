#pragma once

#include <SDL.h>
#include "TargetTexture.h"
#include "SpriteMesh.h"
#include "TriangleMesh2D.h"
#include "Sprite3DEx.h"
#include "Sprite3DExNode.h"
#include "Renderer.h"
#include "TriangleMesh3D.h"



namespace pix
{
	/// <summary>
	/// Renders sprites and textures with a camera that has a world-position, zoom, rotation.
	/// </summary>
	class TriangleMeshRenderer3D
	{

	public:

		TriangleMeshRenderer3D(int initialVertexBatchSize = 100000) ;
		virtual ~TriangleMeshRenderer3D()  = default;

		void Render(const TriangleMesh3D& mesh, const Transform3D& transform) ;
		void Render(const TriangleMesh2D& mesh, const Transform3D& transform) ;
		void Render(const Sprite3DEx& sprite) ;
		void Render(const Sprite3DExNode& node) ;

		void BeginBatch(const MovableObject3D* camera, float interpolationAlpha, const Vec2f& renderTargetCenter, float verticalFOV = 60.0f) ;
		void RenderBatch(const Texture& boundTexture, TargetTexture* renderTarget) ;

	private:

		struct Configuration
		{
			float    InterpolationAlpha = 1.0f;
			Vec2f RenderTargetCenter = { 0.0f , 0.0f };

			float      CameraDistanceToScreen = (1080 * 0.5f) / std::tan(60.0f * 0.5f * (float)RADIANS_PER_DEGREE); //935.3f; 
			Vec3   InterpolatedCameraPosition = { 0.0 , 0.0, 0.0 };
			Rotation3D InterpolatedInversedCameraRotation;
			Vec3f   InterpolatedCameraAxisZ = { 0.0f, 0.0f, 1.0f };
		};

		Configuration configuration_;

		std::vector<Vertex2D> vertexBatch_;
		std::vector<Vec3> pointBuffer1_;
		std::vector<Vec3> pointBuffer2_;

		static constexpr float minDistanceToCamera_ = 0.5f;
	};

}

