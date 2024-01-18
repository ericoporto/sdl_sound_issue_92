#include <stdio.h>
#include <SDL.h>
#include <SDL_sound.h>

const size_t SampleDefaultBufferSize = 64 * 1024;

int main(int argc, char *argv[])
{
    int i = 0; /* used for debug later */
    Sound_Init();

    Sound_Sample * sample = Sound_NewSampleFromFile("VacuumNoise.ogg", NULL, SampleDefaultBufferSize);
    if (!sample)
        return -1;

    FILE *f = fopen("test.raw", "wb");
    if (!f)
    {
        Sound_FreeSample(sample);
        return -2;
    }

    do
    {
        if(i++ == 5)
            printf("break-debug-here"); /* next step will be the last decode loop */

        size_t sz = Sound_Decode(sample);
        if (sz > 0)
        {
            fwrite(sample->buffer, 1, sz, f);
        }
    } while ((sample->flags & SOUND_SAMPLEFLAG_EOF) == 0);

    fclose(f);
    Sound_FreeSample(sample);

    return 0;
}
