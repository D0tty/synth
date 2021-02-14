#include <stdio.h>
#include <unistd.h>

#include "notes.h"
#include "rt_note_player.h"

#define LAST_NOTE_IDX NOTES_RANGE - 1

// Get a note index from user input
int get_note_idx(void)
{
    char in = getchar();

    if (in < 'a' || in > 'a' + LAST_NOTE_IDX)
        return NOTHING;

    return in - 'a';
}

int main(void)
{
    audio_setup();

    while (1)
    {
        for (int node_idx = 0; node_idx < NOTES_RANGE - 1; node_idx++)
        {
            int node_idx = get_note_idx();
            set_note(node_idx);
            sleep(1);
        }
    }
}
