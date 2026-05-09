#include "SoundEffect.h"
#include "Audio.h"
#include "ErrorLogger.h"
#include "PixMath.h"

namespace pix
{

	SoundEffect::SoundEffect(const std::string& filePath, float chunkVolume, int repeatCount) 
	{
		SetVolume(chunkVolume);

		SetRepeatCount(repeatCount);

		Reload(filePath);
	}

	SoundEffect::~SoundEffect()
	{
		if(soundChunk_) 
		  Audio::Get().DestroySoundChunk(soundChunk_);
	}

	bool SoundEffect::Reload(const std::string& filePath) 
	{
		Mix_Chunk* newChunk = Audio::Get().LoadSoundChunk(filePath);
		if (!newChunk) return false;

		if (soundChunk_)
			Audio::Get().DestroySoundChunk(soundChunk_);

		soundChunk_ = newChunk;

		Audio::Get().SetSoundChunkVolume(soundChunk_, chunkVolume_);

		return true;
	}



	void SoundEffect::Play() 
	{
		if (!soundChunk_)
		{
			ErrorLogger::Get().LogError("SoundEffect::Play() failure", "SoundChunk is nullptr!");
			return;
		}

		Audio::Get().PlaySoundChunk(-1, soundChunk_, repeatCount_);
	}

	void SoundEffect::Pause() 
	{
		if (!soundChunk_)
		{
			ErrorLogger::Get().LogError("SoundEffect::Pause() failure", "SoundChunk is nullptr!");
			return;
		}

		Audio::Get().PauseSoundChunk(soundChunk_);
	}

	void SoundEffect::Resume()
	{
		if (!soundChunk_)
		{
			ErrorLogger::Get().LogError("SoundEffect::Resume() failure", "SoundChunk is nullptr!");
			return;
		}

		Audio::Get().ResumeSoundChunk(soundChunk_);
	}

	void SoundEffect::Stop() 
	{
		if (!soundChunk_)
		{
			ErrorLogger::Get().LogError("SoundEffect::Stop() failure", "SoundChunk is nullptr!");
			return;
		}

		Audio::Get().StopSoundChunk(soundChunk_);
	}

	void SoundEffect::SetVolume(float volume) 
	{
		volume = GetClamped(volume, 0.0f, 1.0f);

		if(soundChunk_)
		   Audio::Get().SetSoundChunkVolume(soundChunk_, volume);

		chunkVolume_ = volume;
	}

	void SoundEffect::SetRepeatCount(int repeatCount)
	{
		if (repeatCount < -1) repeatCount = -1;

		repeatCount_ = repeatCount;
	}



	float SoundEffect::GetVolume() const
	{
		return chunkVolume_;
	}

	int SoundEffect::GetPlayingChannelCount() const 
	{
		if (!soundChunk_) return 0;
		
		return Audio::Get().GetPlayingChannelCount(soundChunk_);
	}

	int SoundEffect::GetRepeatCount() const
	{
		return repeatCount_;
	}

	bool SoundEffect::IsLoaded() const
	{
		return soundChunk_ != nullptr;
	}

}