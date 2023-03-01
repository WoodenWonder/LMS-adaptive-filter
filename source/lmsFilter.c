/**
 *  @brief Lms filter source file
 *  @author shed258
 */

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <stddef.h>
#include "lmsFilter.h"

int lmsFilter_Init(LmsFilter_t* filter, double step, int length)
{
    int retval = LMS_ERROR;

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
            retval = LMS_OK;
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

double lmsFilter_processArgumentStepSize(const char* stepSize)
{
    unsigned int retval = 0;
    char character;
    unsigned int argNumber;
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

void lmsFilter_FilterSignal(LmsFilter_t* filter, const double* inputSignal, const double* desiredSignal, double* outputSignal, int numSamples)
{
    double y = 0.0;                         /* fitler output */
    double e = 0.0;                         /* error signal */
    double x[MAX_FILTER_LENGTH] = { 0 };    /* fitler input */

    for (int n = 0; n < numSamples; n++)
    {
        int k;
        for (k = 0; k < filter->length; k++)
        {
            int index = n + k;
            if (index < numSamples)
            {
                x[k] = inputSignal[index];
            }
            else
            {
                x[k] = 0.0;
            }
            y += filter->coefficients[k] * x[k];
        }

        e = desiredSignal[n] - y;

        for (k = 0; k < filter->length; k++)
        {
            filter->coefficients[k] += filter->step * e * x[k];
        }

        outputSignal[n] = y;
    }
}