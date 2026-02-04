#pragma once

#include <SDL.h>
#include "PixMath.h"
#include "TargetTexture.h"
#include "SpriteMesh.h"
#include "Sprite2D.h"
#include "Sprite2DNode.h"

namespace pix
{
	/// <summary>
	/// Renders sprites and textures with a camera that has a world-position, zoom, rotation.
	/// </summary>
	class SpriteMeshRenderer2D
	{

	public:

		SpriteMeshRenderer2D(int initialVertexBatchSize = 100000) ;
		virtual ~SpriteMeshRenderer2D()  = default;

		/// <summary>
		/// Renders a texture directly at the given world position. This is suitable for rendering a complex(many vertices) texture-based object that can be animated but doesn't move.  
		/// Note that a non-zero rotation may significantly impact performance with this method.
		void Render(const SpriteMesh& mesh, const Transform2D& transform) ;

		void Render(const Sprite2D& sprite) ;

		void Render(const Sprite2DNode& node) ;

		void Render(const SpriteMesh& mesh, const Vector2d& startPoint, const Vector2d& endPoint, float lineWidth) ;

		void RenderPixel(const SpriteMesh& mesh, const Vector2d& point, float pointWidth) ;

		/// <summary>
		/// Begin() updates the configuration for each frame to render. Make render-calls between Begin() and End().
		/// </summary>
		/// <param name="renderTarget"></param>
		/// <param name="interpolation"></param>
		/// <param name="camera"></param>
		/// <param name="renderCenter"></param>
		void BeginBatch(const MoveableObject2D* camera = nullptr, float interpolationAlpha = 1.0f, const Vector2f& renderTargetCenter = { 0.0f, 0.0f }) ;

		/// <summary>
		/// End() is specific to the SDL2-renderer: It restores an internal state that Begin() modifies to control render scaling.
		/// The SDL2-renderer is using the render-scale to scale the logical screen area to fit the physical one.
		/// </summary>
		void RenderBatch(const Texture& boundTexture, TargetTexture* renderTarget) ;




	private:


		struct Configuration
		{
			float    InterpolationAlpha = 1.0f;
			Vector2d InterpolatedCameraPosition = { 0.0,0.0 };
			Vector2f InterpolatedCameraZoom = { 1.0f, 1.0f };
			Rotation2D InterpolatedInversedCameraRotation;
			Vector2f RenderTargetCenter = { 0.0f , 0.0f };

		};

		void UpdateVertexIndices() ;

		Configuration configuration_;

		std::vector<Vertex2D> vertexBatch_;
		std::vector<int>      vertexIndices_;

	};

}


