#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <string.h>
#include "terminalOutput.h"
#include "sampler.h"
#include "periodTimer.h"
#include "hal/A2D.h"
#include "hal/led.h"

static pthread_t tid;

void Terminal_init() {
    pthread_create(&tid, NULL, &Terminal_startDisplay, NULL);
    
}
void Terminal_cleanup() {
    pthread_cancel(tid);
    pthread_join(tid, NULL);
}

void* Terminal_startDisplay() {
    long long secondAhead;
    while(1) {
        secondAhead = getTimeInMs() + 1000; 
        int sampleNum = Sampler_getHistorySize();
        int potRaw = getVoltage0Reading();
        int freq = potRaw/40;
        double avgLightLvl = convertA2D(Sampler_getAverageReading());
        int dips = getNumDips();
        //TODO: timing jitter info
        Period_statistics_t pStats;
        Period_getStatisticsAndClear(PERIOD_EVENT_SAMPLE_LIGHT, &pStats);
        

        char line1[] = "#Smpl/s = %.3d\tPOT @ %.4d => %.3dHz\tavg = %.3fV\tdips = %.3d\tSmpl ms[ %.3f, %.3f] avg %.3f/%.3d\n";
        
        char line2[1024];
        strncpy(line2, "\0", 1);
        double* doubleHist = Sampler_getHistory(&sampleNum);
        int histLen = sampleNum;
        int histInc = 1;
        if(sampleNum > 20) {
            histLen = 20;
            histInc = sampleNum/20;
        }

        for(int i =0; i < histLen; i++) {
            float histVal = convertA2D(doubleHist[i*histInc]);
            char sampleStr[330];
            snprintf(sampleStr, 330, "%d:%.3f  ", i*histInc, histVal);
            strncat(line2, sampleStr, 330);
        }
        free(doubleHist);
        strncat(line2, "\n\n\0", 3);
        printf(line1, sampleNum, potRaw, freq, avgLightLvl, dips, pStats.minPeriodInMs, pStats.maxPeriodInMs, pStats.avgPeriodInMs, sampleNum);
        printf(line2);
        if(secondAhead - getTimeInMs() > 0) {
           sleepForMs(secondAhead - getTimeInMs()); 
        }
    }

}