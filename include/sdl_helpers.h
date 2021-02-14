#ifndef SDL_HELPERS
#define SDL_HELPERS

#include <SDL/SDL.h>    // Uint8...

// Function type for a SDL audio callback function.
typedef void (*audio_callback_t)(void*, Uint8*, int);

/* Sets up the SDL audio stream with the given callback function and user data.
** Returns 0 on success and 1 on error.
*/
int init_sdl_stream(audio_callback_t callback, void* user_data);

// Closes the SDL audio stream.
void end_sdl_stream(void);

#endif /* ! SDL_HELPERS */
