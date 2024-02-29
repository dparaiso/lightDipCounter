// Main program to build the application
// Has main(); does initialization and cleanup and perhaps some basic logic.

#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include "segDisplay.h"
#include "sampler.h"
#include "udpListener.h"
#include "terminalOutput.h"
#include "hal/led.h"
#include "periodTimer.h"
// #include "hal/led.h"

int main()
{
    Period_init();
    segDisplay_init();
    LED_init();
    Sampler_init();
    Terminal_init();
    UDP_init(); 

    LED_cleanup(); 
    Terminal_cleanup();
    Sampler_cleanup(); 
    segDisplay_cancel();
    Period_cleanup();
    return 0; 

}