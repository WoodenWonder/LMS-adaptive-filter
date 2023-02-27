/**
 *  @brief Signal generator source file
 *  @author shed258
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "signalGenerator.h"

#define TWO_PI (2*M_PI)

/**
 * @brief Generate sine wave based on given parameters
 * @param outputSamples     Buffer with generated samples
 * @param resolution        Samples resolution of generated sine wave
 */
static void signalGenerator_generateSine(double* outputSamples, int resolution)
{
    for (int i = 0; i < resolution; i++)
    {
        outputSamples[i] = sin((i*TWO_PI)/resolution);
    }
}

TypeOfGenSignal_t signalGenerator_processArgumentType(const char* type)
{
    int retval = GEN_SIGNAL_UNKNOWN;

    if (strncmp(type, "sine", (sizeof("--sine")-1)) == 0)
    {
        retval = GEN_SIGNAL_SINE;
    }
    else
    {
        printf("ERROR: Unknown argument for <type>\n");
    }
    return retval;
}

int signalGenerator_processArgumentPeriods(const char* periods)
{
    int retval = -1;
    char character;
    int argNumber;

    for (int i = 0; (periods[i] != '\0'); i++)
    {
        character = periods[i];
        if (!isdigit(character))
        {
            printf("ERROR: Argument <periods> contains non-digit characters\n");
            return retval;
        }
    }
    argNumber = atoi(periods);
    if (argNumber > 0)
    {
        retval = argNumber;
    }
    else
    {
        printf("ERROR: Number for <periods> must be greater than 0\n");
    }
    return retval;
}

int signalGenerator_processArgumentResolution(const char* resolution)
{
    int retval = -1;
    char character;
    int argNumber;

    for (int i = 0; (resolution[i] != '\0'); i++)
    {
        character = resolution[i];
        if (!isdigit(character))
        {
            printf("ERROR: Argument <resolution> contains non-digit characters\n");
            return retval;
        }
    }

    argNumber = atoi(resolution);

    if ((argNumber > 1) && (argNumber <= SIGNAL_GEN_MAX_RESOLUTION))
    {
        retval = argNumber;
    }
    else
    {
        printf("ERROR: Number for <resolution> must be in range: 2 - %d \n", SIGNAL_GEN_MAX_RESOLUTION);
    }
    return retval;
}

void signalGenerator_generateSignal(const SignalGenerator_t *settings, double* outputSamples)
{
    switch(settings->type)
    {
        case GEN_SIGNAL_SINE:
            if (outputSamples != NULL)
            {
                signalGenerator_generateSine(outputSamples, settings->resolution);
            }
            break;

        default:
            break;
    }
}