#include "Renderer.h"
#include "Window.h"
#include "ErrorLogger.h"

namespace pix
{

	Renderer& Renderer::Get()
	{
		static Renderer renderer_;
		return renderer_;
	}




	bool Renderer::Init(int logicalResolutionWidth, int logicalResolutionHeight, bool isIntegerScale, bool isLinearFilter, bool vsync)
	{
		if (isInitialized_) return true;

		if (logicalResolutionWidth < 1) logicalResolutionWidth = 1;
		if (logicalResolutionHeight < 1) logicalResolutionHeight = 1;

		isVsync_ = vsync;

		Uint32 flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE;
		if (vsync)
			flags |= SDL_RENDERER_PRESENTVSYNC;

		sdlRenderer_ = SDL_CreateRenderer(Window::Get().GetSDLWindow(), -1, flags); 

		if (sdlRenderer_ == nullptr)
		{
			ErrorLogger::Get().LogSDLError("Renderer::Init() - SDL_CreateRenderer() fail");
			return false;
		}

		if (SDL_RenderSetLogicalSize(sdlRenderer_, logicalResolutionWidth, logicalResolutionHeight) != 0)
			ErrorLogger::Get().LogSDLError("Renderer::Init() - SDL_RenderSetLogicalSize() fail");

		int w, h;
		SDL_RenderGetLogicalSize(sdlRenderer_, &w, &h);
		logicalResolutionWidth_ = w;
		logicalResolutionHeight_ = h;

		if (logicalResolutionWidth_ != logicalResolutionWidth || logicalResolutionHeight_ != logicalResolutionHeight)
			ErrorLogger::Get().LogError("Renderer::Init() - SDL_RenderGetLogicalSize() fail", "Returned " + std::to_string(logicalResolutionWidth_) + " x " + std::to_string(logicalResolutionHeight_)
				+ " instead of " + std::to_string(logicalResolutionWidth) + "x" + std::to_string(logicalResolutionHeight) + "!");


		if (!SetLinearFilterHint(isLinearFilter))
		{
			if (isLinearFilter)
				ErrorLogger::Get().LogError("Renderer::Init() - SetLinearScaleQuality() fail", "Failed to set scale quality to linear!");
			else
				ErrorLogger::Get().LogError("Renderer::Init() - SetLinearScaleQuality() fail", "Failed to set scale quality to nearest!");
		}

		SetIntegerScale(isIntegerScale);


		SetRenderColor(0, 0, 0, 255);
		SetAndClearRenderTarget(nullptr);

		isInitialized_ = true;

		return true;
	}

	void Renderer::Destroy()
	{
		if (!sdlRenderer_) return;
	
		SDL_DestroyRenderer(sdlRenderer_);

		sdlRenderer_ = nullptr;
		logicalResolutionWidth_ = 0.0f; 
		logicalResolutionHeight_ = 0.0f;
		isVsync_ = false;
		isInitialized_ = false;
	}



	bool Renderer::Render(const Texture& texture, const SDL_Rect* sourceRect, const SDL_FRect* destinationRect) 
	{
		return SDL_RenderCopyF(sdlRenderer_, texture.GetSDLTexture(), sourceRect, destinationRect) == 0;
	}

	bool Renderer::RenderEx(const Texture& texture, const SDL_Rect* sourceRect, const SDL_FRect* destinationRect, double angle, const SDL_FPoint* center, SDL_RendererFlip flipMode) 
	{
		return SDL_RenderCopyExF(sdlRenderer_, texture.GetSDLTexture(), sourceRect, destinationRect, angle, center, flipMode) == 0;
	}

	bool Renderer::RenderGeometryRaw(const Texture& texture, const float* xy, int xyStride, const SDL_Color* color, int colorStride, const float* uv, int uvStride, int vertexCount, const void* indices, int indexCount, int indexSize) 
	{
		return SDL_RenderGeometryRaw(sdlRenderer_, texture.GetSDLTexture(), xy, xyStride, color, colorStride, uv, uvStride, vertexCount, indices, indexCount, indexSize) == 0;
	}

