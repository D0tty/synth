#include "rt_note_player.h"

#include <err.h>

#include <SDL/SDL.h>

#include "instruments.h"
#include "notes.h"
#include "sdl_helpers.h"
#include "values.h"

// The current note.
static enum note s_note = NOTHING;

// Stream variables.
static int s_pos;
static float s_ampl;

// Reset the stream variables.
static void reset_stream_variables(void)
{
    s_pos = 0;
    s_ampl = 1.f;
}

// Writes the current note's frequency in the stream.
static void audio_callback (void* user_data, Uint8* stream, int len)
{
    // Unused, as everything is static.
    (void)user_data;

    for (int index = 0; index < len; index++)
    {
        float freq = note_to_frequency(s_note);
        float value = instruments[INSTRUMENT](freq, s_pos, s_ampl);
        stream[index] = (Uint8)(value * BASE_VOLUME / 2.f);

        s_ampl *= AMPL_REDUCTION;
        s_pos++;

        /* If the end of the stream was reached (wild guess), reset stream
        ** variables.
        */
        if (s_pos >= FREQUENCY)
            reset_stream_variables();
    }
}

void audio_setup(void)
{
    reset_stream_variables();
    int sdl_open_res = init_sdl_stream(audio_callback, NULL);
    if (sdl_open_res < 0)
        errx(1, "SDL open failure.");
}

void set_note(int note_idx)
{
    if (note_idx < 0 || note_idx >= NOTES_RANGE)
        errx(2, "Wrong note value provided.");

    s_note = note_idx;
}
