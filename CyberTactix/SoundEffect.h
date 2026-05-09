#pragma once

#include <SDL_mixer.h>
#include <string>
#include "Uncopyable.h"

namespace pix
{
     // SoundEffect manages the lifetime and playback of a short sound chunk.
     // It is built on top of the Audio singleton.
     //
     // Philosophy:
     // Provides fire-and-forget playback for short sound effects.
     // Multiple instances of the same sound can overlap, while all instances of this sound
     // can still be paused, resumed, or stopped together.
    class SoundEffect : private Uncopyable
    {
    public:
        // ------------ INITIALIZATION ---------------------

        SoundEffect() = default; 
        SoundEffect(const std::string& filePath, float chunkVolume = 1.0f, int repeatCount = 0);
        ~SoundEffect();

        // Loads a new sound chunk from file while preserving volume and repeat count. 
        // Existing loaded data is replaced only if loading succeeds.
        // Replacing a loaded chunk stops existing instances of the old chunk. 
        // Returns true if loading succeeds, false otherwise.
        bool Reload(const std::string& filePath);

        // ----------------- FUNCTIONALITY ---------------------

        // Starts a new sound instance on a free channel.
        // Does not affect already playing or paused instances.
        void Play();
        
        // Pauses all active instances of this sound effect
        void Pause();

        // Resumes all paused instances of this sound effect
        void Resume();

        // Stops all active instances of this sound effect
        void Stop();

        // Sets the chunk volume in the clamped range [0, 1]. 
        // A sound chunk does not need to be loaded.
        void SetVolume(float volume);

        // Sets how often playback repeats after the first play for new sound instances.
        // If repeatCount == -1, SDL_mixer loops it approximately 65000 times.
        void SetRepeatCount(int repeatCount);

        // ------------- GETTERS -----------------

        float GetVolume() const;

        // Returns how many channels are currently playing this sound effect
        int GetPlayingChannelCount() const;

        int GetRepeatCount() const;

        bool IsLoaded() const;

    private:

        Mix_Chunk* soundChunk_ = nullptr;
        float chunkVolume_ = 1.0f;
        int repeatCount_ = 0;
    };

}