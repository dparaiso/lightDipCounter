#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include "terminalOutput.h"
#include "sampler.h"
#include "periodTimer.h"
#include "hal/A2D.h"
#include "hal/led.h"

static pthread_t tid;

void Terminal_init() {
    pthread_create(&tid, NULL, &Terminal_startDisplay, NULL);
    // pthread_join(tid, NULL);
}
void Terminal_cleanup() {
    pthread_cancel(tid);
}

void* Terminal_startDisplay() {
    long long secondAhead;
    while(1) {
        secondAhead = getTimeInMs() + 1000; 
        int sampleNum = Sampler_getHistorySize();
        //TODO: Raw value from the POT, and how many hertz (Hz) this is for the PWM (section 1.5 )
        int potRaw = getVoltage0Reading();
        int freq = potRaw/40;
        double avgLightLvl = convertA2D(Sampler_getAverageReading());
        //TODO: dips
        int dips = getNumDips();
        //TODO: timing jitter info
        
        char line1[] = "#Smpl/s = %.3d\tPOT @ %.4d => %.3dHz\tavg = %.3fV\tdips = %.3d\tSmpl ms[ %.3f, %.3f] avg %.3f/%.3d\n\n";
        printf(line1, sampleNum, potRaw, freq, avgLightLvl, dips);
        
        if(secondAhead - getTimeInMs() > 0) {
           sleepForMs(secondAhead - getTimeInMs()); 
        }
    }

}