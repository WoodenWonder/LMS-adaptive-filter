/**
 *  @brief Lms filter header
 *  @author shed258
 */

#ifndef LMS_FILTER_H
#define LMS_FILTER_H

#define MAX_FILTER_LENGTH 1000

#define LMS_OK      0
#define LMS_ERROR   1

typedef struct
{
    double step;
    int length;
    double coefficients[MAX_FILTER_LENGTH];
} LmsFilter_t;

/**
 * @brief Initialize the filter structure with step size and filter length
 * @param filter    Structure holding LMS filter
 * @param ftep      Step size
 * @param length    Filter length
 * @return LMS_OK when filter initialised succesfully. Otherwise, return LMS_ERROR
 */
int lmsFilter_Init(LmsFilter_t* filter, double step, int length);

/**
 * @brief Process argument <length>
 * @param filterLength  string with argument to process
 * @return Filter length
 */
unsigned int lmsFilter_processArgumentFilterLength(const char* filterLength);

/**
 * @brief Process argument <stepsize>
 * @param stepSize  string with argument to process
 * @return Step size
 */
double lmsFilter_processArgumentStepSize(const char* stepSize);

/**
 * @brief LMS filtering function. Applying the filter to the input signal and desired signal
 * @param filter            Pointer to LMS filter structure
 * @param inputSignal       Array of input signal samples, e.g. noise corrupted signal
 * @param desiredSignal     Array of additional input signal
 * @param outputSignal      Array of output signal
 * @param numSamples        Filter length/length
 */
void lmsFilter_FilterSignal(LmsFilter_t* filter, const double* inputSignal, const double* desiredSignal,
                            double* outputSignal, int numSamples);

#endif  /* LMS_FILTER_H */