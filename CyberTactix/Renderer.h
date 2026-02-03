#pragma once


#include "TargetTexture.h"
#include "PixMath.h"

namespace pix
{
	// The Renderer singleton manages the lifetime of the SDL_Renderer and exposes core functionality for rendering.
	//
	// Philosophy:
	// The purpose of the Renderer singleton is to have core functionality for rendering centralized. 
    // SDL types are used where possible to preserve zero-overhead usage in rendering and
    // direct compatibility with SDL documentation. Engine-specific types (textures) are
    // used where correctness and safe usage must be guaranteed by the API.
	class Renderer : private Uncopyable
	{
	public:

		// Returns the Renderer instance
		static Renderer& Get();

		// Initialization

		// Initializes the Renderer singleton (assumes that the Window singleton is initialized).
		// Clears the default render target to black to ensure a deterministic initial backbuffer.
		// Returns true if initialization succeeds or if the Renderer is already initialized, false otherwise.
		bool Init(int logicalResolutionWidth, int logicalResolutionHeight, bool isIntegerScale, bool isLinearFilter, bool vsync);

		// Destroys the SDL_Renderer.
		// Must be called before destroying the Window to ensure proper cleanup order.
		void Destroy();

		// RENDERING (All render functions return true on success and false on fail.)

		bool Render(const Texture& texture, const SDL_Rect* sourceRect = nullptr, const SDL_FRect* destinationRect = nullptr);

		// NOTE: A non-zero angle may noticeably impact performance
		bool RenderEx(const Texture& texture, const SDL_Rect* sourceRect, const SDL_FRect* destinationRect, double angle, const SDL_FPoint* center, SDL_RendererFlip flipMode = SDL_FLIP_NONE);

		bool RenderGeometryRaw(const Texture& texture, const float* xy, int xy_stride, const SDL_Color* color, int color_stride, const float* uv, int uv_stride, int vertexCount, const void* indices, int indexCount, int size_indices);
		bool RenderGeometry(const Texture& texture, const SDL_Vertex* vertices, int vertexCount, const int* indices, int indexCount);

		void SetAndClearRenderTarget(TargetTexture* renderTarget);

		// STATE

		// Sets the current render target. The default render target is set by nullptr.
        // NOTE: When a non-null render target is set, SDL resets the render scale to (1,1).
		void SetRenderTarget(TargetTexture* renderTarget = nullptr);

		void SetRenderColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a);

		void SetRenderScale(float scaleX, float scaleY);

		// Returns true if the hint was set, false otherwise
		bool SetLinearFilterHint(bool isLinearFilter);

		void SetIntegerScale(bool isIntegerScale);

		void SwapBuffers();

		// GETTERS

		void GetRenderColor(Uint8& r, Uint8& g, Uint8& b, Uint8& a) const;

		float GetLogicalResolutionWidth() const;

		float GetLogicalResolutionHeight() const;

		bool IsLinearFilterHint() const;

		bool IsIntegerScale() const;

		bool IsVsync() const;

		Vector2f GetRenderScale() const;

		SDL_Renderer* GetSDLRenderer() const;

		bool IsInitialized() const;

	private:

		Renderer();
		~Renderer();

		SDL_Renderer* sdlRenderer_;
		float logicalResolutionWidth_;
		float logicalResolutionHeight_;
		bool isVsync_;
		bool isInitialized_;
	};
}
