/**
 *  @brief Lms filter header
 *  @author shed258
 */

#ifndef LMS_FILTER_H
#define LMS_FILTER_H

#define MAX_ORDER 100

#define LMS_OK      0
#define LMS_ERROR   1

typedef struct
{
    double step;
    int order;
    double coefficients[MAX_ORDER];
} LmsFilter_t;

/**
 * @brief Initialize the filter structure with step size and filter order
 * @param filter    Structure holding LMS filter
 * @param ftep      Step size
 * @param order     Filter order
 * @return LMS_OK when filter initialised succesfully. Otherwise, return LMS_ERROR
 */
int lmsFilter_Init(LmsFilter_t* filter, double step, int order);

/**
 * @brief LMS filtering function. Applying the filter to the input signal and desired signal
 * @param filter            Pointer to LMS filter structure
 * @param inputSignal       Array of input signal samples, e.g. noise corrupted signal
 * @param desiredSignal     Array of additional input signal
 * @param outputSignal      Array of output signal
 * @param numSamples        Filter size/order
 */
void lmsFilter_FilterSignal(LmsFilter_t* filter, const double* inputSignal, const double* desiredSignal,
                            double* outputSignal, int numSamples);

#endif  /* LMS_FILTER_H */