	bool Renderer::RenderGeometry(const Texture& texture, const SDL_Vertex* vertices, int vertexCount, const int* indices, int indexCount) 
	{
		return SDL_RenderGeometry(sdlRenderer_, texture.GetSDLTexture(), vertices, vertexCount, indices, indexCount) == 0;
	}

	void Renderer::SetAndClearRenderTarget(TargetTexture* renderTarget)
	{
		SetRenderTarget(renderTarget);

		if (SDL_RenderClear(sdlRenderer_) != 0)
			ErrorLogger::Get().LogSDLError("SetAndClearRenderTarget() - SDL_RenderClear() fail");
	}



	void Renderer::SetRenderTarget(TargetTexture* renderTarget)
	{
		if (renderTarget == nullptr)
		{
			if (SDL_SetRenderTarget(sdlRenderer_, nullptr) != 0)
				ErrorLogger::Get().LogSDLError("SetRenderTarget() - SDL_SetRenderTarget() fail");
		}
		else
		{
			if (SDL_SetRenderTarget(sdlRenderer_, renderTarget->GetSDLTexture()) != 0)
				ErrorLogger::Get().LogSDLError("SetRenderTarget() - SDL_SetRenderTarget() fail");
		}
	}

	void Renderer::SetRenderColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a)
	{
		if (SDL_SetRenderDrawColor(sdlRenderer_, r, g, b, a) != 0)
			ErrorLogger::Get().LogSDLError("SetRenderColor() - SDL_SetRenderDrawColor() fail");
	}

	void Renderer::SetRenderScale(float scaleX, float scaleY)
	{
		if (SDL_RenderSetScale(sdlRenderer_, scaleX, scaleY) != 0)
			ErrorLogger::Get().LogSDLError("SetRenderScale() - SDL_RenderSetScale() fail");
	}

	bool Renderer::SetLinearFilterHint(bool isLinearFilter)
	{
		return SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, isLinearFilter ? "1" : "0") == SDL_TRUE;
	}

	void Renderer::SetIntegerScale(bool isIntegerScale)
	{
		
		if (SDL_RenderSetIntegerScale(sdlRenderer_, isIntegerScale ? SDL_TRUE : SDL_FALSE) != 0)
			ErrorLogger::Get().LogSDLError("SetIntegerScale() - SDL_RenderSetIntegerScale() fail");
	}

	void Renderer::SwapBuffers()
	{
		SDL_RenderPresent(sdlRenderer_);
	}
	


	void Renderer::GetRenderColor(Uint8& r, Uint8& g, Uint8& b, Uint8& a) const
	{
		if (SDL_GetRenderDrawColor(sdlRenderer_, &r, &g, &b, &a) != 0)
			ErrorLogger::Get().LogSDLError("GetRenderColor() - SDL_GetRenderDrawColor() fail");

	}

	float Renderer::GetLogicalResolutionWidth() const
	{
		return logicalResolutionWidth_;
	}

	float Renderer::GetLogicalResolutionHeight() const
	{
		return logicalResolutionHeight_;
	}

	bool Renderer::IsLinearFilterHint() const
	{
		const char* hint = SDL_GetHint(SDL_HINT_RENDER_SCALE_QUALITY);
	
		return hint && std::strcmp(hint, "0") != 0 && std::strcmp(hint, "nearest") != 0;
	}


	bool Renderer::IsIntegerScale() const
	{
		return SDL_RenderGetIntegerScale(sdlRenderer_) == SDL_TRUE;
	}

	bool Renderer::IsVsync() const
	{
		return isVsync_;
	}

	Vector2f Renderer::GetRenderScale() const
	{
		Vector2f scale;

		SDL_RenderGetScale(sdlRenderer_, &scale.X, &scale.Y);

		return scale;
	}

	SDL_Renderer* Renderer::GetSDLRenderer() const
	{
		return sdlRenderer_;
	}

	bool Renderer::IsInitialized() const 
	{
		return isInitialized_;
	}



	Renderer::Renderer() :
		sdlRenderer_(nullptr),
		logicalResolutionWidth_(0.0f),
		logicalResolutionHeight_(0.0f),
		isVsync_(false),
		isInitialized_(false)
	{
	}

	Renderer::~Renderer()
	{
		Destroy();
	}

}
