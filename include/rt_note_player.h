#ifndef RT_NOTE_PLAYER_H
#define RT_NOTE_PLAYER_H

#define INSTRUMENT GAME_BOY_IDX

// Sets up the audio stream with predefined parameters (see source file).
void audio_setup(void);

// Sets the static note (see source file) to the given value.
void set_note(int note_idx);

#endif /* ! RT_NOTE_PLAYER_H */
