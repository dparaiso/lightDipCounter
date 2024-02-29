// terminalOutput.h
// Module to display data sampled by sampler.c in the background (uses a thread).

// Displays the following:
// Line 1:
// - light samples taken during the previous second
// - Raw value from the POT, and how many hertz (Hz) this is for the PWM 
// - The averaged light level displayed as a voltage with 3 decimal places.
// - The number of light level dips from the previous second
// - Timing jitter information for samples collected during the previous second
// - Minimum time between light samples.
// - Maximum time between light samples.
// - Average time between light samples.
// - Number of times sampled
// Line 2:
// Display 20 sample from the previous second

#ifndef _TERM_OUTPUT_H_
#define _TERM_OUTPUT_H_

// Begin/end the thread which displays data from the sample.
void Terminal_init();
void Terminal_cleanup();

// Starts displaying data received from the sample
void* Terminal_startDisplay();

#endif