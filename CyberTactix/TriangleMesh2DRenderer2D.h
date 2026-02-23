#pragma once

#include <SDL.h>
#include "PixMath.h"
#include "TargetTexture.h"
#include "Sprite2D.h"
#include "Sprite2DEx.h"
#include "Sprite2DExNode.h"

namespace pix
{
	/// <summary>
	/// Renders complex sprites and meshes to a render target.
	/// </summary>
	class TriangleMesh2DRenderer2D
	{

	public:

		TriangleMesh2DRenderer2D(int initialVertexBatchSize = 100000) ;
		virtual ~TriangleMesh2DRenderer2D()  = default;

		/// <summary>
		/// Renders a complex mesh directly at the given world transrom. This is suitable for rendering an object that can be animated but doesn't move.  
		/// Note that a non-zero rotation may noticeably impact performance with this method.
		void Render(const TriangleMesh2D& mesh, const Transform2D& transform) ;

		void Render(const Sprite2DEx& sprite) ;

		void Render(const Sprite2DExNode& node) ;

		/// <summary>
		/// Begin() updates the configuration for each frame to render. Render()-methods must be called between Begin() and End().
		/// </summary>
		/// <param name="renderTarget"></param>
		/// <param name="interpolation"></param>
		/// <param name="camera"></param>
		/// <param name="renderCenter"></param>
		void BeginBatch(const MovableObject2D* camera = nullptr, float interpolationAlpha = 1.0f, const Vec2f& renderTargetCenter = { 0.0f, 0.0f }) ;

		/// <summary>
		/// End() is specific to the SDL2-renderer: It restores an internal state that Begin() modifies to control render scaling.
		/// The SDL2-renderer is using the render-scale to scale the logical screen area to fit the physical one.
		/// </summary>
		//void EndBatch();

		void RenderBatch(const Texture& boundTexture, TargetTexture* renderTarget) ;


	private:

		struct Configuration
		{
			float    InterpolationAlpha = 1.0f;
			Vec2  InterpolatedCameraPosition = { 0.0,0.0 };
			Vec2f InterpolatedCameraZoom = { 1.0f, 1.0f };
			Rotation2D InterpolatedInversedCameraRotation;
			Vec2f RenderTargetCenter = { 0.0f , 0.0f };
		};




		Configuration configuration_;

		std::vector<Vertex2D> vertexBatch_;

		std::vector<Vec2> pointBuffer1_;
		std::vector<Vec2> pointBuffer2_;
	};

}


