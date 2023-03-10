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
    float step;
    int length;
    float coefficients[MAX_FILTER_LENGTH];
} LmsFilter_t;

/**
 * @brief Initialize the filter structure with step size and filter length
 * @param filter    Structure holding LMS filter
 * @param step      Step size
 * @param length    Filter length
 * @return LMS_OK when filter initialised succesfully. Otherwise, return LMS_ERROR
 */
int lmsFilter_Init(LmsFilter_t* filter, float step, int length);

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
float lmsFilter_processArgumentStepSize(const char* stepSize);

/**
 * @brief Filtering function.
 * Applying the filter to the input signal and desired signal.
 * Saving processed samples to the file.
 * @param filter            Pointer to LMS filter structure
 * @param fileName          Name of the file containing input samples
 * @return LMS_OK when processed succesfully. Otherwise, return LMS_ERROR
 */
int lmsFilter_FilterSignalAndSaveToFile(LmsFilter_t* filter, const char* fileName);

#endif  /* LMS_FILTER_H */