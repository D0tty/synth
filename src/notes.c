#include "notes.h"

/* The frequency for each note (in Hz I suppose).
** Sidenote: I love this syntax!
*/
static const float frequencies[NOTES_RANGE] =
{
    [C]         = 32.7f,
    [C_SHARP]   = 34.65f,
    [D]         = 36.71f,
    [D_SHARP]   = 38.89f,
    [E]         = 41.2f,
    [F]         = 43.65f,
    [F_SHARP]   = 46.25f,
    [G]         = 49.f,
    [G_SHARP]   = 51.91f,
    [A]         = 55.f,
    [A_SHARP]   = 58.27f,
    [B]         = 61.74f,

    [NOTHING]   = 0.f,
};

float note_to_frequency(enum note note)
{
    return frequencies[note];
}
