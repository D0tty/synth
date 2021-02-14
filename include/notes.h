#ifndef NOTES_H
#define NOTES_H

// Number of notes in our range.
#define NOTES_RANGE 12

// Representation of the notes as an enum.
enum note
{
    C = 0,
    C_SHARP,
    D,
    D_SHARP,
    E,
    F,
    F_SHARP,
    G,
    G_SHARP,
    A,
    A_SHARP,
    B,
};

// Returns the frequency corresponding to the given note (in Hz I suppose).
float note_to_frequency(enum note note);

#endif /* ! NOTES_H */
