#ifndef LMS_H
#define LMS_H

#define MAX_ORDER 100

typedef struct
{
    double step;
    int order;
    double weights[MAX_ORDER];
} LMSFilter;

void init_lms_filter(LMSFilter* filter, double step, int order);
void filter(LMSFilter* filter, double* input_signal, double* desired_signal, double* output_signal, int num_samples);

#endif