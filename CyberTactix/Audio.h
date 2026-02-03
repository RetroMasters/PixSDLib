#pragma once

#include <SDL_mixer.h>
#include <string>
#include"Uncopyable.h"

namespace pix
{
    // The Audio singleton manages the lifetime of the default audio device provided by
    // SDL_mixer and exposes simple functionality for managing stereo sound.
    //
    // Technical note:
    // SDL_mixer has two separate data structures for audio data. One it calls a
    // "chunk," which is meant to be a file completely decoded into memory up
    // front, and the other it calls "music" which is a file intended to be
    // decoded on demand. Historically, uncompressed formats like WAV were intended 
    // for chunks, while compressed formats like MP3 were streamed as music.
    //   
    // In modern times, this isn't split by format anymore, and most are
    // interchangeable, so the question is what the app thinks is worth
    // predecoding or not. Chunks might take more memory, but once they are loaded
    // won't need to decode again, whereas music always needs to be decoded on the
    // fly. Also, crucially, there are as many channels for chunks as the app can
    // allocate, but SDL_mixer only offers a single "music" channel.
    // 
    // This class is not thread-safe; all calls must occur on the same thread.
    // 
    // Philosophy:
    // The purpose of the Audio singleton is to have simple functionality for managing stereo sound centralized.
    // This should be sufficient for most games using stereo sound. For more sophisticated audio, a different audio system should be used.
    class Audio : private Uncopyable
    {
    public:

        // Returns the Audio instance
        static Audio& Get();

        // Initializes the Audio singleton.
        // This will internally call SDL_Init(SDL_INIT_AUDIO) if it hasn't been initialized yet. 
        // It opens the default device for stereo sound with reasonable defaults.
        // If MP3 or OGG is not supported, the ErrorLogger will log this.
        // Returns true if initialization succeeds or if Audio is already initialized, false otherwise.
        bool Init();

        // Destroys the Audio
        void Destroy();

        // ------------- MUSIC -----------------------

        // Halts the current track and loads a new one. There is only one "music" channel.
        void ReloadMusic(const std::string& path);

        // Halts and deletes music
        void DeleteMusic();

        void PlayMusic();

        void PauseMusic();

        void ResumeMusic();

        // This will stop further playback of music until a new music object is started
        void StopMusic();

        // Set music volume between [0, 1]
        void SetMusicVolume(float volume);

        // Playing sound will repeat repeatCount times. If repeatCount == -1, it will loop "infinitely" (approx 65000 times). 
        void SetMusicRepeatCount(int repeatCount);

        // Set the current time position in the music stream, in seconds
        void SetMusicPosition(double timePosition);


        // ---------------- SOUND CHUNK --------------------

        // Loads a supported audio format fully into memory as a sound chunk
        Mix_Chunk* LoadSoundChunk(const std::string& path);

        void DestroySoundChunk(Mix_Chunk* soundChunk);

        void PlaySoundChunk(int channel, Mix_Chunk* soundChunk, int repeatCount);

        void PauseSoundChunk(Mix_Chunk* soundChunk);

        void ResumeSoundChunk(Mix_Chunk* soundChunk);

        void StopSoundChunk(Mix_Chunk* soundChunk);

        void SetSoundChunkVolume(Mix_Chunk* soundChunk, float volume);

        // ------------------ CHANNELS ---------------------

        // Reallocates the specified number of channels for simultaneous playback of sound chunks. 
        // Returns the actual number of channels allocated.
        int ReallocChannels(int channelCount);

        void PauseAllChannels();

        void ResumeAllPausedChannels();

        void StopAllChannels();

        // Set the volume for all channels (except music) in the range [0, 1]
        void SetChannelVolume(float volume);

        // Sets master volume in range [0, 1]. All channel volumes will be multiplied by the master volume.
        void SetMasterVolume(float volume);

        // ------------------- GETTERS -------------------------

        bool IsMusicPaused() const;

        float GetMusicVolume() const;

        // Gets the current time position in the music stream, in seconds
        // Returns -1.0 on fail
        double GetMusicPosition() const;

        int  GetMusicRepeatCount() const;

        int  GetAllocatedChannelCount() const;

        // Returns the amount of currently playing/paused channels
        int  GetOccupiedChannelCount() const;

        float GetChannelVolume() const;

        float GetMasterVolume() const;

        // Returns the number of active (non-paused) channels currently playing the sound chunk
        int GetPlayingChannelCount(Mix_Chunk* soundChunk) const;

    private:

        Audio();
        ~Audio();

        Mix_Music* musicTrack_;
        float      masterVolume_;
        float      channelVolume_;
        float      musicVolume_;  // Music is running on a dedicated separate channel.
        int        musicRepeatCount_;  //current SDL-BUG: 0,1 play both 1x, 2 plays 3x (correctly)
        bool       isInitialized_;
    };

}
