#include <iostream>

#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_image.h>
#include<SDL_ttf.h>

#include "ErrorLogger.h"
#include "Window.h"

//#include <>


// The error logger is initialized by setting the output path using SDL_GetPrefPath() and creating "ErrorLog.txt" to store logged errors.
// On Windows, the output path might look like this: C:\\Users\\bob\\AppData\\Roaming\\companyName\\appName\\ErrorLog.txt
std::string GetPrefPath(const std::string& companyName, const std::string& appName)
{
    char* prefPath = SDL_GetPrefPath(companyName.c_str(), appName.c_str());
    if (!prefPath) return std::string();

    std::string result = std::string(prefPath);
    SDL_free(prefPath);

    return result;
}

int main(int argc, char* argv[])
{
    SDL_Init(SDL_INIT_VIDEO);

    pix::ErrorLogger::Get().Init(GetPrefPath("RetroMasters", "Cyber Tactix"), 4);

    pix::Window::Get().Init(800, 600, false, "Cyber Tactix");

    pix::Window::Get().SetFullscreen(true);

    pix::ErrorLogger::Get().LogSDLError("LastSDLError");

    SDL_Delay(5000);

    pix::Window::Get().Destroy();
    //SDL_DestroyWindow(window);

    pix::ErrorLogger& Log = pix::ErrorLogger::Get();


    SDL_Quit();
    return 0;
}