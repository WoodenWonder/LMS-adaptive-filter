/**
 *  @brief Signal generator header
 *  @author shed258

 */

#ifndef SIGNAL_GENERATOR_H
#define SIGNAL_GENERATOR_H

#define SIGNAL_GEN_MAX_RESOLUTION   100000

typedef enum
{
    GEN_SIGNAL_UNKNOWN = 0,
    GEN_SIGNAL_SINE,
} TypeOfGenSignal_t;

typedef struct
{
    TypeOfGenSignal_t type;
    int periods;
    int resolution;
} SignalGenerator_t;

/**
 * @brief Process argument <type>
 * @param type  string with argument to process
 * @return enumerated type of signal
 */
TypeOfGenSignal_t signalGenerator_processArgumentType(const char* type);

/**
 * @brief Process argument <periods>
 * @param periods  string with argument to process
 * @return the number of periods of the generated signal
 */
int signalGenerator_processArgumentPeriods(const char* periods);

/**
 * @brief Process argument <resolution>
 * @param resolution  string with argument to process
 * @return the number ot samples in one period
 */
int signalGenerator_processArgumentResolution(const char* resolution);

/**
 * @brief Generate waveform based on given parameters
 * @param settings          Set of parameters of generated signal
 * @param outputSamples     Buffer with generated samples
 */
void signalGenerator_generateSignal(const SignalGenerator_t *settings, double* outputSamples);

#endif  /* SIGNAL_GENERATOR_H */