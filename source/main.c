/**
 *  @brief Main source file
 *  @author shed258
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lmsFilter.h"
#include "signalGenerator.h"

#define MAX_ARGC_NUMBER                 6
#define ARGC_NUMBER_FOR_GENERATE_MODE   6
#define ARGC_NUMBER_FOR_FILTER_MODE     5

typedef enum
{
    GENERATE_ARG_TYPE = 2,
    GENERATE_ARG_RESOLUTION,
    GENERATE_ARG_PERIODS,
    GENERATE_ARG_FILE
} ArgGenerate_t;

typedef enum
{
    FILTER_ARG_LENGTH = 2,
    FILTER_ARG_STEP_SIZE,
    FILTER_ARG_FILE
} ArgFilter_t;

/**
 * @brief Set of usage information
 */
static const char *const usage[] =
{
    "Options:\n",
    "  --help                                               Display this information.\n",
    "  --version                                            Display version information.\n",
    "  --generate <type> <resolution> <periods> <file>      Generate samples for the selected waveform and number of periods and save them to a file\n",
    "  --filter <length> <stepsize> <file>                  Filter the signal in the form of samples read from the file. The parameters of the LMS filter are filter length(order) and step size\n",
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
 * @brief Process argument <file> in --generate mode
 * @param filename  Argument with the name of the file to save the samples
 * @return EXIT_SUCCESS when filename is allowed
 */
static int processGenerateArgumentFile(const char* filename)
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
 * @brief Process argument <file> in --filter mode
 * @param filename  Argument with the name of the file to read the samples
 * @return EXIT_SUCCESS when filename does not exist or is empty
 */
static int verifyFilterArgumentFile(const char* filename)
{
    int retval = EXIT_SUCCESS;
    FILE *file;
    long fileSize;

    file = fopen(filename, "r");

    if (file != NULL)
    {
        fseek(file, 0L, SEEK_END);
        fileSize = ftell(file);
        if (fileSize == 0)
        {
            printf("File is empty\n");
            retval = EXIT_FAILURE;
        }
        fclose(file);
    }
    else
    {
        printf("File does not exist\n");
        retval = EXIT_FAILURE;
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
        case GENERATE_ARG_TYPE:
            settings->type = signalGenerator_processArgumentType(arg);
            if (settings->type == GEN_SIGNAL_UNKNOWN)
            {
                retval = EXIT_FAILURE;
            }
            break;

        case GENERATE_ARG_PERIODS:
            settings->periods = signalGenerator_processArgumentPeriods(arg);
            if (settings->periods < 1)
            {
                retval = EXIT_FAILURE;
            }
            break;

        case GENERATE_ARG_RESOLUTION:
            settings->resolution = signalGenerator_processArgumentResolution(arg);
            if (settings->resolution < 1)
            {
                retval = EXIT_FAILURE;
            }
            break;

        case GENERATE_ARG_FILE:
            if (processGenerateArgumentFile(arg) != EXIT_SUCCESS)
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
 * @brief Process parameters for LMS adaptive filtering
 * @param arg       Program argument
 * @param index     Argument index
 * @param filter    Pointer to the structure holding LMS filter parameters
 * @return EXIT_SUCCESS when all parameters correct
 */
static int processArgsToStartFiltering(char* arg, ArgFilter_t index, LmsFilter_t* filter)
{
    int retval = EXIT_SUCCESS;

    switch(index)
    {
        case FILTER_ARG_LENGTH:
            filter->length = lmsFilter_processArgumentFilterLength(arg);
            printf("filter->length %d \n", filter->length);
            if (filter->length < 1)
            {
                retval = EXIT_FAILURE;
            }
            break;

        case FILTER_ARG_STEP_SIZE:
            filter->step = lmsFilter_processArgumentStepSize(arg);
            printf("filter->step %f \n", filter->step);
            if (!(filter->step > 0))
            {
                retval = EXIT_FAILURE;
            }
            break;

        case FILTER_ARG_FILE:
            if (verifyFilterArgumentFile(arg) != EXIT_SUCCESS)
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
    const char *tmpFileName = "tmp";

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

    for (unsigned int m = 0; m < settings->resolution; m++)
    {
        char outputSamplesAsCharacter[32];
        snprintf(outputSamplesAsCharacter, sizeof(outputSamplesAsCharacter), "%f", outputSamples[m]);
        if (fputs(outputSamplesAsCharacter, tmp) != EOF)
        {
            if (fputs(";\n", tmp) == EOF)
            {
                return EXIT_FAILURE;
            }
        }
        else
        {
            return EXIT_FAILURE;
        }
    }

    char tmpBuffer[8192];
    unsigned long nread;

    for (unsigned int i = 0; i < settings->periods; i++)
    {
        fseek(tmp, 0, SEEK_SET);
        while ((nread = fread(tmpBuffer, 1, sizeof(tmpBuffer), tmp)) > 0)
        {
            fwrite(tmpBuffer, 1, nread, file);
        }
    }

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
                double *outputSamples = NULL;
                outputSamples = (double*)malloc(signalSettings.periods * signalSettings.resolution * sizeof(double));
                signalGenerator_generateSignal(&signalSettings, outputSamples);
                retval = saveGeneratedSamplesToFile(&signalSettings, outputSamples, argv[GENERATE_ARG_FILE]);
                free(outputSamples);
            }
            else
            {
                printMissingParameterError(argv[0]);
                return EXIT_FAILURE;
            }
        }
        else if (strncmp(argv[1], "--filter", (sizeof("--filter")-1)) == 0)
        {
            if (argc == ARGC_NUMBER_FOR_FILTER_MODE)
            {
                LmsFilter_t filter;

                for (int i = 2; i < argc; i++)
                {
                    if (processArgsToStartFiltering(argv[i], i, &filter) != EXIT_SUCCESS)
                    {
                        return EXIT_FAILURE;
                    }
                }
                printf("ALL OK\n");
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
