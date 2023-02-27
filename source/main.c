/**
 *  @brief Main source file
 *  @author shed258
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lmsFilter.h"
#include "signalGenerator.h"

#define MAX_ARGC_NUMBER 6
#define ARGC_NUMBER_FOR_GENERATE_MODE 6

const char *tmpFileName = "tmp";

typedef enum
{
    ARG_TYPE = 2,
    ARG_RESOLUTION,
    ARG_PERIODS,
    ARG_FILE
} ArgGenerate_t;

/**
 * @brief Set of usage information
 */
static const char *const usage[] =
{
    "Options:\n",
    "  --help                                               Display this information.\n",
    "  --version                                            Display version information.\n",
    "  --generate <type> <resolution> <periods> <file>      Generate samples for the selected waveform\n",
    "                                                       and number of periods and save them to a file\n",
    NULL
};

static const char illegalFilenameChars[] =
{
    '#', '%', '{', '}', '\\', '/', '*', '?', '$', '!', ':', '@', '+', '=', '^', '~', ','
};

/**
 * @brief Print usage information
 * @param target    Program name
 */
static void showUsage(const char* target)
{
    int index = 0;

    printf("Usage: %s [options] file...\n", target);

    while (usage[index] != NULL)
    {
        printf("%s", usage[index++]);
    }
}

/**
 * @brief Print message in case of argument error
 * @param target    Program name
 */
static void printArgumentError(const char* target)
{
    printf("\nERROR: wrong argument\n\n");
    showUsage(target);
}

/**
 * @brief Print message in case of missing parameter
 * @param target    Program name
 */
static void printMissingParameterError(const char* target)
{
    printf("\nERROR: Missing parameter\n\n");
    showUsage(target);
}

/**
 * @brief Process argument <file>
 * @param filename  Argument with the name of the file to save the samples
 * @return EXIT_SUCCESS when filename is allowed
 */
static int processArgumentFile(const char* filename)
{
    int retval = EXIT_SUCCESS;

    for (int i = 0; (filename[i] != '\0'); i++)
    {
        for (unsigned int j = 0; j < sizeof(illegalFilenameChars); j++)
        {
            if (filename[i] == illegalFilenameChars[j])
            {
                retval = EXIT_FAILURE;
                printf("ERROR: Argument <file> can not contain following characters: space ");
                for (unsigned int n = 0; n < sizeof(illegalFilenameChars); n++)
                {
                    printf("%c ", illegalFilenameChars[n]);
                }
                printf("\n");
                break;
            }
        }
    }
    return retval;
}

/**
 * @brief Process parameters for signal generation
 * @param arg       Program argument
 * @param index     Argument index
 * @param settings  Set of parameters of generated signal
 * @return EXIT_SUCCESS when all parameters correct
 */
static int processArgsToGenerateWaveform(char* arg, ArgGenerate_t index, SignalGenerator_t* settings)
{
    int retval = EXIT_SUCCESS;

    switch(index)
    {
        case ARG_TYPE:
            settings->type = signalGenerator_processArgumentType(arg);
            if (settings->type == GEN_SIGNAL_UNKNOWN)
            {
                retval = EXIT_FAILURE;
            }
            break;

        case ARG_PERIODS:
            settings->periods = signalGenerator_processArgumentPeriods(arg);
            if (settings->periods < 1)
            {
                retval = EXIT_FAILURE;
            }
            break;

        case ARG_RESOLUTION:
            settings->resolution = signalGenerator_processArgumentResolution(arg);
            if (settings->resolution < 1)
            {
                retval = EXIT_FAILURE;
            }
            break;

        case ARG_FILE:
            if (processArgumentFile(arg) != EXIT_SUCCESS)
            {
                retval = EXIT_FAILURE;
            }
            break;

        default:
            retval = EXIT_FAILURE;
            break;
    }
    return retval;
}

/**
 * @brief Save generated samples to a file
 * @param settings      Set of parameters of generated signal
 * @param outputSamples Buffer with generated samples
 * @param fileName      The name of the file to save samples to

 * @return EXIT_SUCCESS when all parameters correct
 */
static int saveGeneratedSamplesToFile(const SignalGenerator_t *settings, double* outputSamples, const char* fileName)
{
    int retval = EXIT_SUCCESS;

    FILE *tmp = fopen(tmpFileName, "w+");
    if (!tmp)
    {
        perror(tmpFileName);
        return EXIT_FAILURE;
    }

    FILE *file = fopen(fileName, "w");
    if (!file)
    {
        perror(fileName);
        return EXIT_FAILURE;
    }

    for (int m = 0; m < settings->resolution; m++)
    {
        char outputSamplesAsCharacter[32];
        snprintf(outputSamplesAsCharacter, sizeof(outputSamplesAsCharacter), "%f", outputSamples[m]);
        if (fputs(outputSamplesAsCharacter, tmp) != EOF)
        {
            if (fputc('\n', tmp) == EOF)
            {
                return EXIT_FAILURE;
            }
        }
        else
        {
            return EXIT_FAILURE;
        }
    }

    long tmpSize = ftell(tmp);
    char *tmpBuffer;
    tmpBuffer = (char*)malloc(tmpSize);

    fseek(tmp, 0, SEEK_SET);

    unsigned long nread;
    nread = fread(tmpBuffer, 1, tmpSize, tmp);

    for (int i = 0; i < settings->periods; i++)
    {
        fwrite(tmpBuffer, 1, nread, file);
    }

    free(tmpBuffer);

    if (fclose(tmp))
    {
        perror(tmpFileName);
        return EXIT_FAILURE;
    }
    if (fclose(file))
    {
        perror(fileName);
        return EXIT_FAILURE;
    }
    if (remove(tmpFileName) != 0)
    {
        printf("Error: could not remove file %s \n", tmpFileName);
        return EXIT_FAILURE;
    }
    return retval;
}

int main(int argc, char **argv)
{
	int retval = EXIT_SUCCESS;

    if ((argc > 1) && (argc <= MAX_ARGC_NUMBER))
    {
        if (strncmp(argv[1], "--help", (sizeof("--help")-1)) == 0)
        {
            showUsage(argv[0]);
        }
        else if (strncmp(argv[1], "--version", (sizeof("--version")-1)) == 0)
        {
            printf("Version: %d.%d.%d\n", MAJOR_VERSION, MINOR_VERSION, PATCH_VERSION);
        }
        else if (strncmp(argv[1], "--generate", (sizeof("--generate")-1)) == 0)
        {
            if (argc == ARGC_NUMBER_FOR_GENERATE_MODE)
            {
                SignalGenerator_t signalSettings = { .type = GEN_SIGNAL_UNKNOWN };

                for (int i = 2; i < argc; i++)
                {
                    if (processArgsToGenerateWaveform(argv[i], i, &signalSettings) != EXIT_SUCCESS)
                    {
                        return EXIT_FAILURE;
                    }
                }
                double *outputSamples;
                outputSamples = (double*)malloc(signalSettings.periods * signalSettings.resolution * sizeof(double));
                signalGenerator_generateSignal(&signalSettings, outputSamples);
                retval = saveGeneratedSamplesToFile(&signalSettings, outputSamples, argv[ARG_FILE]);
                if (retval != EXIT_SUCCESS)
                {
                    return EXIT_FAILURE;
                }
                free(outputSamples);
            }
            else
            {
                printMissingParameterError(argv[0]);
                return EXIT_FAILURE;
            }
        }
        else
        {
            printArgumentError(argv[0]);
            return EXIT_FAILURE;
        }
    }
    else
    {
        showUsage(argv[0]);
        return EXIT_FAILURE;
    }
	return retval;
}
