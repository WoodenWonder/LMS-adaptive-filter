#include <stddef.h>
#include "lms.h"

int lmsInitFilter(LmsFilter_t* filter, double step, int order)
{
    int retval = LMS_ERROR;

    if (filter != NULL)
    {
        if (order <= MAX_ORDER)
        {
            filter->step = step;
            filter->order = order;

            for (int i = 0; i < order; i++)
            {
                filter->coefficients[i] = 0.0;
            }
            retval = LMS_OK;
        }
        else
        {
            printf("Filter order exceeds MAX_ORDER %d\n", MAX_ORDER);
        }
    }
    return retval;
}

void lmsFilterSignal(LmsFilter_t* filter, const double* inputSignal, const double* desiredSignal, double* outputSignal, int numSamples)
{
    double y = 0.0;                 /* fitler output */
    double e = 0.0;                 /* error signal */
    double x[MAX_ORDER] = { 0 };    /* fitler input */

    for (int n = 0; n < numSamples; n++)
    {
        int k;
        for (k = 0; k < filter->order; k++)
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

        for (k = 0; k < filter->order; k++)
        {
            filter->coefficients[k] += filter->step * e * x[k];
        }

        outputSignal[n] = y;
    }
}