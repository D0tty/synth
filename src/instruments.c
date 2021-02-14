#include "instruments.h"

#define NYQIST_FLT  44100.f

#include <math.h>

#include "notes.h"

val_at_pos_t instruments[NB_INSTRUMENTS] =
{
    [SIMPLE_SINE_IDX]   = simple_sine,
    [XYLOPHONE_IDX]     = xylophone,
    [FLUTE_IDX]         = flute,
    [GAME_BOY_IDX]      = game_boy,
};

static float relative_pos (int pos)
{
    return pos / NYQIST_FLT;
}

float simple_sine(float freq, int pos, float ampl)
{
    return ampl * sinf(M_PI * freq * relative_pos(pos));
}

float xylophone(float freq, int pos, float ampl)
{
    (void) ampl;
    float pos_rel = relative_pos(pos);

    float env = 1.f - pos_rel;
    env *= 1.f + 0.1f * sinf(10.f * M_PI * pos_rel);
    return env * sinf(M_PI * freq * pos_rel);
}

float flute(float freq, int pos, float ampl)
{
    float pos_rel = relative_pos(pos);
    float pi_pos_rel = M_PI * pos_rel;
    float total = freq * pi_pos_rel;

    float res = sinf(total) - 0.3f + sinf(2.f * total) * 0.2f
        + sinf(3.f * total) * 0.3f + sinf(4.f * total) * 0.2f;
    res *= sinf(pi_pos_rel);
    res *= 1.f + 0.1f * sinf(2.f * pi_pos_rel);
    return res * ampl;
}

float game_boy(float freq, int pos, float ampl)
{
    (void)ampl;

    float res = fmodf(2.f * freq * relative_pos(pos), 2);
    if (res > 1.f)
        res = 1.f - res;
    return res - 0.5f;
}
