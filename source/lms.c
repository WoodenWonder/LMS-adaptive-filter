#include "lms.h"

/* Initialize the filter with step size and filter order */
void init_lms_filter(LMSFilter* filter, double step, int order)
{
	int i;

    filter->step = step;
    filter->order = order;

    for (i = 0; i < order; i++)
    {
        filter->weights[i] = 0.0;
    }
}

/* Apply the filter to the input signal and desired signal */
void filter(LMSFilter* filter, double* input_signal, double* desired_signal, double* output_signal, int num_samples)
{
    int n;
    for (n = 0; n < num_samples; n++)
    {
        int k;
        double x[MAX_ORDER];
        for (k = 0; k < filter->order; k++)
        {
            int index = n + k;
            if (index < num_samples)
            {
                x[k] = input_signal[index];
            }
            else
            {
                x[k] = 0.0;
            }
        }

        double y = 0.0;
        for (k = 0; k < filter->order; k++)
        {
            y += filter->weights[k] * x[k];
        }
        double e = desired_signal[n] - y;

        for (k = 0; k < filter->order; k++)
        {
            filter->weights[k] += filter->step * e * x[k];
        }

        output_signal[n] = y;
    }
}