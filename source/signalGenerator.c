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
 * @param settings          Parameters of signal
 * @param outputSamples     Buffer with generated samples
 * @return EXIT_SUCCESS when sine waveform generated and saved to the file
 */
static int signalGenerator_generateSine(const SignalGenerator_t *settings, const char* outputFileName)
{
    int retval = EXIT_SUCCESS;
    char outputSamplesAsCharacter[32];

    float* outputSamples = NULL;
    outputSamples = (float*)malloc(settings->resolution * sizeof(float));

    FILE *file = fopen(outputFileName, "w");
    if (!file)
    {
        perror(outputFileName);
        return EXIT_FAILURE;
    }
    printf("Output file:                  %s \n", outputFileName);

    for (unsigned int n = 0; n < settings->cycles; n++)
    {
        for (unsigned int i = 0; i < settings->resolution; i++)
        {
            if (n == 0) /* Generate one waveform in first iteration and then use it in next cycles */
            {
                outputSamples[i] = sin((i*TWO_PI)/settings->resolution);
            }
            snprintf(outputSamplesAsCharacter, sizeof(outputSamplesAsCharacter), "%f", outputSamples[i]);
            if (fputs(outputSamplesAsCharacter, file) != EOF)
            {
                if (fputs(";\n", file) == EOF)
                {
                    return EXIT_FAILURE;
                }
            }
            else
            {
                return EXIT_FAILURE;
            }
        }

        int progress = 0;
        if (((n % 10) == 0) || (n == settings->cycles - 1))
        {
            progress = ((float)n/settings->cycles)*100;
            if (n == settings->cycles - 1)
            {
                progress = 100;
            }
            printf("Waveform generation progress: %d%%\r", progress);
            fflush(stdout);
        }
    }
    printf("\n");

    free(outputSamples);

    if (fclose(file))
    {
        perror(outputFileName);
        return EXIT_FAILURE;
    }
    return retval;
}

TypeOfGenSignal_t signalGenerator_processArgumentType(const char* type)
{
    int retval = GEN_SIGNAL_UNKNOWN;

    if (strncmp(type, "sine", (sizeof("--sine")-1)) == 0)
    {
        retval = GEN_SIGNAL_SINE;
        printf("Signal type:                  sine\n");
    }
    else
    {
        printf("ERROR: Unknown argument for <type>\n");
    }
    return retval;
}

unsigned int signalGenerator_processArgumentCycles(const char* cycles)
{
    unsigned int retval = 0;
    char character;
    unsigned int argNumber;

    for (int i = 0; (cycles[i] != '\0'); i++)
    {
        character = cycles[i];
        if (!isdigit(character))
        {
            printf("ERROR: Argument <cycles> contains non-positive integer characters\n");
            return retval;
        }
    }
    argNumber = atoi(cycles);
    if (argNumber > 0)
    {
        retval = argNumber;
        printf("Number of cycles:             %d \n", retval);
    }
    else
    {
        printf("ERROR: Number for <cycles> must be greater than 0\n");
    }
    return retval;
}

unsigned int signalGenerator_processArgumentResolution(const char* resolution)
{
    unsigned int retval = 0;
    char character;
    unsigned int argNumber;

    for (int i = 0; (resolution[i] != '\0'); i++)
    {
        character = resolution[i];
        if (!isdigit(character))
        {
            printf("ERROR: Argument <resolution> contains non-positive integer characters\n");
            return retval;
        }
    }
    argNumber = atoi(resolution);
    if ((argNumber > 1) && (argNumber <= SIGNAL_GEN_MAX_RESOLUTION))
    {
        retval = argNumber;
        printf("Signal resolution:            %d \n", retval);
    }
    else
    {
        printf("ERROR: Number for <resolution> must be in range: 2 - %d \n", SIGNAL_GEN_MAX_RESOLUTION);
    }
    return retval;
}

int signalGenerator_generateSignal(const SignalGenerator_t *settings, const char* outputFileName)
{
    int retval = EXIT_SUCCESS;

    switch(settings->type)
    {
        case GEN_SIGNAL_SINE:
            if (outputFileName != NULL)
            {
                if (signalGenerator_generateSine(settings, outputFileName) != EXIT_SUCCESS)
                {
                    retval = EXIT_FAILURE;
                }
            }
            break;

        default:
            retval = EXIT_FAILURE;
            break;
    }
    return retval;
}