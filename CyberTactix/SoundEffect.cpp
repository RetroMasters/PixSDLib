#include "SoundEffect.h"
#include "Audio.h"
#include "ErrorLogger.h"

namespace pix
{
	SoundEffect::SoundEffect():
		soundChunk_(nullptr),
		volume_(1.0f),
		repeatCount_(0)
	{
	}

	SoundEffect::SoundEffect(const std::string& filePath, float chunkVolume):
		soundChunk_(nullptr),
		volume_(chunkVolume),
		repeatCount_(0)
	{
		Reload(filePath, chunkVolume);
	}

	SoundEffect::~SoundEffect()
	{
		// The sound effect might just be unused, so it is not necessarily an error when sounChunk_ is nullptr
		if(soundChunk_) 
		  Audio::Get().DestroySoundChunk(soundChunk_);
	}

	bool SoundEffect::Reload(const std::string& filePath, float chunkVolume) 
	{
		if(soundChunk_)
		  Audio::Get().DestroySoundChunk(soundChunk_);

		soundChunk_ = Audio::Get().LoadSoundChunk(filePath);

		if (soundChunk_)
		{
			SetVolume(chunkVolume); 
			return true;
		}

		return false;
	}



	void SoundEffect::Play() 
	{
		Audio::Get().PlaySoundChunk(-1, soundChunk_, repeatCount_);
	}

	void SoundEffect::Pause() 
	{
		Audio::Get().PauseSoundChunk(soundChunk_);
	}

	void SoundEffect::Resume()
	{
		Audio::Get().ResumeSoundChunk(soundChunk_);
	}

	void SoundEffect::Stop() 
	{
		Audio::Get().StopSoundChunk(soundChunk_);
	}

	void SoundEffect::SetVolume(float volume) 
	{
		if (volume < 0.0f) volume = 0.0f;
		if (volume > 1.0f) volume = 1.0f;

		Audio::Get().SetSoundChunkVolume(soundChunk_, volume);

		volume_ = volume;
	}

	void SoundEffect::SetRepeatCount(int loops)
	{
		if (loops < -1) loops = -1;

		repeatCount_ = loops;
	}



	float SoundEffect::GetVolume() const
	{
		return volume_;
	}

	int SoundEffect::GetPlayingChannelCount() const 
	{
		return Audio::Get().GetPlayingChannelCount(soundChunk_);
	}

	int SoundEffect::GetRepeatCount() const
	{
		return repeatCount_;
	}

}