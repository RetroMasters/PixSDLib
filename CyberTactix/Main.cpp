#include <iostream>

//#include "Input.h"

#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_image.h>
#include<SDL_ttf.h>

#include "ErrorLogger.h"
#include "Window.h"
#include "SpriteMeshAnimator.h"

#include "Gameloop.h"
#include "ClassStyleReference.h"



int main(int argc, char* argv[])
{
    SDL_Init(SDL_INIT_VIDEO);

    pix::ErrorLogger::Get().Init(pix::GameLoop::GetPrefPath("RetroMasters", "Cyber Tactix"), 4);

    pix::Window::Get().Init("Cyber Tactix", 800, 600, true);

    SDL_Delay(4000);

    pix::Window::Get().Destroy();
   
    SDL_Quit();
    return 0;
}