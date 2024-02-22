#include "sampler.h"
#include "periodTimer.h"
#include "hal/A2D_Photoresistor.h"
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h> 

long long temp_max = 1000; 

pthread_t ptid; 
double weighting_value = 0.001;
double history[1000];  
int hist_pos = 0; 
int prev_pos = 0; 
long long num_samples_taken = 0; 
double ave_reading = 0;
double prev_ave_reading = 0;  


// Begin/end the background thread which samples light levels.
void Sampler_init(void){
    pthread_create(&ptid, NULL, &Sampler_moveCurrentDataToHistory, NULL ); 
}

void Sampler_cleanup(void){
    pthread_cancel(ptid); 
}

// Must be called once every 1s.
// Moves the samples that it has been collecting this second into
// the history, which makes the samples available for reads (below).
void* Sampler_moveCurrentDataToHistory(){

    // get sample and redo ave reading 
    // loop once every second
    int lightSample = getVoltage1Reading(); 
    history[hist_pos] = lightSample; 
    prev_ave_reading = lightSample; 
    struct timespec sleep; 
    sleep.tv_sec = 0.001; 
    sleep.tv_nsec = 1000000;
    num_samples_taken++; 
    hist_pos++; 
    for(int i = 0; i < 1000000; i++){
        lightSample = getVoltage1Reading(); 
        history[hist_pos] = lightSample; 
        ave_reading = weighting_value * lightSample + (1-weighting_value)*prev_ave_reading; 
        prev_ave_reading = ave_reading;  

        num_samples_taken++; 
        hist_pos++; 
        printf("light sample: %d\tavg: %f\tsamples: %lld\n", lightSample, ave_reading, num_samples_taken); 
        nanosleep(&sleep, &sleep); 
    }
    // use period timer to check when to redo averageReading 
    return NULL; 
}

// Get the number of samples collected during the previous complete second.
int Sampler_getHistorySize(void){
    return hist_pos - prev_pos; 
}

// Get a copy of the samples in the sample history.
// Returns a newly allocated array and sets `size` to be the
// number of elements in the returned array (output-only parameter).
// The calling code must call free() on the returned pointer.
// Note: It provides both data and size to ensure consistency.
double* Sampler_getHistory(int *size){
    if (*size > hist_pos + 1){ // corner case
        return NULL; 
    }

    double* copy = (double*)malloc((*size)*sizeof(double)); 
    for(int i = 0; i < hist_pos; i++){
        copy[i] = history[i]; 
    }

    return copy; 
}

// Get the average light level (not tied to the history).
double Sampler_getAverageReading(void){
    return ave_reading; 
}

// Get the total number of light level samples taken so far.
long long Sampler_getNumSamplesTaken(void){
    return num_samples_taken; 
}