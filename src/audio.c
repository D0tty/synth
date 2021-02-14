#include <math.h>

#include <SDL/SDL.h>

#include "instruments.h"
#include "notes.h"
#include "sdl_helpers.h"

static int g_debug = 0;

typedef struct {
    unsigned int instrument;
    float volume;
    unsigned int length;
    float *notes;
} Channel;

typedef struct Chunk_s {
    unsigned int nb_channels;
    unsigned int pos;
    Channel **channels;
    struct Chunk_s *next;
} Chunk;

typedef struct {
    int ended;
    unsigned int nb_chunks;
    Chunk *chunks_f;
    Chunk *chunks_l;
} Song;

static void audio_callback(void *udata, Uint8 *stream, int len)
{
    Song *song = (Song*)udata;
    static float ampl = 1.0f;
    static int pos = 0;
    static Chunk *chunk = NULL;
    static size_t chunk_len = 0;
    int i = 0;

    if(!chunk && !song->ended)
    {
        size_t j;
        chunk = song->chunks_f;
        for(j = 0; j < chunk->nb_channels; j++)
            if(chunk->channels[j]->length > chunk_len)
                chunk_len = chunk->channels[j]->length;
    }

    for(; i < len; i++)
    {
        size_t j;
        stream[i] = 0;
        if(song->ended)
            continue;
        for(j = 0; j < chunk->nb_channels; j++)
        {
            if(chunk->channels[j]->length > chunk->pos)
            {
                float freq = chunk->channels[j]->notes[chunk->pos];
                int index = chunk->channels[j]->instrument;
                float s = instruments[index](freq, pos, ampl);
                stream[i] += (int)(chunk->channels[j]->volume/2.f * s);
            }
        }
        ampl *= 0.9999;

        pos++;
        if(pos >= 44100)
        {
            ampl = 1.0;
            pos = 0;
            /* next note in chunk */
            chunk->pos++;
            if(chunk->pos >= chunk_len)
            {
                /* next chunk in song */
                chunk = chunk->next;
                if(!chunk)
                {
                    if(g_debug)
                        fprintf(stderr, "Song finished\n");
                    song->ended = 1;
                }
                else if(g_debug)
                    fprintf(stderr, "Moving to next chunk\n");
            }
        }
    }
}

typedef union {
    char command;
    struct {
        char command;
        char *param;
    } string;
    struct {
        char command;
        int param;
    } number;
} Command;

static int read_line(FILE *file, Command *ret)
{
    static char buffer[1024];
    const char *const token_delim = " \t\r\n";

    ret->command = '\0';

    if(!fgets(buffer, 1024, file))
    {
        if(g_debug)
            fprintf(stderr, "End of file reached\n");
        return -1;
    }

    /* reads the command (one character) */
    {
        char *com = strtok(buffer, token_delim);
        if(!com || com[0] == '\0' || com[0] == '#' || com[1] != '\0')
            /* Blank or commented line */
            return 0;
        ret->command = com[0];
    }

    /* reads the parameter */
    {
        char *param = strtok(NULL, token_delim);
        if(!param)
        {
            fprintf(stderr, "No parameter given to %c command\n",
                    ret->command);
            return -1;
        }
        switch(ret->command)
        {
        case 'l': /* label */
        case 'j': /* goto label */
        case 'n': /* notes */
            ret->string.param = malloc(sizeof(char) * (strlen(param) + 1));
            strcpy(ret->string.param, param);
            break;
        case 'p': /* start of chunk (number of channels) */
        case 'v': /* volume for channel */
        case 'i': /* instrument for channel */
            {
                char *endptr;
                ret->number.param = strtol(param, &endptr, 10);
                if(*endptr != '\0')
                {
                    fprintf(stderr, "Non-numeric parameter to %c command\n",
                            ret->command);
                    return -1;
                }
            }
            break;
        default:
            fprintf(stderr, "Unknown command %c\n", ret->command);
            return -1;
        }
    }

    return 0;
}

static Channel *new_channel(void)
{
    Channel *channel = malloc(sizeof(Channel));
    channel->instrument = 0;
    channel->volume = 100;
    channel->length = 0;
    channel->notes = NULL;
    return channel;
}

static Chunk *new_chunk(Song *song, size_t nb_channels)
{
    Chunk *chunk;
    song->nb_chunks++;
    if(song->chunks_l)
    {
        song->chunks_l->next = malloc(sizeof(Chunk));
        song->chunks_l = song->chunks_l->next;
    }
    else
        song->chunks_f = song->chunks_l = malloc(sizeof(Chunk));
    chunk = song->chunks_l;
    chunk->next = NULL;

    chunk->nb_channels = nb_channels;
    chunk->pos = 0;
    chunk->channels = malloc(sizeof(Channel*) * nb_channels);

    if(g_debug)
        fprintf(stderr, "Creating channel 0\n");
    chunk->channels[0] = new_channel();
    return chunk;
}

