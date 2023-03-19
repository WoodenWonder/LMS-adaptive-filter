/**
 *  @brief Lms filter source file
 *  @author shed258
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <ctype.h>
#include <math.h>
#include "lmsFilter.h"

/**
 * @brief Function opens input file and calculate number of samples
 * @param fileName  Name of the file with input samples
 * @return Number of samples in file. If failed return -1
 */
static int lmsFilter_countNumberOfSamplesInFile(const char* fileName)
{
    FILE* fp = fopen(fileName, "r");
    if (fp == NULL)
    {
        perror(fileName);
        return -1;
    }
    int count = 0;
    int ch;
    while ((ch = fgetc(fp)) != EOF)
    {
        if (ch == '\n')
        {
            count++;
        }
    }
    if (fclose(fp))
    {
        perror(fileName);
        return -1;
    }
    printf("Input file samples:           %d\n", count);

    return count;
}

/**
 * @brief LMS filtering function. Applying the filter to the input signal and desired signal
 * This implementation is a type of acoustic silencer. The input and desired signals are equal
 * @param filter        Pointer to LMS filter structure
 * @param input         Array of input samples, e.g. noise corrupted signal
 * @param desired       Array of additional input
 * @param output        Array of output
 * @param error         Mean square error
 * @return EXIT_SUCCESS when processed succesfully. Otherwise, return EXIT_FAILURE
 */
static int lmsFilter_Lms(LmsFilter_t* filter, const float* input, const float* desired,
                         float* output, float* error)
{
    (void)desired;

    int retval = EXIT_SUCCESS;
    float y = 0.0;                         /* fitler output */
    float x[MAX_FILTER_LENGTH] = { 0 };    /* fitler input */
    int k;

    for (k = 0; k < filter->length; k++)
    {
        x[k] = input[k];
        y += filter->coefficients[k] * x[k];

        /* The input and desired signals are equal */
        *error = input[0] - y;

        /* By default in LMS algotithm it should go as in line below */
        // *error = desired[n] - y;

        for (k = 0; k < filter->length; k++)
        {
            filter->coefficients[k] += filter->step * (*error) * x[k];
        }
        *output = y;
    }

    if (isfinite(*output) == 0)
    {
        printf("WARNING: Algorithm goes unstable! stopped\n");
        retval = EXIT_FAILURE;
    }
    return retval;
}

int lmsFilter_Init(LmsFilter_t* filter, float step, int length)
{
    int retval = EXIT_FAILURE;

    if (filter != NULL)
    {
        if (length <= MAX_FILTER_LENGTH)
        {
            filter->step = step;
            filter->length = length;

            for (int i = 0; i < length; i++)
            {
                filter->coefficients[i] = 0.0;
            }
            retval = EXIT_SUCCESS;
        }
        else
        {
            printf("Filter length exceeds MAX_FILTER_LENGTH %d\n", MAX_FILTER_LENGTH);
        }
    }
    return retval;
}

unsigned int lmsFilter_processArgumentFilterLength(const char* filterLength)
{
    unsigned int retval = 0;
    char character;
    unsigned int argNumber;

    for (int i = 0; (filterLength[i] != '\0'); i++)
    {
        character = filterLength[i];
        if (!isdigit(character))
        {
            printf("ERROR: Argument <length> wrong format\n");
            return retval;
        }
    }
    argNumber = atoi(filterLength);
    if (argNumber > 0)
    {
        retval = argNumber;
    }
    else
    {
        printf("ERROR: Number for <length> must be greater than 0\n");
    }
    return retval;
}

float lmsFilter_processArgumentStepSize(const char* stepSize)
{
    float retval = 0;
    char character;
    float argNumber;
    static unsigned int dotCounter = 0;

    for (int i = 0; (stepSize[i] != '\0'); i++)
    {
        character = stepSize[i];
        if (isdigit(character))
        {
            continue;
        }
        else if (stepSize[i] == '.')
        {
            if((++dotCounter > 1) || (i == 0))
            {
                printf("ERROR: Argument <stepsize> wrong format\n");
                return retval;
            }
            else
            {
                continue;
            }
        }
        else
        {
            printf("ERROR: Argument <stepsize> contains non-digit characters\n");
            return retval;
        }
    }
    argNumber = atof(stepSize);
    if (argNumber > 0)
    {
        retval = argNumber;
    }
    else
    {
        printf("ERROR: Number for <stepsize> must be greater than 0\n");
    }
    return retval;
}

int lmsFilter_FilterSignalAndSaveToFile(LmsFilter_t* filter, const char* inputFileName)
{
    int retval = EXIT_SUCCESS;

    FILE* fSamples = fopen(inputFileName, "r");
    if (fSamples == NULL)
    {
        perror(inputFileName);
        return EXIT_FAILURE;
    }

    int numOfSamples = lmsFilter_countNumberOfSamplesInFile(inputFileName);
    if (filter->length > numOfSamples)
    {
        printf("Error: Filter length cannot be greater than number of samples in file\n");
        fclose(fSamples);
        return EXIT_FAILURE;
    }

    char* filteredFileName = NULL;
    const char* filteredFileSuffix = "filtered";
    filteredFileName = (char*)malloc(strlen(inputFileName) + strlen(filteredFileSuffix) + 1);
    strcpy(filteredFileName, inputFileName);
    strcat(filteredFileName, filteredFileSuffix);

    FILE *fFiltered = fopen(filteredFileName, "w");
    if (!fFiltered)
    {
        perror(filteredFileName);
        return EXIT_FAILURE;
    }

    rewind(fSamples);  /* Reset file position indicator to beginning of the file */
    float window[filter->length];
    int index = 0;
    int zerosToAdd = filter->length - 1;
    float output = 0;
    float errror = 0;
    int progress = 0;

    /* Slide window through input file and print each subsequence */
    while (index < numOfSamples)
    {
        if (index == 0)
        {
            /* Fill the first window with initial values */
            for (int i = 0; i < filter->length; i++)
            {
                if (fscanf(fSamples, "%f;", &window[i]) != 1)
                {
                    printf("Error reading file %s\n", inputFileName);
                    fclose(fSamples);
                    return EXIT_FAILURE;
                }
            }
        }
        else
        {
            for (int i = 0; i < filter->length - 1; i++)    /* Shift window */
            {
                window[i] = window[i+1];
            }

            /* Fill last element of window with next value from file or zero if end of file is reached */
            if (fscanf(fSamples, "%f;", &window[filter->length-1]) != 1)
            {
                window[filter->length-1] = 0;
                if (--zerosToAdd < 0)
                {
                    break;
                }
            }
        }

        retval = lmsFilter_Lms(filter, window, NULL, &output, &errror);
        if (retval != EXIT_SUCCESS)
        {
            break;
        }
        fprintf(fFiltered, "%d;%.6lf;%.6lf;%.6lf;\n",index, window[0], output, errror);
        index++;

        if (((index % 10) == 0) || (index == numOfSamples))
        {
            progress = ((float)index/numOfSamples)*100;
            if (index == numOfSamples - 1)
            {
                progress = 100;
            }
            printf("LMS filtering progress:       %d%%\r", progress);
            progress = 0;
            fflush(stdout);
        }
    }

    fflush(stdout);
    printf("\n");

    if (fclose(fSamples))
    {
        perror(inputFileName);
        return EXIT_FAILURE;
    }
    if (fclose(fFiltered))
    {
        perror(filteredFileName);
        return EXIT_FAILURE;
    }

    free(filteredFileName);

    return retval;
}
