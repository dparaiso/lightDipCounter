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
double history[1200];  
long long num_samples_second = 0; 
long long num_samples_taken = 0; 
double avg_reading = 0;


// Begin/end the background thread which samples light levels.
void Sampler_init(void){
    if(pthread_create(&ptid, NULL, &Sampler_readSamples, NULL ) < 0){
        perror("pthread_create() error in Sampler"); 
        exit(1); 
    } 
}

void Sampler_cleanup(void){
    pthread_cancel(ptid); 
}

void* Sampler_readSamples(){
    while(1){
        Sampler_moveCurrentDataToHistory();
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

    clock_t startingSecondTime = clock(); 
    while((clock()-startingSecondTime)*1000/CLOCKS_PER_SEC < 1000){

        lightSample = getVoltage1Reading();

        Sampler_exponentialAvgCalculator(lightSample);
 
        history[pos] = lightSample; 
        // printf("light sample: %d\tavg: %f\tsamples: %lld\tpos: %d\n", lightSample, avg_reading, num_samples_taken, pos); 
        // printf("current pos: %d\n", pos); 
        pos++; 
        num_samples_taken++;
        clock_t startMS = clock();
        while((clock()-startMS)*1000/CLOCKS_PER_SEC < 1);
        
    }

    //potential mutex here
    // idea is history = newBuffer; 
    num_samples_second = pos; 

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
double* Sampler_getHistory(int *size){
    if (*size > num_samples_second + 1){ // corner case
        return NULL; 
    }

    double* copy = (double*)malloc((*size)*sizeof(double)); 
    for(int i = 0; i < num_samples_second; i++){
        copy[i] = history[i]; 
    }

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