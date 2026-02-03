#pragma once

#include <SDL_mixer.h>
#include <string>
#include"Uncopyable.h"

namespace pix
{
    // The SoundEffect class manages the lifetime and replay functionality of sound chunks.
    // It is built on top of the Audio singleton. 
    // 
    // Philosophy:
    // We want fire-and-forget functionality, sounds that can be stacked, but also paused and resumed at once.
    class SoundEffect : private Uncopyable
    {
    public:
        // ------------ INITIALIZATION ---------------------

        SoundEffect(); 
        SoundEffect(const std::string& filePath, float chunkVolume = 1.0f);
        ~SoundEffect();

        // Reload() reloads a new sound chunk from file and sets its volume (the state of the repeat count remains unchanged).
        // Returns true if reloaded successfully, false otherwise.
        bool Reload(const std::string& filePath, float chunkVolume = 1.0f);

        // ----------------- FUNCTIONALITY ---------------------

        // Starts a new sound instance on a free channel, does not affect already playing or paused ones
        void Play();
        // Pauses all sound instances currently playing
        void Pause();
        // Resumes all sound instances previously paused
        void Resume();
        // Stops and removes all sound instances (playing or paused)
        void Stop();

        //  Sets the chunk volume in range [0,1]
        void  SetVolume(float volume);

        // Playing sound will repeat repeatCount times. If repeatCount is -1, it will loop "infinitely" (~65000 times). 
        void  SetRepeatCount(int repeatCount);

        // ------------- GETTERS -----------------

        float GetVolume() const;

        // Returns how many simultaneous channels are playing the sound
        int   GetPlayingChannelCount() const;

        int   GetRepeatCount() const;

    private:

        Mix_Chunk* soundChunk_;
        float      volume_;
        int        repeatCount_;
    };

}