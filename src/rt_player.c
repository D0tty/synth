#include <stdio.h>
#include <unistd.h>

#include "notes.h"
#include "rt_note_player.h"

int main(void)
{
    audio_setup();

    while (1)
    {
        for (int node_idx = 0; node_idx < NOTES_RANGE; node_idx++)
        {
            printf("Playing note %d\n", node_idx);
            set_note(node_idx);
            sleep(1);
        }
    }
}
