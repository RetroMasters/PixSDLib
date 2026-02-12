#include "Audio.h"
#include "ErrorLogger.h"
#include "PixMath.h"

namespace pix
{

	Audio& Audio::Get() 
	{
		static Audio soundManager_;
		return soundManager_;
	}


	bool Audio::Init()
	{
		if (isInitialized_) return true;

		// Mix_Init() is optional, but initializing common codecs upfront improves error reporting
		int flags = Mix_Init(MIX_INIT_MP3 | MIX_INIT_OGG);
		if((MIX_INIT_MP3 & flags) == 0)
			ErrorLogger::Get().LogError("Audio::Init() - Mix_Init() fail", "MP3 codec not available (files will fail to load if used)!");
		if ((MIX_INIT_OGG & flags) == 0)
			ErrorLogger::Get().LogError("Audio::Init() - Mix_Init() fail", "OGG codec not available (files will fail to load if used)!");

		if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) != 0)
		{
			ErrorLogger::Get().LogSDLError("Audio::Init() - Mix_OpenAudio() fail");
			return false;
		}

		SetMasterVolume(GetMasterVolume());

		isInitialized_ = true;
		return true;
	}

	void Audio::Destroy()
	{
		if (!isInitialized_) return;

		DeleteMusic();

		Mix_CloseAudio();
		Mix_Quit();

		musicTrack_ = nullptr;
		masterVolume_ = 0.3f;
		channelVolume_ = 1.0f;
		musicVolume_ = 1.0f;
		musicRepeatCount_ = -1;
		isInitialized_ = false;
	}



	void Audio::ReloadMusic(const std::string& path)
	{
		if (musicTrack_ != nullptr)
		{
			Mix_HaltMusic();
			Mix_FreeMusic(musicTrack_);
		}

		musicTrack_ = Mix_LoadMUS(path.c_str());

		if (musicTrack_ == nullptr)
			ErrorLogger::Get().LogSDLError("Audio::ReloadMusic() - Mix_LoadMUS() fail");
	}

	void Audio::DeleteMusic()
	{
		if (!musicTrack_) return;
		
			Mix_HaltMusic();
			Mix_FreeMusic(musicTrack_);
			musicTrack_ = nullptr;
	}

	void Audio::PlayMusic()
	{
		if (musicTrack_ == nullptr)
		{
			ErrorLogger::Get().LogError("Audio::PlayMusic() fail", "Music track is nullptr!");
			return;
		}

		Mix_HaltMusic();

		if (Mix_PlayMusic(musicTrack_, musicRepeatCount_) != 0)
			ErrorLogger::Get().LogSDLError("Audio::PlayMusic - Mix_PlayMusic() fail");
	}

	void Audio::PauseMusic()
	{
		if (musicTrack_ == nullptr) return;

		Mix_PauseMusic();
	}

	void Audio::ResumeMusic()
	{
		if (musicTrack_ == nullptr) return;

		Mix_ResumeMusic();
	}

	void Audio::StopMusic()
	{
		if (musicTrack_ == nullptr) return;

		Mix_HaltMusic();
	}

	void Audio::SetMusicVolume(float volume)
	{
		musicVolume_ = GetClampedValue(volume, 0.0f, 1.0f);
		Mix_VolumeMusic((musicVolume_ * MIX_MAX_VOLUME) * GetMasterVolume());
	}

	void Audio::SetMusicRepeatCount(int loops)
	{
		if (loops < -1) loops = -1;

		musicRepeatCount_ = loops;
	}

	void Audio::SetMusicPosition(double timePosition)
	{
		if (timePosition < 0.0) timePosition = 0.0;

		if (Mix_SetMusicPosition(timePosition) != 0)
			ErrorLogger::Get().LogSDLError("Audio::SetMusicPosition() - Mix_SetMusicPosition() fail");
	}



	Mix_Chunk* Audio::LoadSoundChunk(const std::string& path)
	{
		Mix_Chunk* newChunk = Mix_LoadWAV(path.c_str());

		if (newChunk == nullptr)
			ErrorLogger::Get().LogError("Audio::LoadSoundChunk() fail", "Could not load " + path);

		return newChunk;
	}

	void Audio::DestroySoundChunk(Mix_Chunk* soundChunk)
	{
		if (soundChunk == nullptr)
		{
			ErrorLogger::Get().LogError("Audio::DestroySoundChunk() fail", "SoundChunk is nullptr!");
			return;
		}

		Mix_FreeChunk(soundChunk);
	}

	void Audio::PlaySoundChunk(int channel, Mix_Chunk* soundChunk, int repeatCount)
	{
		if (soundChunk == nullptr)
		{
			ErrorLogger::Get().LogError("Audio::PlaySoundChunk() fail", "SoundChunk is nullptr!");
			return;
		}

		if ((Mix_PlayChannel(channel, soundChunk, repeatCount) == -1))
			ErrorLogger::Get().LogSDLError("Audio::PlaySoundChunk() - Mix_PlayChannel() fail");
	}

	void Audio::PauseSoundChunk(Mix_Chunk* soundChunk)
	{
		if (soundChunk == nullptr) return;

		int channelCount = GetAllocatedChannelCount();

		for (int i = 0; i < channelCount; i++)
		{
			if (Mix_GetChunk(i) == soundChunk)
			{
				Mix_Pause(i);
			}
		}
	}

	void Audio::ResumeSoundChunk(Mix_Chunk* soundChunk)
	{
		if (soundChunk == nullptr) return;

		int channelCount = GetAllocatedChannelCount();

		for (int i = 0; i < channelCount; i++)
		{
			if (Mix_GetChunk(i) == soundChunk)
				Mix_Resume(i);
		}
	}

	void Audio::StopSoundChunk(Mix_Chunk* soundChunk)
	{
		if (soundChunk == nullptr)
		{
			ErrorLogger::Get().LogError("Audio::StopSoundChunk() fail", "SoundChunk is nullptr!");
			return;
		}

		int channelCount = GetAllocatedChannelCount();

		for (int i = 0; i < channelCount; i++)
		{
			if (Mix_GetChunk(i) == soundChunk)
			{
				if (Mix_HaltChannel(i) != 0)
					ErrorLogger::Get().LogSDLError("Audio::StopSoundChunk() - Mix_GetChunk() fail");
			}
		}
	}

	void Audio::SetSoundChunkVolume(Mix_Chunk* soundChunk, float volume)
	{
		if (soundChunk == nullptr)
		{
			ErrorLogger::Get().LogError("Audio::SetSoundChunkVolume() fail", "SoundChunk is nullptr!");
			return;
		}

		Mix_VolumeChunk(soundChunk, GetClampedValue(volume, 0.0f, 1.0f) * MIX_MAX_VOLUME);
	}



	int Audio::ReallocChannels(int channelCount) 
	{
		if (channelCount < 0) channelCount = 0;

		return Mix_AllocateChannels(channelCount);
	}

	void Audio::PauseAllChannels()
	{
		Mix_Pause(-1);
	}

	void Audio::ResumeAllPausedChannels()
	{
		Mix_Resume(-1);
	}

	void Audio::StopAllChannels()
	{
		if (Mix_HaltChannel(-1) != 0)
			ErrorLogger::Get().LogSDLError("Audio::StopAllChannels() - Mix_HaltChannel() fail");
	}

	void Audio::SetChannelVolume(float volume)
	{
		channelVolume_ = GetClampedValue(volume, 0.0f, 1.0f);
		Mix_Volume(-1, (channelVolume_ * MIX_MAX_VOLUME) * GetMasterVolume());
	}

	void Audio::SetMasterVolume(float volume)
	{
		masterVolume_ = GetClampedValue(volume, 0.0f, 1.0f);

		// Update physical volume
		SetChannelVolume(GetChannelVolume()); 
		SetMusicVolume(GetMusicVolume());
	}

    

	bool Audio::IsMusicPaused() const
	{
		return Mix_PausedMusic();
	}

	float Audio::GetMusicVolume() const
	{
		return musicVolume_;
	}

	double Audio::GetMusicPosition() const
	{
		if (musicTrack_ == nullptr)
		{
			ErrorLogger::Get().LogError("Audio::GetMusicPosition() fail", "Music track is nullptr!");
			return -1.0;
		}

		double position = Mix_GetMusicPosition(musicTrack_);
		if (position < 0.0)
			ErrorLogger::Get().LogError("Audio::GetMusicPosition() - Mix_GetMusicPosition() fail", "Getting music position is not supported for the used codec!");

		return position;
	}

	int Audio::GetMusicRepeatCount() const
	{
		return musicRepeatCount_;
	}

	int Audio::GetAllocatedChannelCount() const
	{
		return Mix_AllocateChannels(-1);
	}

	int Audio::GetOccupiedChannelCount() const 
	{
		return Mix_Playing(-1); //"Mix_Playing() does not check if the channel has been paused."
	}

	float Audio::GetChannelVolume() const 
	{
		return channelVolume_; //Mix_Volume(-1,-1);
	}

	float Audio::GetMasterVolume() const 
	{
		return masterVolume_;
	}

	int Audio::GetPlayingChannelCount(Mix_Chunk* soundChunk) const 
	{
		if (soundChunk == nullptr)
		{
			ErrorLogger::Get().LogError("Audio::GetPlayingChannelCount() fail", "SoundChunk is nullptr!");
			return 0;
		}

		int channelCount = GetAllocatedChannelCount();

		int playingChannelCount = 0;

		for (int i = 0; i < channelCount; i++)
		{                      //"Mix_Playing() does not check if the channel has been paused."
			if ((Mix_GetChunk(i) == soundChunk) && (Mix_Paused(i) == 0) && (Mix_Playing(i) > 0))
			{
				playingChannelCount++;
			}
		}

		return playingChannelCount;
	}



	Audio::Audio():
		musicTrack_(nullptr),
		masterVolume_(0.3f),
		channelVolume_(1.0f),
		musicVolume_(1.0f),
		musicRepeatCount_(-1),
		isInitialized_(false)
	{
	}

	Audio::~Audio()
	{
		Destroy();
	}

}