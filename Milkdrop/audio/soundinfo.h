#ifndef MILKDROP_AUDIO_SOUNDINFO_H
#define MILKDROP_AUDIO_SOUNDINFO_H



#define NUM_FREQUENCIES              512   // # of freq. samples you want *out* of the FFT, for 0-11kHz range.
                                           //   ** this must be a power of 2!
                                           //   ** the actual FFT will use twice this many frequencies **
typedef struct
{
    float imm[2][3];                        // bass, mids, treble, no damping, for each channel (long-term average is 1)
    float avg[2][3];                        // bass, mids, treble, some damping, for each channel (long-term average is 1)
    float med_avg[2][3];                    // bass, mids, treble, more damping, for each channel (long-term average is 1)
    float long_avg[2][3];                   // bass, mids, treble, heavy damping, for each channel (long-term average is 1)
    float infinite_avg[2][3];               // bass, mids, treble: winamp's average output levels. (1)
    float fWaveform[2][576];                // Not all 576 are valid! - only NUM_WAVEFORM_SAMPLES samples are valid for each channel (note: NUM_WAVEFORM_SAMPLES is declared in shell_defines.h)
    float fSpectrum[2][NUM_FREQUENCIES];    // NUM_FREQUENCIES samples for each channel (note: NUM_FREQUENCIES is declared in shell_defines.h)
} td_soundinfo;                             // ...range is 0 Hz to 22050 Hz, evenly spaced.





#endif 