#include "sdl_helpers.h"

#include <SDL/SDL.h>

int init_sdl_stream(audio_callback_t callback, void* user_data)
{
    SDL_Init(SDL_INIT_AUDIO | SDL_INIT_TIMER);

    SDL_AudioSpec audiospec;
    audiospec.freq = 44100;
    audiospec.format = AUDIO_S16SYS;
    audiospec.channels = 2;
    audiospec.samples = 1024;
    audiospec.callback = callback;
    audiospec.userdata = user_data;

    return SDL_OpenAudio(&audiospec, NULL);
}

void end_sdl_stream(void)
{
    SDL_CloseAudio();
    SDL_Quit();
}
