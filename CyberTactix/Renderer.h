#pragma once

#include "Uncopyable.h"
#include <SDL_render.h>
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
	// 
	// Rendering is a critical subsystem; after Init() failure, normal engine execution should not continue.
    // Rendering methods therefore assume successful initialization and do not perform repeated initialization checks.
	class Renderer : private Uncopyable
	{
	public:

		// Returns the Renderer instance
		static Renderer& Get();

		// ####################################################### INITIALIZATION #######################################################################

		// Initializes the Renderer singleton. Requires the Window singleton to be initialized.
        // Clears the default render target to black to ensure a deterministic initial backbuffer.
        // Returns true if the SDL_Renderer was created, or if the Renderer is already initialized.
		// Calling Init() again after successful initialization has no effect and returns true.
        // Renderer setup failures after creation are logged but do not make Init() fail.
		// Renderer is not meant to be reinitialized during normal program execution.
		bool Init(int logicalResolutionWidth, int logicalResolutionHeight, bool isIntegerScale, bool isLinearFilter, bool vsync);

		// Destroys the SDL_Renderer.
        // Destroy Texture subclasses before this, since they wrap SDL_Texture resources associated with this SDL_Renderer.
        // Must be called before destroying the Window to ensure proper cleanup order.
		void Destroy();

		// ###################################### RENDERING (All render methods return true on success and false on failure) ##############################

		bool Render(const Texture& texture, const SDL_Rect* sourceRect = nullptr, const SDL_FRect* destinationRect = nullptr);

		// NOTE: A non-zero angle may noticeably impact performance
		bool RenderEx(const Texture& texture, const SDL_Rect* sourceRect, const SDL_FRect* destinationRect, double angle, const SDL_FPoint* center, SDL_RendererFlip flipMode = SDL_FLIP_NONE);

		bool RenderGeometryRaw(const Texture& texture, const float* xy, int xyStride, const SDL_Color* color, int colorStride, const float* uv, int uvStride, int vertexCount, const void* indices, int indexCount, int indexSize);

		bool RenderGeometry(const Texture& texture, const SDL_Vertex* vertices, int vertexCount, const int* indices, int indexCount);

		// Clears the current render target using the current render color.
        // The current render target may be the default backbuffer or a TargetTexture.
		bool Clear();

		// ##################################### SETTING STATE (Boolean methods return true on success and false on failure) ######################################################################

		// Sets the current render target. Pass nullptr to set the default backbuffer.
        // Render target binding is global renderer state and remains active until changed.
        // NOTE: When a non-null render target is set, SDL resets the render scale to (1,1).
		bool SetRenderTarget(TargetTexture* renderTarget = nullptr);

		bool SetRenderColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a);

		bool SetRenderScale(float scaleX, float scaleY);

		// Sets the renderer's texture filtering preference (implemented internally as an SDL hint).
        // Returns true if the hint was successfully set, false otherwise.
		bool SetLinearFilter(bool isLinearFilter);

		bool SetIntegerScale(bool isIntegerScale);

		void SwapBuffers();

		// ############################################################ GETTERS #######################################################################

		void GetRenderColor(Uint8& r, Uint8& g, Uint8& b, Uint8& a) const;

		float GetLogicalResolutionWidth() const;

		float GetLogicalResolutionHeight() const;

		bool IsLinearFilter() const;

		bool IsIntegerScale() const;

	    // Vsync can only be queried as it is only applied when the Renderer is created
		bool IsVsync() const;

		Vec2f GetRenderScale() const;

		SDL_Renderer* GetSDLRenderer() const;

		bool IsInitialized() const;

	private:

		Renderer() = default;

		// Final safety cleanup.
        // Prefer explicit Destroy() calls during normal shutdown to control destruction order
        // (Texture subclasses before Renderer, Renderer before Window).
		~Renderer();

		SDL_Renderer* sdlRenderer_ = nullptr;
		float logicalResolutionWidth_ = 0.0f;
		float logicalResolutionHeight_ = 0.0f;
		bool isVsync_ = true;
		bool isInitialized_ = false;
	};
}
