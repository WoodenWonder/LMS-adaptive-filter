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
    unsigned int cycles;
    unsigned int resolution;
} SignalGenerator_t;

/**
 * @brief Process argument <type>
 * @param type  string with argument to process
 * @return enumerated type of signal
 */
TypeOfGenSignal_t signalGenerator_processArgumentType(const char* type);

/**
 * @brief Process argument <cycles>
 * @param cycles  string with argument to process
 * @return the number of cycles of the generated signal
 */
unsigned int signalGenerator_processArgumentCycles(const char* cycles);

/**
 * @brief Process argument <resolution>
 * @param resolution  string with argument to process
 * @return the number ot samples in one cycle
 */
unsigned int signalGenerator_processArgumentResolution(const char* resolution);

/**
 * @brief Generate waveform based on given parameters
 * @param settings          Set of parameters of generated signal
 * @param outputFileName    The name of the file to save samples to
 * @return EXIT_SUCCESS when waveform generated and saved to the file
 */
int signalGenerator_generateSignal(const SignalGenerator_t *settings, const char* outputFileName);

#endif  /* SIGNAL_GENERATOR_H */