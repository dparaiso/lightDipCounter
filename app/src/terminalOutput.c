#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include "terminalOutput.h"
#include "sampler.h"
#include "periodTimer.h"

pthread_t tid;

void Terminal_init() {
    pthread_create(&tid, NULL, &Terminal_startDisplay, NULL);
    // pthread_join(tid, NULL);
}
void Terminal_output() {
    pthread_cancel(tid);
}

void Terminal_startDisplay() {
    clock_t startingSecondTime = clock(); 
    while((clock()-startingSecondTime)*1000/CLOCKS_PER_SEC < 1000) {
        int sampleNum = Sampler_getHistorySize();
        //TODO: Raw value from the POT, and how many hertz (Hz) this is for the PWM (section 1.5 )
        float avgLightLvl = Sampler_getAverageReading();
        //TODO: dips
        //TODO: timing jitter info
        char temp1[] = "#Smpl/s = %d\tPOT @ %d => %dHz\tavg = %fV\tdips = %d\tSmpl ms[ %f, %f] avg %f/%d";
        clock_t startMS = clock();
        while((clock()-startMS)*1000/CLOCKS_PER_SEC < 1);
    }

}