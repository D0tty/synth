#ifndef INSTRUMENTS_H
#define INSTRUMENTS_H

#define NB_INSTRUMENTS  4

#define SIMPLE_SINE_IDX 0
#define XYLOPHONE_IDX   1
#define FLUTE_IDX       2
#define GAME_BOY_IDX    3

/* Function defining the value of the signal at a point in time depending on
** the frequency being played.
*/
typedef float (*val_at_pos_t)(float freq, int pos, float ampl);

// Various instruments.
float simple_sine(float freq, int pos, float ampl);
float xylophone(float freq, int pos, float ampl);
float flute(float freq, int pos, float ampl);
float game_boy(float freq, int pos, float ampl);

extern val_at_pos_t instruments[NB_INSTRUMENTS];

#endif /* ! INSTRUMENTS_H */
