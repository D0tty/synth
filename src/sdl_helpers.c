#include "sdl_helpers.h"

#include <SDL/SDL.h>

#include "values.h"

int init_sdl_stream(audio_callback_t callback, void* user_data)
{
    SDL_Init(SDL_INIT_AUDIO | SDL_INIT_TIMER);

    SDL_AudioSpec audiospec;
    audiospec.freq = FREQUENCY;
    audiospec.format = AUDIO_S16SYS;
    audiospec.channels = 2;
    audiospec.samples = 1024;
    audiospec.callback = callback;
    audiospec.userdata = user_data;

    int sdl_open_res = SDL_OpenAudio(&audiospec, NULL);
    if (sdl_open_res >= 0)
        SDL_PauseAudio(0);

    return sdl_open_res;
}

void end_sdl_stream(void)
{
    SDL_CloseAudio();
    SDL_Quit();
}