static float char2freq(char c)
{
    size_t index;
    if(c >= 'A' && c <= 'Z')
        index = c - 'A';
    else if(c >= 'a' && c <= 'z')
        index = c - 'a' + 26;
    else if(c >= '0' && c <= '9')
        index = c - '0' + 26 + 26;
    else
        return 0.f;

    // We don't want to include the nothing.
    size_t nb_notes = NOTES_RANGE - 1;

    enum note note = index % nb_notes;
    float freq = note_to_frequency(note);

    index /= nb_notes;
    while(index--)
        freq *= 2;
    return freq;
}

static Channel *find_channel(Song *song, Chunk **chunk)
{
    /* last chunk is finished */
    if(*chunk && (*chunk)->pos == (*chunk)->nb_channels)
    {
        (*chunk)->pos = 0;
        *chunk = NULL;
    }
    /* no current chunk ; auto-allocate one */
    if(!*chunk)
    {
        if(g_debug)
            fprintf(stderr, "Autocreating chunk (1 channel)\n");
        *chunk = new_chunk(song, 1);
    }
    return (*chunk)->channels[(*chunk)->pos];
}

static Song *read_song(FILE *file)
{
    Command command;
    Song *song = malloc(sizeof(Song));
    Chunk *chunk = NULL;
    song->ended = 0;
    song->nb_chunks = 0;
    song->chunks_f = song->chunks_l = NULL;
    if(g_debug)
        fprintf(stderr, "Starting parsing of song file...\n");
    while(read_line(file, &command) != -1)
    {
        switch(command.command)
        {
        case '\0':
            break;
        case 'p':
            {
                if(g_debug)
                    fprintf(stderr, "New chunk (%d channels)\n",
                            command.number.param);
                if(command.number.param < 1)
                    return NULL;
                if(chunk)
                    chunk->pos = 0;
                chunk = new_chunk(song, command.number.param);
            }
            break;
        case 'v':
            {
                /* select channel from chunk
                 * a new chunk may be allocated */
                Channel *channel = find_channel(song, &chunk);

                /* set the volume */
                channel->volume = command.number.param;
            }
            break;
        case 'i':
            {
                /* select channel from chunk
                 * a new chunk may be allocated */
                Channel *channel = find_channel(song, &chunk);

                /* set the instrument */
                channel->instrument = command.number.param;
            }
            break;
        case 'n':
            {
                size_t i;
                /* select channel from chunk
                 * a new chunk may be allocated */
                Channel *channel = find_channel(song, &chunk);

                /* fill it with notes! */
                channel->length = strlen(command.string.param);
                channel->notes = malloc(sizeof(float) * channel->length);
                for(i = 0; i < channel->length; i++)
                    channel->notes[i] = char2freq(command.string.param[i]);
                if(g_debug)
                    fprintf(stderr, "%u notes read into channel %u\n",
                        channel->length, chunk->pos);

                /* to next channel */
                chunk->pos++;
                if(chunk->pos < chunk->nb_channels)
                    chunk->channels[chunk->pos] = new_channel();
            }
            break;
        case 'l':
        case 'j':
            fprintf(stderr, "Warning: '%c' is currently unimplemented\n",
                command.command);
            break;
        }
    }
    if(chunk->pos != chunk->nb_channels)
    {
        fprintf(stderr, "Missing %d channels in chunk\n",
                chunk->nb_channels - chunk->pos);
        return NULL;
    }
    chunk->pos = 0;
    if(g_debug)
        fprintf(stderr, "Song file parsed\n");
    return song;
}

int play(FILE *file, int debug)
{
    Song *song = NULL;

    g_debug = debug;

    song = read_song(file);

    if(g_debug)
        fprintf(stderr, "Initializing SDL...\n");

    int sdl_open_res = init_sdl_stream(audio_callback, (void*)song);
    if(sdl_open_res < 0)
    {
        fprintf(stderr, "Unable to open audio context\n");
        return 1;
    }

    if(g_debug)
        fprintf(stderr, "Starting audio...\n");

    SDL_PauseAudio(0);

    while(!song->ended)
        SDL_Delay(500);

    if(g_debug)
        fprintf(stderr, "Stopping audio and exiting gracefully\n");

    end_sdl_stream();
    return 0;
}
