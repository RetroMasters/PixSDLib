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

		void BeginBatch(const MoveableObject3D* camera, float interpolationAlpha, const Vector2f& renderTargetCenter, float verticalFOV = 60.0f) ;
		void RenderBatch(const Texture& boundTexture, TargetTexture* renderTarget) ;

	private:

		struct Configuration
		{
			float    InterpolationAlpha = 1.0f;
			Vector2f RenderTargetCenter = { 0.0f , 0.0f };

			float      CameraDistanceToScreen = (1080 * 0.5f) / std::tan(60.0f * 0.5f * RADIANS_PER_DEGREE_F); //935.3f; 
			Vector3d   InterpolatedCameraPosition = { 0.0 , 0.0, 0.0 };
			Rotation3D InterpolatedInversedCameraRotation;
			Vector3f   InterpolatedCameraAxisZ = { 0.0f, 0.0f, 1.0f };
		};

		Configuration configuration_;

		std::vector<Vertex2D> vertexBatch_;
		std::vector<Vector3d> pointBuffer1_;
		std::vector<Vector3d> pointBuffer2_;

		static constexpr float minDistanceToCamera_ = 0.5f;
	};

}

