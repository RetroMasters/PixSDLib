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
			ErrorLogger::Get().LogError("Audio::Init() - Mix_Init() failure", "MP3 codec not available (files will fail to load if used)!");
		if ((MIX_INIT_OGG & flags) == 0)
			ErrorLogger::Get().LogError("Audio::Init() - Mix_Init() failure", "OGG codec not available (files will fail to load if used)!");

		if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) != 0)
		{
			ErrorLogger::Get().LogSDLError("Audio::Init() - Mix_OpenAudio() failure");
			Mix_Quit();
			return false;
		}

		isInitialized_ = true;

		SetMasterVolume(masterVolume_); // Audio must be initialized for the methods to succeed

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
		if (!isInitialized_) return;

		Mix_Music* newTrack = Mix_LoadMUS(path.c_str());

		if (!newTrack)
		{
			ErrorLogger::Get().LogSDLError("Audio::ReloadMusic() - Mix_LoadMUS() failure");
			return;
		}

		DeleteMusic();
		musicTrack_ = newTrack;
	}


	void Audio::DeleteMusic()
	{
		if (!isInitialized_ || !musicTrack_) return;
		
		Mix_HaltMusic();
		Mix_FreeMusic(musicTrack_);
		musicTrack_ = nullptr;
	}

	void Audio::PlayMusic()
	{
		if (!isInitialized_ || !musicTrack_) return;

		Mix_HaltMusic();

		if (Mix_PlayMusic(musicTrack_, musicRepeatCount_) != 0)
			ErrorLogger::Get().LogSDLError("Audio::PlayMusic() - Mix_PlayMusic() failure");
	}

	void Audio::PauseMusic()
	{
		if (!isInitialized_ || !musicTrack_) return;

		Mix_PauseMusic();
	}

	void Audio::ResumeMusic()
	{
		if (!isInitialized_ || !musicTrack_) return;

		Mix_ResumeMusic();
	}

	void Audio::StopMusic()
	{
		if (!isInitialized_ || !musicTrack_) return;

		Mix_HaltMusic();
	}

	void Audio::SetMusicVolume(float volume)
	{
		if (!isInitialized_) return;

		musicVolume_ = GetClamped(volume, 0.0f, 1.0f);
		Mix_VolumeMusic((musicVolume_ * MIX_MAX_VOLUME) * GetMasterVolume());
	}

	void Audio::SetMusicRepeatCount(int loops)
	{
		if (loops < -1) loops = -1;

		musicRepeatCount_ = loops;
	}

	void Audio::SetMusicPosition(double timePosition)
	{
		if (!isInitialized_ || !musicTrack_) return;

		if (timePosition < 0.0) timePosition = 0.0;

		if (Mix_SetMusicPosition(timePosition) != 0)
			ErrorLogger::Get().LogSDLError("Audio::SetMusicPosition() - Mix_SetMusicPosition() failure");
	}

	Mix_Chunk* Audio::LoadSoundChunk(const std::string& path)
	{
		if (!isInitialized_) return nullptr;

		Mix_Chunk* newChunk = Mix_LoadWAV(path.c_str());

		if (!newChunk)
			ErrorLogger::Get().LogSDLError("Audio::LoadSoundChunk() - Mix_LoadWAV() failure");

		return newChunk;
	}

	void Audio::DestroySoundChunk(Mix_Chunk* soundChunk)
	{
		if (!isInitialized_) return;

		if (!soundChunk)
		{
			ErrorLogger::Get().LogError("Audio::DestroySoundChunk() failure", "SoundChunk is nullptr!");
			return;
		}

		Mix_FreeChunk(soundChunk);
	}

	void Audio::PlaySoundChunk(int channel, Mix_Chunk* soundChunk, int repeatCount)
	{
		if (!isInitialized_) return;

		if (!soundChunk)
		{
			ErrorLogger::Get().LogError("Audio::PlaySoundChunk() failure", "SoundChunk is nullptr!");
			return;
		}

		if (repeatCount < -1) repeatCount = -1;

		if ((Mix_PlayChannel(channel, soundChunk, repeatCount) == -1))
			ErrorLogger::Get().LogSDLError("Audio::PlaySoundChunk() - Mix_PlayChannel() failure");
	}

	void Audio::PauseSoundChunk(Mix_Chunk* soundChunk)
	{
		if (!isInitialized_) return;

		if (!soundChunk)
		{
			ErrorLogger::Get().LogError("Audio::PauseSoundChunk() failure", "SoundChunk is nullptr!");
			return;
		}

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
		if (!isInitialized_) return;

		if (!soundChunk)
		{
			ErrorLogger::Get().LogError("Audio::ResumeSoundChunk() failure", "SoundChunk is nullptr!");
			return;
		}

		int channelCount = GetAllocatedChannelCount();

		for (int i = 0; i < channelCount; i++)
		{
			if (Mix_GetChunk(i) == soundChunk)
				Mix_Resume(i);
		}
	}

	void Audio::StopSoundChunk(Mix_Chunk* soundChunk)
	{
		if (!isInitialized_) return;

		if (!soundChunk)
		{
			ErrorLogger::Get().LogError("Audio::StopSoundChunk() failure", "SoundChunk is nullptr!");
			return;
		}

		int channelCount = GetAllocatedChannelCount();

		for (int i = 0; i < channelCount; i++)
		{
			if (Mix_GetChunk(i) == soundChunk)
			{
				if (Mix_HaltChannel(i) != 0)
					ErrorLogger::Get().LogSDLError("Audio::StopSoundChunk() - Mix_HaltChannel() failure");
			}
		}
	}

	void Audio::SetSoundChunkVolume(Mix_Chunk* soundChunk, float volume)
	{
		if (!isInitialized_) return;

		if (!soundChunk)
		{
			ErrorLogger::Get().LogError("Audio::SetSoundChunkVolume() failure", "SoundChunk is nullptr!");
			return;
		}

		Mix_VolumeChunk(soundChunk, GetClamped(volume, 0.0f, 1.0f) * MIX_MAX_VOLUME);
	}

	int Audio::ReallocChannels(int channelCount)
	{
		if (!isInitialized_) return 0;

		if (channelCount < 0) channelCount = 0;

		int allocatedCount = Mix_AllocateChannels(channelCount);

		SetChannelVolume(channelVolume_);

		return allocatedCount;
	}

	void Audio::PauseAllChannels()
	{
		if (!isInitialized_) return;

		Mix_Pause(-1);
	}

	void Audio::ResumeAllPausedChannels()
	{
		if (!isInitialized_) return;

		Mix_Resume(-1);
	}

	void Audio::StopAllChannels()
	{
		if (!isInitialized_) return;

		if (Mix_HaltChannel(-1) != 0)
			ErrorLogger::Get().LogSDLError("Audio::StopAllChannels() - Mix_HaltChannel() failure");
	}

	void Audio::SetChannelVolume(float volume)
	{
		if (!isInitialized_) return;

		channelVolume_ = GetClamped(volume, 0.0f, 1.0f);
		Mix_Volume(-1, (channelVolume_ * MIX_MAX_VOLUME) * GetMasterVolume());
	}

	void Audio::SetMasterVolume(float volume)
	{
		if (!isInitialized_) return;

		masterVolume_ = GetClamped(volume, 0.0f, 1.0f);

		// Update physical volume
		SetChannelVolume(GetChannelVolume()); 
		SetMusicVolume(GetMusicVolume());
	}

    

	bool Audio::IsMusicPaused() const
	{
		if (!isInitialized_) return false;

		return Mix_PausedMusic();
	}

	float Audio::GetMusicVolume() const
	{
		return musicVolume_;
	}

	double Audio::GetMusicPosition() const
	{
		if (!isInitialized_) return -1.0;

		if (!musicTrack_)
		{
			ErrorLogger::Get().LogError("Audio::GetMusicPosition() failure", "Music track is nullptr!");
			return -1.0;
		}

		double position = Mix_GetMusicPosition(musicTrack_);
		if (position < 0.0)
			ErrorLogger::Get().LogError("Audio::GetMusicPosition() - Mix_GetMusicPosition() failure", "Getting music position is not supported for the used codec!");

		return position;
	}

	int Audio::GetMusicRepeatCount() const
	{
		return musicRepeatCount_;
	}

	int Audio::GetAllocatedChannelCount() const
	{
		if (!isInitialized_) return 0;

		return Mix_AllocateChannels(-1);
	}

	int Audio::GetOccupiedChannelCount() const 
	{
		if (!isInitialized_) return 0;

		return Mix_Playing(-1); // Mix_Playing() also counts paused channels
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
		if (!isInitialized_) return 0;

		if (!soundChunk)
		{
			ErrorLogger::Get().LogError("Audio::GetPlayingChannelCount() failure", "SoundChunk is nullptr!");
			return 0;
		}

		int channelCount = GetAllocatedChannelCount();

		int playingChannelCount = 0;

		for (int i = 0; i < channelCount; i++)
		{                      
			// Mix_Playing() also counts paused channels
			if ((Mix_GetChunk(i) == soundChunk) && (Mix_Paused(i) == 0) && (Mix_Playing(i) > 0))
			{
				playingChannelCount++;
			}
		}

		return playingChannelCount;
	}


	Audio::~Audio()
	{
		Destroy();
	}

}