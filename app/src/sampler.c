#include "sampler.h"
#include "hal/A2D.h"
#include "hal/led.h"
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h> 
#include <stdbool.h> 
#include "periodTimer.h"

static pthread_t ptid; 
static double weighting_value = 0.001;
double history[MAX_SAMPLES];  
static long long num_samples_second = 0; 
static long long num_samples_taken = 0; 
static double avg_reading = 0;
_Atomic long long num_dips = 0; 
static pthread_mutex_t historyLock = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t dipLock = PTHREAD_MUTEX_INITIALIZER;
static bool dipState = false; 

// Begin/end the background thread which samples light levels.
void Sampler_init(void){
    if(pthread_create(&ptid, NULL, &Sampler_readSamples, NULL ) < 0){
        perror("pthread_create() error in Sampler"); 
        exit(1); 
    } 
}

void Sampler_cleanup(void){
    pthread_cancel(ptid); 
    pthread_join(ptid, NULL);
}

void* Sampler_readSamples(){

    while(1){
        Sampler_moveCurrentDataToHistory();
        countDips(); 
    }
    return NULL; 
}

void Sampler_exponentialAvgCalculator(int newSample){
    if(num_samples_taken == 0){
        avg_reading = (double)newSample; 
    }else{
        double prev = avg_reading; 
        avg_reading = weighting_value * newSample + (1-weighting_value)*prev; 
    }
}

// Must be called once every 1s.
// Moves the samples that it has been collecting this second into
// the history, which makes the samples available for reads (below).
void Sampler_moveCurrentDataToHistory(){

    int pos = 0; 
    int lightSample = 0; 
    double buffer[MAX_SAMPLES]; 
    long long secondAhead = getTimeInMs() + 1000; 

    while(getTimeInMs() < secondAhead){ 

        lightSample = getVoltage1Reading();
        Period_markEvent(PERIOD_EVENT_SAMPLE_LIGHT);

        Sampler_exponentialAvgCalculator(lightSample);
 
        buffer[pos] = lightSample; 
        // printf("light sample: %d\tavg: %f\tsamples: %lld\tpos: %d\n", lightSample, avg_reading, num_samples_taken, pos); 
        // printf("current pos: %d\n", pos); 
        pos++; 
        num_samples_taken++;
        sleepForMs(1); 
        
    }

    pthread_mutex_lock(&historyLock); 
    for(int i = 0; i < pos; i++){
        history[i] = buffer[i]; 
    }
    num_samples_second = pos; 

    pthread_mutex_unlock(&historyLock);
}

// Get the number of samples collected during the previous complete second.
int Sampler_getHistorySize(void){
    return num_samples_second; 
}

// Get a copy of the samples in the sample history.
// Returns a newly allocated array and sets `size` to be the
// number of elements in the returned array (output-only parameter).
// The calling code must call free() on the returned pointer.
// Note: It provides both data and size to ensure consistency.
double* Sampler_getHistory(){
    // idea is history = newBuffer;
    // pthread_mutex_lock(&historyLock);  
    double* copy = (double*)malloc((num_samples_second)*sizeof(double)); 
    for(int i = 0; i < num_samples_second; i++){
        copy[i] = history[i]; 
    }
    // pthread_mutex_unlock(&historyLock);
    return copy; 
}

// Get the average light level (not tied to the history).
double Sampler_getAverageReading(void){
    return avg_reading; 
}

// Get the total number of light level samples taken so far.
long long Sampler_getNumSamplesTaken(void){
    return num_samples_taken; 
}


void countDips(){
    double threshold = convertA2D(Sampler_getAverageReading());

    pthread_mutex_lock(&historyLock); 
    int historySize = Sampler_getHistorySize(); 
    double* historyCopy = Sampler_getHistory(); 
    pthread_mutex_unlock(&historyLock); 
    long long dips = 0; 
    for(int i = 0; i < historySize; i++){
        double sample = convertA2D(historyCopy[i]); 
        if(!dipState && sample <= threshold-0.1 ){
            dips++;
            dipState = true;
            // flag = 0; 

        }

        if(dipState && sample > threshold - 0.09){
            dipState = false; 
        }
    }

    pthread_mutex_lock(&dipLock); 
    num_dips = dips; 
    pthread_mutex_unlock(&dipLock); 

    free(historyCopy); 

}


long long getNumDips(){
    return num_dips; 
}
