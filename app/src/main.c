// Main program to build the application
// Has main(); does initialization and cleanup and perhaps some basic logic.

#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include "segDisplay.h"
#include "sampler.h"
#include "udpListener.h"
// #include "hal/led.h"

int main()
{
    segDisplay_init();
    LED_init();
    Sampler_init();
    UDP_init(); 
    // UDP_cleanup();
    LED_cleanup(); 
    Sampler_cleanup(); 
    segDisplay_cancel();
    return 0; 

}